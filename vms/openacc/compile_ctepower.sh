set -x
mcxx -O3 --ompss-2 -c test_bench.cpp
pgimcxx -O3 -std=c++11 --ompss-2 --openacc -c predict.cpp
pgimcxx -O3 -std=c++11 --ompss-2 --openacc -o predict test_bench.o predict.o
