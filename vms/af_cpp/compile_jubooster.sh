#!/bin/sh

module load CUDA 

set -x

g++ -fopenmp -O3 -g -std=c++11 af_predict.cpp io.cpp -o af_predict \
    -I$CONDA_PREFIX/include/eigen3 -I$CONDA_PREFIX/include/ \
    -Wl,-rpath,$CONDA_PREFIX/lib64 -L$CONDA_PREFIX/lib64 \
    -Wl,-rpath,$CONDA_PREFIX/lib -L$CONDA_PREFIX/lib \
    -lafcuda -lboost_program_options \
    -lcudart # -lcublas -lcusolver -lcufft -lnvrtc -lcusparse -lforge -lXev
    #-DCUDA_PROFILE -I/usr/local/cuda/include -L/usr/local/cuda/lib64 \
