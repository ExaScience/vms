
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <chrono>
#include <iostream>

#include "predict.h"
#include "vms_tb.h"

void print_checksum(P_flat out)
{
	P_base U_check = out[0];
	P_base M_check = out[1];
	P_base B_check = out[2];
	P_base F_check = out[3];

    printf(CRC_FMT ", " CRC_FMT ", " CRC_FMT ", " CRC_FMT  "\n", U_check, M_check, B_check, F_check);

	for (int c = 0; c < block_size * num_proteins - 3;)
	{
		assert(out[c] == U_check); c++;
		assert(out[c] == M_check); c++;
		assert(out[c] == B_check); c++;
		assert(out[c] == F_check); c++;
	}
}

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

double tick() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return ms / 1000.;
}

void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c][p] = F_base(F_type(in [c%tb_num_compounds][p]));
}

void prepare_model(
    const float U_in[num_samples][num_proteins][num_latent],
    const float M_in[num_samples][num_latent],
    const float B_in[num_samples][num_features][num_latent],
    U_base U_out[num_samples][num_proteins][num_latent],
    M_base M_out[num_samples][num_latent],
    B_base B_out[num_samples][num_features][num_latent],
    P_base &U_check,
    P_base &M_check,
    P_base &B_check
    )
{
	CRC_INIT(U_check);
	CRC_INIT(M_check);
	CRC_INIT(B_check);

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
            {
                U_out[i][j][k] = U_base(U_type(U_in[i][j][k]));
                CRC_ADD(U_check, U_out[i][j][k]);
            }

        for (int j = 0; j < num_latent; j++)
        {
            M_out[i][j] = M_base(M_type(M_in[i][j]));
            CRC_ADD(M_check, M_out[i][j]);
        }

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
            {
                B_out[i][j][k] = B_base(B_type(B_in[i][j][k]));
                CRC_ADD(B_check, B_out[i][j][k]);
            }
    }
}


int check_result(
    int num_compounds,
    const P_base out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {
            float o = P_type(out[c][p]);
            float r = ref[c % tb_num_compounds][p];
            if (std::abs(o - r) < epsilon)
            {
                ; //printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
            }
            else
            {
                printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                nerrors++;
            }
        }

    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);
    return nerrors;
}

void predict_or_update_model(
		bool update_model,
		int num_compounds,
		const F_flat features,    //[block_size*num_features]
		      P_flat predictions, //[block_size*num_proteins]
		const U_flat U_in,        //[num_samples][num_proteins][num_latent]
		const M_flat M_in,        //[num_samples][num_latent]
		const B_flat B_in)        //[num_samples][num_features][num_latent]
{

}


void update_model(
    const  U_arr U_in,
    const  M_arr M_in,
    const  B_arr B_in)
{
    F_flat  in_block;
	int c = 0;
	for (int i=0; i<block_size; i++)
		for (int j=0; j<num_features; j++)
		{
			in_block[c] = c;
			c++;
		}

    P_flat out_block_1;

    predict_or_update_model(true, 0, in_block, out_block_1, &U_in[0][0][0], &M_in[0][0], &B_in[0][0][0]);
#pragma omp taskwait

#ifdef CHECKSUM_MODEL
    P_flat out_block_2;
	checksum_model(in_block, out_block_2, U_in, M_in, B_in);
    printf("FPGA checksums U, M, B, F: ");
	print_checksum(out_block_1);
    printf("CPU  checksums U, M, B, F: ");
	print_checksum(out_block_2);
#endif
}


void predict_compounds(int num_compounds, const F_flx in, P_flx out)
{
    const U_flat empty_U = {0};
    const M_flat empty_mu = {0};
    const B_flat empty_B = {0};

    int i;
    for(i=0; i<=num_compounds - block_size; i+=block_size)
    {
        predict_or_update_model(false, block_size, &in[i][0], &out[i][0], empty_U, empty_mu, empty_B);
    }

    // last block left-overs
    int nc = num_compounds - i;
    if (nc == 0) {
#pragma omp taskwait
    } else {
		F_flat in_block;
		P_flat out_block;

		std::memcpy(in_block, &in[i][0], nc*num_features*sizeof(F_base));
        predict_or_update_model(false, nc, in_block, out_block, empty_U, empty_mu, empty_B);
#pragma omp taskwait
        std::memcpy(&out[i][0], out_block, nc*num_proteins*sizeof(P_base));
    }
}



