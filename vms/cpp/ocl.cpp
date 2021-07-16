
#include <cmath>
#include <thread>
#include <iostream>
#include <stdexcept>

#include "xcl2.hpp"
#include "predict.h"
#include "vms_tb.h"

cl::Device get_first_device(const std::string &vendor_name = "Xilinx")
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto it = std::find_if(platforms.begin(), platforms.end(), [&vendor_name](cl::Platform &p) {
        return p.getInfo<CL_PLATFORM_NAME>() == vendor_name; 
    });

    if (it == platforms.end())
        throw std::runtime_error("Failed to find platforms that match vendor '" + vendor_name + "'");
    cl::Platform platform = *it;

    //Getting ACCELERATOR Devices and selecting 1st such device
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);

    if (devices.size() == 0)
        throw std::runtime_error("Failed to find any devices in platform '" + vendor_name + "'");

    return devices.at(0);
}

struct CLData;

struct Kernel
{
    CLData &cl_data;
    cl::Kernel krnl;

    //-- keep track of kernel executions
    struct Exec
    {
        unsigned nargs = 0;
        std::vector<cl::Buffer> inputArgs, outputArgs;
        std::vector<cl::Event> inputWait, outputWait, krnlWait;
    };

    std::vector<Exec> execs;

    Kernel(CLData &c);

    template <typename T>
    void addInputArg(const T *ptr, int nelem);

    template <typename T>
    void addInputArg(const T val);

    template <typename T>
    void addOutputArg(const T *ptr, int nelem);

    void go();
    void finish();

    cl::CommandQueue &q() const;
    cl::Context &context() const;

    Exec &cur_exec();
    Exec &new_exec();
};



struct CLData
{
    const int num_kernels = 2;
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
        if (std::string(emulation_mode) != "hw")
        {
            setenv("XCL_EMULATION_MODE", emulation_mode, 1);
            if (verbose) printf("XCL_EMULATION_MODE=%s\n", emulation_mode);
        }

        device = get_first_device();
        context = cl::Context(device);
        q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE /*| CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/);
        cl::Program::Binaries bins{{xclbin, xclbin_len}};
        program = cl::Program(context, { device }, bins);

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
    : cl_data(c), krnl(cl::Kernel(cl_data.program, cl_data.function_name))
{
    new_exec();
}

cl::CommandQueue &Kernel::q() const { return cl_data.q; }
cl::Context &Kernel::context() const { return cl_data.context; }

Kernel::Exec &Kernel::cur_exec() { return execs.back(); }
Kernel::Exec &Kernel::new_exec() { execs.push_back({}); return cur_exec(); }

template <typename T>
void Kernel::addInputArg(const T *ptr, int nelem)
{
    cl::Event inputDone;
    if (verbose)
    {
        std::cout << " input arg " << cur_exec().nargs << ": " << nelem << " of size " << sizeof(T)
                    << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                    << std::endl;
    }
    cl::Buffer buf(context(), CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * nelem, (void *)ptr);
    krnl.setArg(cur_exec().nargs++, buf);
    q().enqueueMigrateMemObjects({buf}, 0, 0, &inputDone); 
    cur_exec().inputWait.push_back(inputDone);
    cur_exec().inputArgs.push_back(buf);
}

template <typename T>
void Kernel::addInputArg(const T val)
{
    krnl.setArg(cur_exec().nargs++, val);
}

template <typename T>
void Kernel::addOutputArg(const T *ptr, int nelem)
{
    if (verbose)
    {
        std::cout << " output arg " << cur_exec().nargs << ": " << nelem << " of size " << sizeof(T)
                    << " (" << (sizeof(T) * nelem) / 1024 << "K)"
                    << std::endl;
    }
    cl::Buffer buf(cl_data.context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * nelem, (void *)ptr);
    krnl.setArg(cur_exec().nargs++, buf);
    cur_exec().outputArgs.push_back(buf);
}

void Kernel::go()
{
    cl::Event krnlDone;
    cl_data.q.enqueueTask(krnl, &cur_exec().inputWait, &krnlDone);
    cur_exec().krnlWait.push_back(krnlDone);

    for (auto &buffer_output : cur_exec().outputArgs)
    {
        cl::Event outputDone;
        q().enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST, &cur_exec().krnlWait, &outputDone);
        cur_exec().outputWait.push_back(outputDone);
    }

    new_exec();
}

void Kernel::finish()
{
    for (auto &e : execs) 
        cl::Event::waitForEvents(e.outputWait);
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