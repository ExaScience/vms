set -x
/usr/bin/g++ -O2 -g -std=c++11 af_predict.cpp io.cpp -o af_predict -I$CONDA_PREFIX/include/eigen3 -I$CONDA_PREFIX/include/ \
    -Wl,-rpath,$CONDA_PREFIX/lib -L$CONDA_PREFIX/lib \
    -Wl,-rpath,$CONDA_PREFIX/lib64 -L$CONDA_PREFIX/lib64 \
    -lafcuda -lboost_program_options \
    #-DCUDA_PROFILE -I/usr/local/cuda/include -L/usr/local/cuda/lib64 \
    #-lcudart # -lcublas -lcusolver -lcufft -lnvrtc -lcusparse -lforge -lXev