int main(int argc, char *argv[])
{
    int num_repeat = 1;
    int num_compounds = 10;

    if (argc > 1 && std::atoi(argv[1]))
    {
        num_repeat = std::atoi(argv[1]);
    }

    if (argc > 2 && std::atoi(argv[2]))
    {
        num_compounds = std::atoi(argv[2]);
    }
    
    printf("  dt:    %s\n", DT_NAME);
    printf("  nrep:  %d\n", num_repeat);
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %d\n", num_compounds);
    printf("  blks:  %d\n", block_size);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    P_base (*tb_output_base)[num_proteins] = new P_base[num_compounds][num_proteins];
    F_base (*tb_input_base)[num_features] = new F_base[num_compounds][num_features];

    U_base (*Ub)[num_proteins][num_latent] = new U_base[num_samples][num_proteins][num_latent];
    M_base (*Mb)[num_latent]               = new M_base[num_samples][num_latent];
    B_base (*Bb)[num_features][num_latent] = new B_base[num_samples][num_features][num_latent];

    P_base  U_check_tb, M_check_tb, B_check_tb;

    prepare_tb_input(num_compounds, tb_input, tb_input_base);
    prepare_model(U, M, B, Ub, Mb, Bb, U_check_tb, M_check_tb, B_check_tb);

    int nerrors = 0;

    printf("Updating model\n");
    update_model(Ub, Mb, Bb);

    printf("Predicting\n");
    double start = tick();
    for(int n=0; n<num_repeat; n++)
    {
        predict_compounds(num_compounds, tb_input_base, tb_output_base);
    }
#pragma omp taskwait
    double stop = tick();
    nerrors += check_result(num_compounds, tb_output_base, tb_ref);
    double elapsed = stop-start;
    printf("took %.2f sec; %.2f compounds/sec\n", elapsed, num_compounds * num_repeat / elapsed);

    delete[] tb_output_base;
    delete[] tb_input_base;
    delete[] Ub;
    delete[] Mb;
    delete[] Bb;

    return nerrors;
}

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

//OCL_CHECK doesn't work if call has templatized function call
#define OCL_CHECK(error,call)                                       \
    call;                                                           \
    if (error != CL_SUCCESS) {                                      \
      printf("%s:%d Error calling " #call ", error code is: %d\n",  \
              __FILE__,__LINE__, error);                            \
      exit(EXIT_FAILURE);                                           \
    }

#include <CL/cl2.hpp>

std::vector<cl::Device> get_devices(const std::string& vendor_name) {

    size_t i;
    cl_int err;
    std::vector<cl::Platform> platforms;
    OCL_CHECK(err, err = cl::Platform::get(&platforms));
    cl::Platform platform;
    for (i  = 0 ; i < platforms.size(); i++){
        platform = platforms[i];
        OCL_CHECK(err, std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err));
        if (platformName == vendor_name){
            std::cout << "Found Platform" << std::endl;
            std::cout << "Platform Name: " << platformName.c_str() << std::endl;
            break;
        }
    }
    if (i == platforms.size()) {
        std::cout << "Error: Failed to find Xilinx platform" << std::endl;
        exit(EXIT_FAILURE);
    }
   
    //Getting ACCELERATOR Devices and selecting 1st such device 
    std::vector<cl::Device> devices;
    OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices));
    return devices;
}

struct CLData
{
    cl_int err;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl;
    int nargs;

    CLData(const char *name, const std::vector<char> xclbin)
    {
        std::vector<cl::Device> devices = get_devices("Xilinx");
        devices.resize(1);
        device = devices[0];

        OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
        cl::Program::Binaries bins{{xclbin.data(), xclbin.size()}};
        OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
        OCL_CHECK(err, krnl = cl::Kernel(program, name, &err));
        nargs = 0;
    }

    template <typename T>
    void addInputArg(const T *ptr, int nelem)
    {
        OCL_CHECK(err, cl::Buffer buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * nelem, ptr, &err));
        OCL_CHECK(err, err = krnl.setArg(nargs++, ptr));
    }

    template <typename T>
    void addInputArg(const T val)
    {
        OCL_CHECK(err, err = krnl.setArg(nargs++, val));
    }

    std::vector<cl::Buffer> outputArgs;

    template <typename T>
    void addOutputArg(const T *ptr, int nelem)
    {
        OCL_CHECK(err, cl::Buffer buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * nelem, ptr, &err));
        OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
        outputArgs.push_back(buf);
    }

    void go()
    {
        OCL_CHECK(err, err = q.enqueueTask(krnl));
        for ( auto & buffer_output : outputArgs)
            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
        q.finish();
    }
};
