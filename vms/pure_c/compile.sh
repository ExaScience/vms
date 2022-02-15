#!/bin/bash

set -e
set -x

. /home/vanderaa/local/spack/share/spack/setup-env.sh

spack unload --all
spack load mcxx ^nanos6@cluster ^intel-oneapi-mpi
mcc -DOMPSS -O3 --ompss-2 predict.c test_bench.c ompss.c mpi.c -o predict_ompss_cluster

spack unload --all
spack load mcxx ^nanos6@argodsm ^intel-oneapi-mpi
mcc -DOMPSS -O3 --ompss-2 predict.c test_bench.c ompss.c mpi.c -o predict_ompss_argodsm

spack unload --all
spack load intel-oneapi-mpi@2021.5.1%gcc@9.3.0
gcc -DUSE_MPI -O3 predict.c test_bench.c ompss.c mpi.c -o predict_mpi -lmpi
