#include <omp.h>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <cstdio>
#include <chrono>
#include <string>
#include <algorithm>

#include <boost/program_options.hpp>

#include <arrayfire.h>

#ifdef CUDA_PROFILE
#include <cuda_profiler_api.h>
#endif

#include "io.h"
#include "error.h"

typedef Eigen::Matrix< std::uint8_t, Eigen::Dynamic, Eigen::Dynamic > MatrixX8;

namespace po = boost::program_options;

double tick() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return ms / 1000.;
}

const int float_size = sizeof(Eigen::MatrixXf::Scalar) * 8;

af::array as_af(const Eigen::MatrixXf &m)
{
    return af::array(m.rows(), m.cols(), m.data());
}

struct Sample 
{
    Sample(int s, std::string modeldir, std::vector<int> devices)
    {
        Eigen::MatrixXd dF0, dM0, dU1;

        auto base = modeldir + "sample-" + std::to_string(s) + "-";
        read_matrix(base + "F0-link.ddm", dF0);
        read_matrix(base + "M0-hypermu.ddm", dM0);
        read_matrix(base + "U1-latents.ddm", dU1);

        F0 = dF0.cast<float>(); 
        M0 = dM0.cast<float>(); 
        U1 = dU1.cast<float>();

        for(int dev : devices) {
            af::setDevice(dev);
            af_F0.push_back(as_af(F0)); 
            af_M0.push_back(as_af(M0)); 
            af_U1.push_back(as_af(U1)); 
        }
    }

    std::vector<af::array> af_F0, af_M0, af_U1;
    Eigen::MatrixXf F0, M0, U1;

};

static int mpi_world_size;
static int mpi_world_rank;

#ifdef USE_MPI
void mpi_init() 
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);

    printf("Using mpi (rank %d out of %d)\n", mpi_world_rank, mpi_world_size);
}

void mpi_finit()
{
    MPI_Finalize();
}

template<typename MatrixX8>
void mpi_combine_results(MatrixX8 &ret)
{
    MPI_Allreduce(MPI_IN_PLACE, ret.data(), ret.size(), MPI_UNSIGNED_CHAR, MPI_SUM, MPI_COMM_WORLD);
}
#else 

void mpi_init() {
    mpi_world_size = 1;
    mpi_world_rank = 0;
}
void mpi_finit() {}

template<typename MatrixX8>
void mpi_combine_results(MatrixX8 &) {}
#endif

std::vector<Sample> read_model(int samples_from, int samples_to, std::string modeldir, std::vector<int> devices)
{
    std::vector<Sample> model;

    printf( "Reading model from %s\n", modeldir.c_str());

    auto sample0 = Sample(1, modeldir, devices);
    size_t nlat  = sample0.U1.rows();
    size_t nfeat = sample0.F0.cols();
    size_t nprot = sample0.U1.cols();

    printf( "  nprot: %lu\n", nprot);
    printf( "  nlat:  %lu\n", nlat);
    printf( "  nfeat: %lu\n", nfeat);


    for(int s = samples_from; s <= samples_to; s++)
        model.push_back(Sample(s, modeldir, devices));

    printf( "  nsmpl: %lu\n", model.size());

    return model;
}

Eigen::MatrixXf read_features(const std::string& ffile, int limit)
{
    Eigen::MatrixXd features;

    printf( "Reading features from %s\n", ffile.c_str());
    read_matrix(ffile, features);

    if (limit > 0 && limit < features.rows()) 
        return features.block(0, 0, limit, features.cols()).cast<float>();

    return features.cast<float>();
}

