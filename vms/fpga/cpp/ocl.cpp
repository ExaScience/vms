
#include <cmath>
#include <thread>
#include <iostream>

#include "xcl2.hpp"

#include "predict.h"
#include "vms_tb.h"

static const bool verbose = false;

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
            if (verbose)
            {
                std::cout << "Found Platform" << std::endl;
                std::cout << "Platform Name: " << platformName.c_str() << std::endl;
            }
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
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl;
    unsigned nargs;

    std::vector<cl::Buffer> outputArgs;
    std::vector<cl_stream> inputStreams, outputStreams;
    std::vector<std::thread> threads;


    CLData(const char *name, const unsigned char * xclbin, unsigned int xclbin_len,
        const char *emulation_mode)
    {
        cl_int err;
        if (std::string(emulation_mode) != "hw")
        {
            setenv("XCL_EMULATION_MODE", emulation_mode, 1);
            if (verbose) printf("XCL_EMULATION_MODE=%s\n", emulation_mode);
        }

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
    void addInputStream(const T *ptr, int nelem)
    {
        cl_int err;
        if (verbose)
        {
            std::cout << " input stream " << nargs << ": " << nelem << " of size " << sizeof(T)
                      << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                      << std::endl;
        }
        cl_mem_ext_ptr_t ext; // extra Xilinx-specific OpenCL parameters
        ext.param = krnl.get();
        ext.obj = NULL;
        ext.flags = nargs;

        // Create write stream for argument
        cl_stream stream;
        OCL_CHECK(err, stream = xcl::Stream::createStream(device.get(), XCL_STREAM_READ_ONLY, CL_STREAM, &ext, &err));

        // Initiating the WRITE transfer
        cl_stream_xfer_req req{0};
        req.flags = CL_STREAM_EOT;
        std::string name("input_stream_" + std::to_string(nargs));
        req.priv_data = (void *)name.c_str();

        // Thread for writing data to input stream independently in case of default blocking transfers.
        threads.push_back(std::thread(xcl::Stream::writeStream, stream, ptr, nelem * sizeof(T), &req, &err));

        nargs++;
    }

    template <typename T>
    void addOutputStream(const T *ptr, int nelem)
    {
        cl_int err;
        if (verbose)
        {
            std::cout << " output stream " << nargs << ": " << nelem << " of size " << sizeof(T)
                      << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                      << std::endl;
        }

        cl_mem_ext_ptr_t ext; // extra Xilinx-specific OpenCL parameters
        ext.param = krnl.get();
        ext.obj = NULL;
        ext.flags = nargs;

        // Create write stream for argument
        cl_stream stream;
        OCL_CHECK(err, stream = xcl::Stream::createStream(device.get(), XCL_STREAM_WRITE_ONLY, CL_STREAM, &ext, &err));

        // Initiating the WRITE transfer
        cl_stream_xfer_req req{0};
        req.flags = CL_STREAM_EOT;
        std::string name("output_stream_" + std::to_string(nargs));
        req.priv_data = (void *)name.c_str();

        // Thread for writing data to input stream independently in case of default blocking transfers.
        threads.push_back(std::thread(xcl::Stream::readStream, stream, ptr, nelem * sizeof(T), &req, &err));

        nargs++;
    }

    template <typename T>
    void addInputArg(const T *ptr, int nelem)
    {
        cl_int err;
        if (verbose)
        {
            std::cout << " input arg " << nargs << ": " << nelem << " of size " << sizeof(T)
                      << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                      << std::endl;
        }
        OCL_CHECK(err, cl::Buffer buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * nelem, (void*)ptr, &err));
        OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
    }

    template <typename T>
    void addInputArg(const T val)
    {
        cl_int err;
        OCL_CHECK(err, err = krnl.setArg(nargs++, val));
    }

    template <typename T>
    void addOutputArg(const T *ptr, int nelem)
    {
        cl_int err;
        if (verbose)
        {
            std::cout << " output arg " << nargs << ": " << nelem << " of size " << sizeof(T)
                      << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                      << std::endl;
        }
        OCL_CHECK(err, cl::Buffer buf(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * nelem, (void*)ptr, &err));
        OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
        outputArgs.push_back(buf);
    }

    void go()
    {
        cl_int err;
        OCL_CHECK(err, err = q.enqueueTask(krnl));
        for ( auto & buffer_output : outputArgs)
            OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));

        for (auto &t : threads) t.join();
        q.finish();

        nargs = 0;
        outputArgs.clear();
        inputStreams.clear();
        outputStreams.clear();
        threads.clear();
    }
};

extern unsigned char KERNEL_VAR[];
extern unsigned int KERNEL_VAR_LEN;

CLData cl_data("predict_or_update_model", KERNEL_VAR, (std::uint64_t)&KERNEL_VAR_LEN, EMULATION_MODE);

void update_model(
    const  U_arr U_in,
    const  M_arr M_in,
    const  B_arr B_in)
{
    F_base *in_block;
    P_base *out_block;

    posix_memalign((void**)&out_block, 4096, block_size*num_proteins*sizeof(P_base));
    posix_memalign((void**)&in_block,  4096, block_size*num_features*sizeof(F_base));

	int c = 0;
	for (int i=0; i<block_size; i++)
		for (int j=0; j<num_features; j++)
		{
			in_block[c] = c;
			c++;
		}


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
    // round up
    int num_blocks = (num_compounds + block_size - 1) / block_size;

    for(int c=0; c<block_size*num_blocks; c+=block_size)
    {
        if (verbose) printf("c: %d\n", c);
        int num_compounds_left = std::min(block_size, num_compounds - c);
        cl_data.addInputArg(false);
        cl_data.addInputArg(num_compounds_left);
        cl_data.addInputArg(&in[c][0], block_size*num_features);
        cl_data.addOutputArg(&out[c][0], block_size*num_proteins);
        cl_data.go();
    }
}