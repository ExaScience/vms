#include <stdio.h>

#include "predict.h"

int mpi_world_size;
int mpi_world_rank;

#ifdef USE_MPI

#include <mpi.h>

void mpi_init() 
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);

    printf("Using mpi (rank %d out of %d)\n", mpi_world_rank, mpi_world_size);
}

void mpi_finit()
{
    MPI_Finalize();
}

void mpi_combine_results(int num_compounds, P_flx data)
{
    MPI_Allreduce(MPI_IN_PLACE, data, num_compounds * num_proteins, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
}
#else 

void mpi_init() {
    mpi_world_size = 1;
    mpi_world_rank = 0;
}
void mpi_finit() {}

void mpi_combine_results(int num_compounds,  P_flx predictions) {}
#endif
