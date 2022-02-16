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

void mpi_send_compound(int compound, P_flx data) {}

void mpi_combine_results(int num_compounds, P_flx data)
{
    MPI_Allreduce(MPI_IN_PLACE, data, num_compounds * num_proteins, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
}
#elif defined(USE_GPI)

#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include <GASPI.h>
#include <GASPI_Ext.h>

static int gaspi_free() {
    gaspi_number_t queue_size, queue_max;
    gaspi_queue_size_max(&queue_max); 
    gaspi_queue_size(0, &queue_size); 
    assert(queue_size <= queue_max);
    return (queue_max - queue_size);
}

#define SUCCESS_OR_DIE(f...) \
do  { \
  const gaspi_return_t r = f; \
  if (r != GASPI_SUCCESS) { \
    printf("Error: " #f "[%s:%d]: %d\n", __FILE__, __LINE__, r);  \
    abort(); \
  } \
} while (0)


static void gaspi_checked_wait()
{
    SUCCESS_OR_DIE(gaspi_wait(0, GASPI_BLOCK));
}

static int gaspi_wait_for_queue() {
    int free = gaspi_free();
    assert(free >= 0);
    while ((free = gaspi_free()) == 0) gaspi_checked_wait(); 
    assert(free > 0);
    return free;
}

#define SUCCESS_OR_RETRY(f...) \
do  { \
  gaspi_return_t r; \
  do { \
      r = f; \
           if (r == GASPI_SUCCESS) ; \
      else if(r == GASPI_QUEUE_FULL) gaspi_wait_for_queue(); \
      else { \
        printf("Error: " #f "[%s:%d]: %d\n", __FILE__, __LINE__, r);  \
        abort(); \
      } \
  } while (r == GASPI_QUEUE_FULL); \
} while (0)

void *gaspi_malloc(int seg, size_t size)
{
    void *ptr;
    SUCCESS_OR_DIE(gaspi_segment_create(seg, size, GASPI_GROUP_ALL, GASPI_BLOCK, GASPI_MEM_UNINITIALIZED));
    SUCCESS_OR_DIE(gaspi_segment_ptr(seg, &ptr));
    return ptr;
}

void mpi_send_compound(int compound, P_flx data)
{
    int offset = compound * num_proteins * sizeof(P_base);
    int size = num_proteins * sizeof(P_base);
    SUCCESS_OR_RETRY(gaspi_write(predictions_seg, offset, 0, predictions_seg, offset, size, 0, GASPI_BLOCK));
}

void mpi_init() 
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);

    SUCCESS_OR_DIE(gaspi_proc_init(GASPI_BLOCK));

    printf("Using gaspi+mpi (rank %d out of %d)\n", mpi_world_rank, mpi_world_size);
}

void mpi_finit()
{
    gaspi_proc_term(GASPI_BLOCK);
    MPI_Finalize();
}

void mpi_combine_results(int num_compounds, P_flx data)
{
    UNUSED(num_compounds);
    UNUSED(data);
    gaspi_barrier(GASPI_GROUP_ALL,GASPI_BLOCK);
}

#else

void mpi_init() {
    mpi_world_size = 1;
    mpi_world_rank = 0;
}
void mpi_finit() {}

void mpi_send_compound(int compound, P_flx data) {}
void mpi_combine_results(int num_compounds,  P_flx predictions) {}
#endif
