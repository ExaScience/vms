

#ifdef USE_MPI

#include <cassert>
#include <cstdio>
#include <mpi.h>

#include "predict.h"

void mpi_init(int &mpi_world_size, int &mpi_world_rank) 
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);
}

void mpi_finit()
{
    MPI_Finalize();
}

void mpi_combine_results(int num_blocks, P_blks predictions)
{
    assert(sizeof(P_base) == 1);
    unsigned num_bytes = num_blocks * block_size * num_latent * num_proteins;
    printf("reducing %u bytes\n", num_bytes);
    MPI_Allreduce(MPI_IN_PLACE, predictions, num_bytes, MPI_UNSIGNED_CHAR, MPI_SUM, MPI_COMM_WORLD);
}
#else 

void mpi_init(int &mpi_world_size, int &mpi_world_rank) {
    mpi_world_size = 1;
    mpi_world_rank = 0;
}
void mpi_finit() {}

void mpi_combine_results(int num_blocks, P_blks predictions) {}
#endif
