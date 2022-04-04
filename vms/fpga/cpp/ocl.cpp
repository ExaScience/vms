
#include <cmath>
#include <thread>
#include <iostream>
#include <algorithm>
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
    const int kernel_no, input_bank, output_bank;

    //-- keep track of kernel executions
    struct Exec
    {
        unsigned nargs = 0;
        std::vector<cl::Buffer> inputArgs, outputArgs;
        std::vector<cl_mem_ext_ptr_t> inputExt, outputExt;
        std::vector<cl::Event> inputWait, outputWait, krnlWait;
    };

    std::vector<Exec> execs;

    Kernel(CLData &c, int k, int i, int o);

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
    const static int num_kernels = 3;
    const int input_banks[num_kernels] = { 0, 1, 3 };
    const int output_banks[num_kernels] = { 0, 1, 3 };
    int cur_kernel;

    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    std::vector<Kernel> kernels;

    const char *function_name;

    CLData(
        const char *function_name,
        const char *filename,
        const char *emulation_mode
    ) : function_name(function_name)
    {
        auto fileBuf = xcl::read_binary_file(filename);
        init(fileBuf.data(), fileBuf.size(), emulation_mode);
    }

    CLData(
        const char *function_name,
        const unsigned char * xclbin, unsigned int xclbin_len,
        const char *emulation_mode
    ) : function_name(function_name)
    {
        init(xclbin, xclbin_len, emulation_mode);
    }


    void init(
        const unsigned char *xclbin,
        unsigned int xclbin_len,
        const char *emulation_mode
    ) 
    {
        if (std::string(emulation_mode) != "hw")
        {
            setenv("XCL_EMULATION_MODE", emulation_mode, 1);
            if (verbose) printf("XCL_EMULATION_MODE=%s\n", emulation_mode);
        }

        device = get_first_device();
        context = cl::Context(device);
        q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
        cl::Program::Binaries bins{{xclbin, xclbin_len}};
        program = cl::Program(context, { device }, bins);

        for(int i=0; i<num_kernels; ++i) kernels.push_back(Kernel(*this, i, input_banks[i], output_banks[i]));
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


Kernel::Kernel(CLData &c, int k, int i, int o)
    : cl_data(c),
      krnl(cl::Kernel(cl_data.program, cl_data.function_name)),
      kernel_no(k), input_bank(i), output_bank(o)
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
        printf("  input arg %d: %d of size %ld (%luK)\n",  cur_exec().nargs, nelem, sizeof(T), (sizeof(T) * nelem) / 1024);

    cur_exec().inputExt.push_back({});
    auto &ext = cur_exec().inputExt.back();
    ext.flags = input_bank | XCL_MEM_TOPOLOGY; // DDR[input_bank]
    ext.param = 0;
    ext.obj   = (void *)ptr; 

    cl::Buffer buf(context(), CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(T) * nelem, &ext);
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
        printf("  output arg %d: %d of size %ld (%luK)\n",  cur_exec().nargs, nelem, sizeof(T), (sizeof(T) * nelem) / 1024);

    cur_exec().outputExt.push_back({});
    auto &ext = cur_exec().outputExt.back();
    ext.flags = output_bank | XCL_MEM_TOPOLOGY; // DDR[output_bank]
    ext.param = 0;
    ext.obj   = (void *)ptr;     

    cl::Buffer buf(cl_data.context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(T) * nelem, &ext);
    krnl.setArg(cur_exec().nargs++, buf);
    cur_exec().outputArgs.push_back(buf);
}

void Kernel::go()
{
    if (verbose)
        printf("  enqueueing task on kernel %d (input_bank=%d, output_bank=%d)\n", kernel_no, input_bank, output_bank);

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

#ifdef KERNEL_VAR

extern "C" unsigned char KERNEL_VAR[];
extern "C" unsigned int KERNEL_VAR_LEN;

CLData cl_data("predict_one_block", KERNEL_VAR, (std::uint64_t)&KERNEL_VAR_LEN, EMULATION_MODE);
#else

CLData cl_data("predict_one_block", KERNEL_FILENAME, EMULATION_MODE);
#endif

void predict_compounds(
               int num_blocks,
               const F_blks features,     //[block_size*num_features]
                     P_blks predictions,  //[block_size*num_proteins]
               const Model &m)
{
    for(int block=0; block<num_blocks; block++)
    {
        printf("  Block %d/%d\n", block, num_blocks);
        auto &kernel = cl_data.get_next_kernel();
        kernel.addInputArg(&features[block][0][0], block_size*num_features);
        kernel.addOutputArg(&predictions[block][0][0], block_size*num_proteins);
        kernel.addInputArg(m.nr);
        kernel.addInputArg(&m.U[0][0][0], num_samples*num_proteins*num_latent);
        kernel.addInputArg(&m.M[0][0], num_samples*num_latent);
        kernel.addInputArg(&m.B[0][0][0], num_samples*num_features*num_latent);
        kernel.go();
    }

    cl_data.finish();
}

#include "xcl2.cpp"