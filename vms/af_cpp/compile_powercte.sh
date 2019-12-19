set -x

LOCAL_PREFIX=$HOME/epeec/local

g++ -O3 -fopenmp -g -std=c++11 af_predict.cpp io.cpp -o af_predict \
    -I/apps/EIGEN/3.3.7/include/eigen3 \
    -I$LOCAL_PREFIX/include \
    -Wl,-rpath,$LOCAL_PREFIX/lib -L$LOCAL_PREFIX/lib \
    -Wl,-rpath,$LOCAL_PREFIX/lib64 -L$LOCAL_PREFIX/lib64 \
    -lafcuda -lboost_program_options \
    #-DCUDA_PROFILE -I/usr/local/cuda/include -L/usr/local/cuda/lib64 \
    #-lcudart # -lcublas -lcusolver -lcufft -lnvrtc -lcusparse -lforge -lXev
