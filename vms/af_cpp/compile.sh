set -x

if [ -n "$CONDA_PREFIX" ]
then

$CXX -fopenmp -O2 -g -std=c++11 af_predict.cpp io.cpp -o af_predict -I$CONDA_PREFIX/include/eigen3 -I$CONDA_PREFIX/include/ \
    -Wl,-rpath,$CONDA_PREFIX/lib -L$CONDA_PREFIX/lib \
    -Wl,-rpath,$CONDA_PREFIX/lib64 -L$CONDA_PREFIX/lib64 \
    -laf -lboost_program_options

else

g++ -fopenmp -O2 -g -std=c++11 af_predict.cpp io.cpp -o af_predict -I/usr/local/include/eigen3 -I/opt/arrayfire/include/ \
    -Wl,-rpath,/opt/arrayfire/lib64 -L/opt/arrayfire/lib64 -laf -lboost_program_options

 fi
