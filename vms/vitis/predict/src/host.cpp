
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <chrono>
#include <iostream>

#include "predict.h"
#include "vms_tb.h"

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
        std::cout << "Error: Failed to find platforms that match vendor '" << vendor_name << "'" << std::endl;
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
    unsigned nargs;

    CLData(const char *name, const unsigned char * xclbin, unsigned int xclbin_len)
    {
        std::vector<cl::Device> devices = get_devices("Xilinx");
        devices.resize(1);
        device = devices[0];

        OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
        cl::Program::Binaries bins{{xclbin, xclbin_len}};
        OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
        OCL_CHECK(err, krnl = cl::Kernel(program, name, &err));
        nargs = 0;
    }

    template <typename T>
    void addInputArg(const T *ptr, int nelem)
    {
        std::cout << " input arg " << nargs << ": " << nelem << " of size " << sizeof(T) << std::endl;
        OCL_CHECK(err, cl::Buffer buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * nelem, (void*)ptr, &err));
        OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
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
        OCL_CHECK(err, cl::Buffer buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * nelem, (void*)ptr, &err));
        OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
        outputArgs.push_back(buf);
    }

    void go()
    {
        OCL_CHECK(err, err = q.enqueueTask(krnl));
        for ( auto & buffer_output : outputArgs)
            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
        q.finish();

        nargs = 0;
        outputArgs.clear();
    }
};

CLData cl_data("predict_or_update_model", KERNEL_VAR, KERNEL_VAR_LEN);


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

    P_flat out_block;

    cl_data.addInputArg(true);
    cl_data.addInputArg(0);
    cl_data.addInputArg(in_block, block_size*num_features);
    cl_data.addOutputArg(out_block, block_size*num_proteins);
    cl_data.addInputArg(U_in, num_samples);
    cl_data.addInputArg(M_in, num_samples);
    cl_data.addInputArg(B_in, num_samples);
    cl_data.go();


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
    assert((num_compounds % block_size) == 0);

    int i;
    for(i=0; i<num_compounds; i+=block_size)
    {
        cl_data.addInputArg(false);
        cl_data.addInputArg(block_size);
        cl_data.addInputArg(&in[i][0], block_size*num_features);
        cl_data.addOutputArg(&out[i][0], block_size*num_proteins);
        cl_data.go();
    }
}



int main(int argc, char *argv[])
{
    int num_repeat = 1;
    int num_blocks = 10;

    if (argc > 1 && std::atoi(argv[1]))
    {
        num_repeat = std::atoi(argv[1]);
    }

    if (argc > 2 && std::atoi(argv[2]))
    {
        num_blocks = std::atoi(argv[2]);
    }
    
    printf("  dt:    %s\n", DT_NAME);
    printf("  nrep:  %d\n", num_repeat);
    printf("  nprot: %d\n", num_proteins);
    printf("  blks:  %d\n", block_size);
    printf("  nblks: %d\n", num_blocks);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    int num_compounds = num_blocks * block_size;

    P_base (*tb_output_base)[num_proteins] = new P_base[num_compounds][num_proteins];
    F_base (*tb_input_base)[num_features]  = new F_base[num_compounds][num_features];

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

