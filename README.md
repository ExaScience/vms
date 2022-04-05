# Virtual Molecule Screening benchmark

Virtual Molecule Screening (VMS) is a computational technique used in drug
discovery that uses machine learning to predict if a chemical compound is likely to bind to a drug target.

This repo contains several implementations of VMS generated from a Matrix Factorization model built by [SMURFF](https://github.com/ExaScience/smurff)

 * vms/pure_c: pure C with OmpSs, OpenMP, MPI, GASPI, and ArgoDSM
 * vms/af_py: Python ArrayFire implementation for CUDA, OpenCL and MKL
 * vms/af_cpp: C++ ArrayFire implementation for CUDA, OpenCL and MKL
 * vms/openacc: OpenACC implementation, with and witouth OmpSs
 * vms/smurfference: Python NumPy, Tensorflow implementation
 * vms/fpga: Xilinx FPGA implementation, with OpenCL and with OmpSs@FPGA