void eigen_predict_block(
    Eigen::Map<MatrixX8> ret,
    const std::vector<Sample> &model,
    const Eigen::MatrixXf &row_features,
    size_t block,
    size_t block_size,
    size_t nprot,
    std::vector<int> devices)
{
    size_t ncomp = row_features.rows();
    size_t nfeat = row_features.cols();

    // printf( "%.2f: start block %d; dev %d\n", tick(), block, dev);
    auto bs = std::min(block_size, ncomp - block);
    auto feat = row_features.block(block, 0, bs, nfeat);
    Eigen::MatrixXf pred = Eigen::MatrixXf::Zero(feat.rows(), nprot);

    for (const auto &sample : model)
    {
        auto U0 = sample.F0 * feat.transpose();
        auto U0c = U0.colwise() + sample.M0.col(0);
        pred += U0c.transpose() * sample.U1;
    }

    // final result is in 8bit unsigned
    ret.block(block, 0, bs, nprot) =  (pred / (float)(model.size())).cast<std::uint8_t>();
    
    //printf( "%.2f: end block %d; dev %d\n", tick(), block, dev);
}

void af_predict_block(
    Eigen::Map<MatrixX8> &ret,
    const std::vector<Sample> &model,
    const Eigen::MatrixXf &row_features,
    size_t block,
    size_t block_size,
    size_t nprot,
    std::vector<int> devices,
    int eval_every
    )
{
    size_t ncomp = row_features.rows();
    size_t nfeat = row_features.cols();

    auto load_block = [&row_features, nfeat, ncomp, block_size](int block) -> af::array {
        if (block >= ncomp) return af::constant(0, 0);
        auto bs = std::min(block_size, ncomp - block);
        return as_af(row_features.block(block, 0, bs, nfeat));
    };

#ifdef _OPENMP
    int dev = omp_get_thread_num();
#else
    int dev = 0;
#endif
    af::setDevice(devices[dev]);
    // printf( "%.2f: start block %d; dev %d\n", tick(), block, dev);
    auto feat = load_block(block);
    auto pred = af::constant(.0, feat.dims(0), nprot);

    int s = 0;
    for (const auto &sample : model)
    {
        auto U0 = af::matmulNT(sample.af_F0[dev], feat);
        auto U0m = af::tile(sample.af_M0[dev], 1, feat.dims(0));
        auto U0c = U0 + U0m;
        pred += af::matmulTN(U0c, sample.af_U1[dev]);
        s++; if ((s % eval_every) == 0) pred.eval();
    }

    // final result is in 8bit unsigned
    af::array out = (pred / (float)(model.size())).as(u8);

    // copy data to host
    out.host(&ret.coeffRef(block, 0));
    //printf( "%.2f: end block %d; dev %d\n", tick(), block, dev);
}

MatrixX8 predict(
        const std::vector<Sample> &model,
        const Eigen::MatrixXf &features,
        size_t block_size,
        std::vector<int> devices,
        bool use_eigen,
        int eval_every
)
{
    size_t ncomp = features.rows();
    size_t nlat  = model.at(0).U1.rows();
    size_t nfeat = model.at(0).F0.cols();
    size_t nprot = model.at(0).U1.cols();

    Eigen::Map<MatrixX8> ret(af::pinned<std::uint8_t>(ncomp*nprot), ncomp, nprot);
    ret.setZero();

    printf( "Predicting for:\n");
    printf( "  ncomp: %lu\n", ncomp);
    printf( "  bs:    %lu\n", block_size);
    if (eval_every > 0)
        printf( "  eval:  %d\n", eval_every);

    auto timer = af::timer::start();

#ifdef CUDA_PROFILE
    cudaProfilerStart();
#endif

    int ndev = devices.size();
    if (ndev > 0) 
    {
#ifdef _OPENMP
        omp_set_num_threads(ndev);
#else
        if (ndev > 1) 
            printf( "warning: No OpenMP, using only first device/core\n");
#endif
    }

    size_t ncomp_per_rank = ncomp / mpi_world_size + 1;
    size_t block_start = ncomp_per_rank * mpi_world_rank;
    size_t block_stop  = std::min(block_start + ncomp_per_rank, ncomp);
    if (mpi_world_size > 1)
    {
        printf( "  mpi_ncomp: %lu\n", ncomp_per_rank);
        printf( "  mpi_start: %lu\n", block_start);
        printf( "   mpi_stop: %lu\n", block_stop);
    }
 
#pragma omp parallel for 
    for(size_t block=block_start; block<block_stop; block+=block_size)
    {
        if (use_eigen)
            eigen_predict_block(ret, model, features, block, block_size, nprot, devices);
        else
            af_predict_block(ret, model, features, block, block_size, nprot, devices, eval_every);
    }

    mpi_combine_results(ret);

#ifdef CUDA_PROFILE
    cudaProfilerStop();
#endif

    double elapsed = timer.stop();
    double compounds_per_sec = (double)ncomp / elapsed;

    printf( "took: %.2f sec; %.2f compounds/second\n", elapsed, compounds_per_sec);

    // terra-ops aka 10^12 ops
    double gops = (double)(model.size()) * (double)ncomp * (double)nlat * (double)(nfeat + nprot) / 1e9;
    double tops = gops / 1e3;

    printf( "%.2f tera-ops; %.2f tera-ops/second (%d-bit floating point ops)\n", tops, tops/elapsed, float_size);
    printf( "%.2f giga-ops; %.2f giga-ops/second (%d-bit floating point ops)\n", gops, gops/elapsed, float_size);
    
    return ret;
}

