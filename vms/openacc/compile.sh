ISYS=/home/vanderaa/local/spack/opt/spack/linux-ubuntu20.04-haswell/gcc-7.3.0/gcc-7.3.0-a5rbwnzsitibfu3dapvxsnsyphpo4lha/include/c++/7.3.0
ISYSX86=/home/vanderaa/local/spack/opt/spack/linux-ubuntu20.04-haswell/gcc-7.3.0/gcc-7.3.0-a5rbwnzsitibfu3dapvxsnsyphpo4lha/include/c++/7.3.0/x86_64-pc-linux-gnu
mcxx -isystem $ISYS -isystem $ISYSX86 -O3 -std=c++11 --ompss-2 -c test_bench.cpp
pgimcxx -isystem $ISYS -isystem $ISYSX86 -O3 -std=c++11 --ompss-2 --openacc -c predict.cpp
pgimcxx -O3 -std=c++11 --ompss-2 --openacc -o predict test_bench.o predict.o
