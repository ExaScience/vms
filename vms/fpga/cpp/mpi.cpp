
#include "predict.h"

#ifdef USE_MPI

#include <cassert>
#include <cstdio>
#include <mpi.h>

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
    int mpi_world_size, mpi_world_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);

    assert(sizeof(P_base) == 1);
    int blocks_per_rank = num_blocks / mpi_world_size;
    unsigned num_bytes = blocks_per_rank * block_size * num_latent * num_proteins;
    if (mpi_world_rank == 0)
    {
        for(int rank=1; rank<mpi_world_size; rank++)
        {
            void *recv_ptr = &predictions[rank * blocks_per_rank];
            MPI_Recv(recv_ptr, num_bytes, MPI_UNSIGNED_CHAR, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        void *send_ptr = &predictions[mpi_world_rank * blocks_per_rank];
        MPI_Send(send_ptr, num_bytes, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    //printf("reducing %u bytes\n", num_bytes);
    //MPI_Allreduce(MPI_IN_PLACE, predictions, num_bytes, MPI_UNSIGNED_CHAR, MPI_SUM, MPI_COMM_WORLD);
}
#else 

void mpi_init(int &mpi_world_size, int &mpi_world_rank) {
    mpi_world_size = 1;
    mpi_world_rank = 0;
}
void mpi_finit() {}

void mpi_combine_results(int num_blocks, P_blks predictions) {}
#endif