int main(int ac, char *av[])
{
    mpi_init();

    std::vector<int> devices;
    std::string backend;

    int repeat = 1;
    int from = 1;
    int to = 100;
    int block_size = 10000;
    int compound_limit = -1;
    std::string modeldir = "model/";
    std::string features_file = "features.ddm";
    std::string out;
    bool use_eigen = false;
    int eval_every = -1;

    // Declare the supported options.
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")

        ("devices", po::value<std::vector<int>>(&devices)->multitoken(), "ArrayFire devices to use")
        ("backend", po::value<std::string>(&backend), "ArrayFire/Eigen backend to use (cuda, opencl, cpu, eigen)")
        ("eval-every", po::value<int>(&eval_every), "ArrayFire: eval() evert N samples")

        ("from", po::value<int>(&from), "Process from this sample onwards")
        ("to", po::value<int>(&to), "Process until this sample (inclusive)")
        ("limit", po::value<int>(&compound_limit), "Process at most this amount of compounds")
        ("repeat", po::value<int>(&repeat), "Repeat this many times, and report fastest")
        ("modeldir", po::value<std::string>(&modeldir), "Model directory")
        ("features", po::value<std::string>(&features_file), "Features file")
        ("block", po::value<int>(&block_size), "Process this many compounds at a time")
        ("out", po::value<std::string>(&out), "Output file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    if (backend.empty()) ;
    else if (backend == "cpu") af::setBackend(AF_BACKEND_CPU);
    else if (backend == "opencl") af::setBackend(AF_BACKEND_OPENCL);
    else if (backend == "cuda") af::setBackend(AF_BACKEND_CUDA);
    else if (backend == "eigen") use_eigen = true;
    else THROWERROR("Unknown backend: " + backend);

    if (use_eigen) 
    {
        printf("Using Eigen\n");
#ifdef _OPENMP
        int nthrd = omp_get_max_threads();
        if (devices.empty()) for(int i=0; i<nthrd; i++) devices.push_back(i);
#endif
    } 
    else
    {
        // use first GPU by default
        if (devices.empty()) devices = {0};
        af::info();
        std::cout << std::endl;
    }

    std::cout << "Using these devices (cores/GPUs):";
    for(auto dev : devices) std:: cout << " " << dev;
    std::cout << std::endl;

    auto model = read_model(from, to, modeldir,  use_eigen ? std::vector<int>() : devices);
    auto features = read_features(features_file, compound_limit);

    MatrixX8 pred;
    for(int r=0; r<repeat; r++)
    {
	    pred = predict(model, features, block_size, devices, use_eigen, eval_every);
    }

    if (!out.empty())
    {
        write_matrix(out, pred.cast<double>());
        printf( "wrote %lu x %lu predictions\n", pred.rows(), pred.cols());
    }

    mpi_finit();

    return 0;
}
