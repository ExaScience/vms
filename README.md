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


 ## Results

 The paper [Virtual Screening on FPGA: Performance and Energy versus Effort](https://arxiv.org/abs/2210.10386)
 contains a comparison of VMS on FPGA, CPU and GPU.

Comparison of energy performance of the VMS application implemented on an Nvidia
A100 GPU, an Intel Skylake CPU (24 cores @ 2.7Ghz) and a Xilinx Alveo U200 FPGA:


|                               | CPU  |   GPU | FPGA |
|-------------------------------|------|-------|------|
| Peak Performance (GF/s)       | 3072 | 19500 | 684  |
| Achieved Performance (GF/s)   | 402  |  3265 | 260  |
| % of Peak Performance         | 13%  |   17% | 38%  |
| Measured Power Drain (Watt)   | 205  |   200 |  37  |
| Energy Efficiency (GF/s/Watt) | 1.8  |   10  |  3   |


1 billion compounds, on a A100 GPU, would take 33 minutes:

```
N (#samples)	100
K (latent dim)	64
F (#features/compound)	500
P (#proteins)	500

Flops/compound	6400000
achieved gflops/second	3265
compounds/second	510156
mega-compounds/second	0.510
seconds/billion compounds	1960
minutes/billion compounds	33
```

