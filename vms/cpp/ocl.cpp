
#include <cmath>
#include <thread>
#include <iostream>

#include "xcl2.hpp"

#include "predict.h"
#include "vms_tb.h"

std::vector<cl::Device> get_devices(const std::string &vendor_name)
{
    size_t i;
    cl_int err;
    std::vector<cl::Platform> platforms;
    OCL_CHECK(err, err = cl::Platform::get(&platforms));
    cl::Platform platform;
    for (i = 0; i < platforms.size(); i++)
    {
        platform = platforms[i];
        OCL_CHECK(err, std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err));
        if (platformName == vendor_name)
        {
            if (verbose)
            {
                std::cout << "Found Platform" << std::endl;
                std::cout << "Platform Name: " << platformName.c_str() << std::endl;
            }
            break;
        }
    }
    if (i == platforms.size())
    {
        std::cout << "Error: Failed to find platforms that match vendor '" << vendor_name << "'" << std::endl;
        exit(EXIT_FAILURE);
    }

    //Getting ACCELERATOR Devices and selecting 1st such device
    std::vector<cl::Device> devices;
    OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices));
    return devices;
}

struct CLData;

struct Kernel
{
    unsigned nargs;
    enum { IDLE, RUNNING } state;
    CLData &cl_data;
    cl::Kernel krnl;

    std::vector<cl::Buffer> outputArgs;
    std::vector<cl_stream> inputStreams, outputStreams;
    std::vector<std::thread> threads; 

    Kernel(CLData &c);

    template <typename T>
    void addInputStream(const T *ptr, int nelem);

    template <typename T>
    void addOutputStream(const T *ptr, int nelem);

    template <typename T>
    void addInputArg(const T *ptr, int nelem);

    template <typename T>
    void addInputArg(const T val);

    template <typename T>
    void addOutputArg(const T *ptr, int nelem);

    void go();
    void finish();
};



struct CLData
{
    const int num_kernels = 1;
    int cur_kernel;

    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    std::vector<Kernel> kernels;

    const char *function_name;

    CLData(
        const char *function_name,
        const unsigned char * xclbin, unsigned int xclbin_len,
        const char *emulation_mode
    ) : function_name(function_name)
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
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
        cl::Program::Binaries bins{{xclbin, xclbin_len}};
        OCL_CHECK(err, program = cl::Program(context, devices, bins, NULL, &err));

        for(int i=0; i<num_kernels; ++i) kernels.push_back(Kernel(*this));
        cur_kernel = -1;
    }

    Kernel &get_next_kernel() {
        cur_kernel = (cur_kernel + 1) % num_kernels;
        return kernels.at(cur_kernel);
    }

    void finish()
    {
        for (auto &k : kernels) k.finish();
    }
};


Kernel::Kernel(CLData &c)
    : nargs(0), state(IDLE), cl_data(c)
{
    cl_int err;
    OCL_CHECK(err, krnl = cl::Kernel(cl_data.program, cl_data.function_name, &err));
}

template <typename T>
void Kernel::addInputStream(const T *ptr, int nelem)
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
    OCL_CHECK(err, stream = xcl::Stream::createStream(cl_data.device.get(), XCL_STREAM_READ_ONLY, CL_STREAM, &ext, &err));

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
void Kernel::addOutputStream(const T *ptr, int nelem)
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
    OCL_CHECK(err, stream = xcl::Stream::createStream(cl_data.device.get(), XCL_STREAM_WRITE_ONLY, CL_STREAM, &ext, &err));

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
void Kernel::addInputArg(const T *ptr, int nelem)
{
    cl_int err;
    if (verbose)
    {
        std::cout << " input arg " << nargs << ": " << nelem << " of size " << sizeof(T)
                    << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                    << std::endl;
    }
    OCL_CHECK(err, cl::Buffer buf(cl_data.context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * nelem, (void *)ptr, &err));
    OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
}

template <typename T>
void Kernel::addInputArg(const T val)
{
    cl_int err;
    OCL_CHECK(err, err = krnl.setArg(nargs++, val));
}

template <typename T>
void Kernel::addOutputArg(const T *ptr, int nelem)
{
    cl_int err;
    if (verbose)
    {
        std::cout << " output arg " << nargs << ": " << nelem << " of size " << sizeof(T)
                    << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                    << std::endl;
    }
    OCL_CHECK(err, cl::Buffer buf(cl_data.context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * nelem, (void *)ptr, &err));
    OCL_CHECK(err, err = krnl.setArg(nargs++, buf));
    outputArgs.push_back(buf);
}

void Kernel::go()
{
    if (state == RUNNING) finish();

    cl_int err;
    OCL_CHECK(err, err = cl_data.q.enqueueTask(krnl));
    state = RUNNING;

    finish();

}

void Kernel::finish()
{
    if (state == IDLE) return;

    cl_int err;
    for (auto &buffer_output : outputArgs)
        OCL_CHECK(err, err = cl_data.q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));

    for (auto &t : threads) t.join();
    cl_data.q.finish();

    nargs = 0;
    outputArgs.clear();
    inputStreams.clear();
    outputStreams.clear();
    threads.clear();

    state = IDLE;
}



extern unsigned char KERNEL_VAR[];
extern unsigned int KERNEL_VAR_LEN;

CLData cl_data("predict_one_block", KERNEL_VAR, (std::uint64_t)&KERNEL_VAR_LEN, EMULATION_MODE);

void predict_compounds(
               int num_compounds,
               const F_flx features,     //[block_size*num_features]
                     P_flx predictions,  //[block_size*num_proteins]
               const U_arr U_in,        //[num_samples][num_proteins][num_latent]
               const M_arr M_in,        //[num_samples][num_latent]
               const B_arr B_in)        //[num_samples][num_features][num_latent]
{
    static int model_no = 0;

    // round up
    int num_blocks = (num_compounds + block_size - 1) / block_size;


    for(int c=0; c<block_size*num_blocks; c+=block_size)
    {
        if (verbose) printf("c: %d\n", c);
        int num_compounds_left = std::min(block_size, num_compounds - c);
        auto &kernel = cl_data.get_next_kernel();
        kernel.addInputArg(model_no);
        kernel.addInputArg(num_compounds_left);
        kernel.addInputArg(&features[c][0], block_size*num_features);
        kernel.addOutputArg(&predictions[c][0], block_size*num_proteins);
        kernel.addInputArg(U_in, num_samples);
        kernel.addInputArg(M_in, num_samples);
        kernel.addInputArg(B_in, num_samples);
        kernel.go();
    }

    cl_data.finish();

    model_no++;
}