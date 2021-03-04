#include <omp.h>

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

std::vector<Sample> read_model(int samples_from, int samples_to, std::string modeldir, std::vector<int> devices)
{
    std::vector<Sample> model;

    fprintf(stderr, "Reading model from %s\n", modeldir.c_str());

    auto sample0 = Sample(1, modeldir, devices);
    size_t nlat  = sample0.U1.rows();
    size_t nfeat = sample0.F0.cols();
    size_t nprot = sample0.U1.cols();

    fprintf(stderr, "  nprot: %lu\n", nprot);
    fprintf(stderr, "  nlat:  %lu\n", nlat);
    fprintf(stderr, "  nfeat: %lu\n", nfeat);


    for(int s = samples_from; s <= samples_to; s++)
        model.push_back(Sample(s, modeldir, devices));

    fprintf(stderr, "  nsmpl: %lu\n", model.size());

    return model;
}

void eigen_predict_block(
    Eigen::Map<MatrixX8> ret,
    const std::vector<Sample> &model,
    const Eigen::Map<Eigen::MatrixXf> &row_features,
    size_t block,
    size_t blocksize,
    size_t nprot,
    std::vector<int> devices)
{
    size_t ncomp = row_features.rows();
    size_t nfeat = row_features.cols();

    // fprintf(stderr, "%.2f: start block %d; dev %d\n", tick(), block, dev);
    auto bs = std::min(blocksize, ncomp - block);
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
    
    //fprintf(stderr, "%.2f: end block %d; dev %d\n", tick(), block, dev);
}

void af_predict_block(
    Eigen::Map<MatrixX8> &ret,
    const std::vector<Sample> &model,
    const Eigen::Map<Eigen::MatrixXf> &row_features,
    size_t block,
    size_t blocksize,
    size_t nprot,
    std::vector<int> devices)
{
    size_t ncomp = row_features.rows();
    size_t nfeat = row_features.cols();

    auto load_block = [&row_features, nfeat, ncomp, blocksize](int block) -> af::array {
        if (block >= ncomp) return af::constant(0, 0);
        auto bs = std::min(blocksize, ncomp - block);
        return as_af(row_features.block(block, 0, bs, nfeat));
    };

    int dev = omp_get_thread_num();
    af::setDevice(devices[dev]);
    // fprintf(stderr, "%.2f: start block %d; dev %d\n", tick(), block, dev);
    auto feat = load_block(block);
    auto pred = af::constant(.0, feat.dims(0), nprot);

    for (const auto &sample : model)
    {
        auto U0 = af::matmulNT(sample.af_F0[dev], feat);
        auto U0m = af::tile(sample.af_M0[dev], 1, feat.dims(0));
        auto U0c = U0 + U0m;
        pred += af::matmulTN(U0c, sample.af_U1[dev]);
    }

    // final result is in 8bit unsigned
    af::array out = (pred / (float)(model.size())).as(u8);

    // copy data to host
    out.host(&ret.coeffRef(block, 0));
    //fprintf(stderr, "%.2f: end block %d; dev %d\n", tick(), block, dev);
}

MatrixX8 predict(const std::vector<Sample> &model, std::string ffile, size_t blocksize, std::vector<int> devices, bool use_eigen)
{
    Eigen::MatrixXd features;
    read_matrix(ffile, features);

    Eigen::Map<Eigen::MatrixXf> row_features(af::pinned<float>(features.nonZeros()), features.rows(), features.cols());

    row_features = features.cast<float>();

    size_t ncomp = features.rows();
    size_t nlat  = model.at(0).U1.rows();
    size_t nfeat = model.at(0).F0.cols();
    size_t nprot = model.at(0).U1.cols();

    Eigen::Map<MatrixX8> ret(af::pinned<std::uint8_t>(ncomp*nprot), ncomp, nprot);

    fprintf(stderr, "Predicting for:\n");
    fprintf(stderr, "  ncomp: %lu\n", ncomp);
    fprintf(stderr, "  bs:    %lu\n", blocksize);

    auto timer = af::timer::start();

#ifdef CUDA_PROFILE
    cudaProfilerStart();
#endif

    int ndev = devices.size();
    if (ndev > 0) omp_set_num_threads(ndev);
#pragma omp parallel for 
    for(size_t block=0; block<ncomp; block+=blocksize)
    {
        if (use_eigen)
            eigen_predict_block(ret, model, row_features, block, blocksize, nprot, devices);
        else
            af_predict_block(ret, model, row_features, block, blocksize, nprot, devices);
    }

#ifdef CUDA_PROFILE
    cudaProfilerStop();
#endif

    double elapsed = timer.stop();
    double compounds_per_sec = (double)ncomp / elapsed;

    fprintf(stderr, "took: %.2f sec; %.2f compounds/second\n", elapsed, compounds_per_sec);

    // terra-ops aka 10^12 ops
    double tops = (double)(model.size()) * (double)ncomp * (double)nlat * (double)(nfeat + nprot) / 1e12;

    fprintf(stderr, "%.2f tera-ops; %.2f tera-ops/second (%d-bit floating point ops)\n", tops, tops/elapsed, float_size);
    
    return ret;
}

int main(int ac, char *av[])
{
    std::vector<int> devices;
    std::string backend;

    int repeat = 1;
    int from = 1;
    int to = 100;
    int blocksize = 10000;
    std::string modeldir = "model/";
    std::string features = "features.ddm";
    std::string out;
    bool use_eigen = false;

    // Declare the supported options.
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")

        ("devices", po::value<std::vector<int>>(&devices)->multitoken(), "ArrayFire devices to use")
        ("backend", po::value<std::string>(&backend), "ArrayFire backend to use (cuda, opencl, cpu)")

        ("from", po::value<int>(&from), "Process from this sample onwards")
        ("to", po::value<int>(&to), "Process until this sample")
        ("repeate", po::value<int>(&repeat), "Repeat this many times, and report fastest")
        ("modeldir", po::value<std::string>(&modeldir), "Model directory")
        ("features", po::value<std::string>(&features), "Features file")
        ("block", po::value<int>(&blocksize), "Process this man compounds at a time")
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
        fprintf(stderr, "Using Eigen\n");
        devices.clear();
    } 
    else
    {
        if (devices.empty())  // use first GPU by default
            devices = {0};
        af::info();

        std::cout << "Using these devices (cores/GPUs):";
        for(auto dev : devices) std:: cout << " " << dev;
        std::cout << std::endl;
    }

    auto model = read_model(from, to, modeldir, devices);

    MatrixX8 pred;
    for(int r=0; r<repeat; r++)
    {
	    pred = predict(model, features, blocksize, devices, use_eigen);
    }

    if (!out.empty())
    {
        write_matrix(out, pred.cast<double>());
        fprintf(stderr, "wrote %lu x %lu predictions\n", pred.rows(), pred.cols());
    }

    return 0;
}
