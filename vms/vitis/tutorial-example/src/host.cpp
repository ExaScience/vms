#include "host.hpp"
#include "vadd.sw_emu.xclbin.inc"

int main(int argc, char** argv)
{
    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;
    cl_int err;
    // Allocate Memory in Host Memory
    std::vector<int,aligned_allocator<int>> source_in1(DATA_SIZE);
    std::vector<int,aligned_allocator<int>> source_in2(DATA_SIZE);
    std::vector<int,aligned_allocator<int>> source_hw_results(DATA_SIZE);
    std::vector<int,aligned_allocator<int>> source_sw_results(DATA_SIZE);

    // Create the test data 
    for(int i = 0 ; i < DATA_SIZE ; i++){
        source_in1[i] = rand() % DATA_SIZE;
        source_in2[i] = rand() % DATA_SIZE;
        source_sw_results[i] = source_in1[i] + source_in2[i];
        source_hw_results[i] = 0;
    }

// OPENCL HOST CODE AREA START
	
// ------------------------------------------------------------------------------------
// Step 1: Get All PLATFORMS, then search for Target_Platform_Vendor (CL_PLATFORM_VENDOR)
//	   Search for Platform: Xilinx 
// Check if the current platform matches Target_Platform_Vendor
// ------------------------------------------------------------------------------------	
    std::vector<cl::Device> devices = get_devices("Xilinx");
    devices.resize(1);
    cl::Device device = devices[0];

// ------------------------------------------------------------------------------------
// Step 1: Create Context
// ------------------------------------------------------------------------------------
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
	
// ------------------------------------------------------------------------------------
// Step 1: Create Command Queue
// ------------------------------------------------------------------------------------
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

// ------------------------------------------------------------------
// Step 1: Load Binary File from disk
// ------------------------------------------------------------------		
    cl::Program::Binaries bins{{xclbin_vadd_sw_emu_xclbin, xclbin_vadd_sw_emu_xclbin_len}};
	
// -------------------------------------------------------------
// Step 1: Create the program object from the binary and program the FPGA device with it
// -------------------------------------------------------------	
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

// -------------------------------------------------------------
// Step 1: Create Kernels
// -------------------------------------------------------------
    OCL_CHECK(err, cl::Kernel krnl_vector_add(program,"vadd", &err));

// ================================================================
// Step 2: Setup Buffers and run Kernels
// ================================================================
//   o) Allocate Memory to store the results 
//   o) Create Buffers in Global Memory to store data
// ================================================================

// ------------------------------------------------------------------
// Step 2: Create Buffers in Global Memory to store data
//             o) buffer_in1 - stores source_in1
//             o) buffer_in2 - stores source_in2
//             o) buffer_ouput - stores Results
// ------------------------------------------------------------------	

// .......................................................
// Allocate Global Memory for source_in1
// .......................................................	
    OCL_CHECK(err, cl::Buffer buffer_in1   (context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, source_in1.data(), &err));
// .......................................................
// Allocate Global Memory for source_in2
// .......................................................
    OCL_CHECK(err, cl::Buffer buffer_in2   (context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, source_in2.data(), &err));
// .......................................................
// Allocate Global Memory for sourcce_hw_results
// .......................................................
    OCL_CHECK(err, cl::Buffer buffer_output(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            vector_size_bytes, source_hw_results.data(), &err));

// ============================================================================
// Step 2: Set Kernel Arguments and Run the Application
//         o) Set Kernel Arguments
//              ----------------------------------------------------
//              Kernel Argument  Description
//              ----------------------------------------------------
//              in1   (input)     --> Input Vector1
//              in2   (input)     --> Input Vector2
//              out   (output)    --> Output Vector
//              size  (input)     --> Size of Vector in Integer
//         o) Copy Input Data from Host to Global Memory on the device
//         o) Submit Kernels for Execution
//         o) Copy Results from Global Memory, device to Host
// ============================================================================	
    int size = DATA_SIZE;
    OCL_CHECK(err, err = krnl_vector_add.setArg(0, buffer_in1));
    OCL_CHECK(err, err = krnl_vector_add.setArg(1, buffer_in2));
    OCL_CHECK(err, err = krnl_vector_add.setArg(2, buffer_output));
    OCL_CHECK(err, err = krnl_vector_add.setArg(3, size));

// ------------------------------------------------------
// Step 2: Copy Input data from Host to Global Memory on the device
// ------------------------------------------------------
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},0/* 0 means from host*/));	
	
// ----------------------------------------
// Step 2: Submit Kernels for Execution
// ----------------------------------------
    OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));
	
// --------------------------------------------------
// Step 2: Copy Results from Device Global Memory to Host
// --------------------------------------------------
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));

    q.finish();
	
// OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0 ; i < DATA_SIZE ; i++){
        if (source_hw_results[i] != source_sw_results[i]){
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                << " Device result = " << source_hw_results[i] << std::endl;
            match = false;
            break;
        }
    }
	
// ============================================================================
// Step 3: Release Allocated Resources
// ============================================================================

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}

