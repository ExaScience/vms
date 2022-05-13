#include <stdio.h>
#include <unistd.h>

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

    char hostname[1024];
    gethostname(hostname, 1024);
    printf("Using mpi (rank %d out of %d on %s)\n", mpi_world_rank, mpi_world_size, hostname);
}

void mpi_finit()
{
    MPI_Finalize();
}

void send_predictions(int compound, const P_base data[num_proteins]) {}
void send_features(int compound, const F_base data[num_features]) {}

void send_inputs(int num_compounds, F_flx features)
{
    perf_start(__FUNCTION__);

    size_t num_compounds_per_rank = num_compounds / mpi_world_size;
    size_t count = num_compounds_per_rank * num_features;

    if (mpi_world_rank == 0)
    {
        for(int rank=1; rank<mpi_world_size; rank++)
        {
            void *send_ptr = &features[rank * num_compounds_per_rank];
            MPI_Send(send_ptr, count, MPI_FLOAT, rank, 1, MPI_COMM_WORLD);
        }
    } else {
        void *recv_ptr = &features[mpi_world_rank * num_compounds_per_rank];
        MPI_Recv(recv_ptr, count, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    perf_end(__FUNCTION__);
}


void combine_results(int num_compounds, P_flx predictions)
{
    perf_start(__FUNCTION__);

    size_t num_compounds_per_rank = num_compounds / mpi_world_size;
    size_t count = num_compounds_per_rank * num_proteins;

    if (mpi_world_rank == 0)
    {
        for(int rank=1; rank<mpi_world_size; rank++)
        {
            void *recv_ptr = &predictions[rank * num_compounds_per_rank];
            MPI_Recv(recv_ptr, count, MPI_FLOAT, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        void *send_ptr = &predictions[mpi_world_rank * num_compounds_per_rank];
        MPI_Send(send_ptr, count, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
    }

    perf_end(__FUNCTION__);
}


void barrier() {
    MPI_Barrier(MPI_COMM_WORLD);
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
    printf("Error: " #f "[%s:%d]: %s\n", __FILE__, __LINE__, gaspi_error_str(r));  \
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
        printf("Error: " #f "[%s:%d]: %s\n", __FILE__, __LINE__, gaspi_error_str(r));  \
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

static int compound_received;

void send_features(int compound, const F_base data[num_features]) 
{
    if (mpi_world_rank == 0) return;


    if (compound_received >= compound)
    {
        // printf("%d: features: not waiting for compound %d, already received up to %d\n", mpi_world_rank, compound, compound_received);
        return;
    }

    perf_start(__FUNCTION__);


    // printf("%d: features: waiting for compound %d, already received up to %d\n", mpi_world_rank, compound, compound_received);

#ifdef USE_OPENMP
#pragma omp flush (compound_received)
#endif
    while (compound_received < compound)
    {
#ifdef USE_OPENMP
#pragma omp master
#endif
        {
            gaspi_notification_id_t id;
            gaspi_notification_t val = 0;
            SUCCESS_OR_DIE(gaspi_notify_waitsome(features_seg, 1, 1, &id, GASPI_BLOCK));
            SUCCESS_OR_DIE(gaspi_notify_reset(features_seg, id, &val));
            // printf("%d: features: Received notification with id %d and value %d\n", mpi_world_rank, id, val);
            compound_received = val;
        }
#ifdef USE_OPENMP
#pragma omp flush (compound_received)
#endif      
    }

    perf_end(__FUNCTION__);
}

void send_predictions(int compound, const P_base data[num_proteins])
{
    if (mpi_world_rank == 0)
        return;

    perf_start(__FUNCTION__);

    int offset = compound * num_proteins * sizeof(P_base);
    int size = num_proteins * sizeof(P_base);
    SUCCESS_OR_RETRY(gaspi_write(predictions_seg, offset, 0, predictions_seg, offset, size, 0, GASPI_BLOCK));

    perf_end(__FUNCTION__);
}

void mpi_init() 
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_world_rank);

    SUCCESS_OR_DIE(gaspi_proc_init(GASPI_BLOCK));

    char hostname[1024];
    gethostname(hostname, 1024);
    printf("Using gaspi+mpi (rank %d out of %d on %s)\n", mpi_world_rank, mpi_world_size, hostname);
}

void mpi_finit()
{
    gaspi_proc_term(GASPI_BLOCK);
    MPI_Finalize();
}

void send_inputs(int num_compounds, F_flx features)
{
    compound_received = 0;

    if (mpi_world_rank != 0) return;

    perf_start(__FUNCTION__);

    size_t num_compounds_per_rank = num_compounds / mpi_world_size;
    size_t count = num_compounds_per_rank * num_features;
    size_t block_size = 1000;

    if (mpi_world_rank == 0)
    {
        for(int rank=1; rank<mpi_world_size; rank++)
        {
            for(int c=0; c<num_compounds_per_rank; c+=block_size)
            {
                int compound = c + rank*num_compounds_per_rank;
                int offset = compound * num_features * sizeof(F_base);
                if (compound + block_size > num_compounds) block_size = num_compounds - compound;
                int size = num_features * sizeof(F_base) * block_size;
                SUCCESS_OR_RETRY(gaspi_write_notify(features_seg, offset, rank, features_seg, offset, size, 
                    1, compound + block_size, 0, GASPI_BLOCK));
                // printf("%d: features: Sent notification with id %d and value %d to %d\n", mpi_world_rank, 1, compound, rank);
            }
        }
    }

    perf_end(__FUNCTION__);
}

void combine_results(int num_compounds, P_flx data)
{
    perf_start(__FUNCTION__);

    UNUSED(num_compounds);
    UNUSED(data);
    if (mpi_world_rank != 0)
    {
        SUCCESS_OR_RETRY(gaspi_notify(predictions_seg, 0, mpi_world_rank, 1, 0, GASPI_BLOCK));
        // printf("%d: predictions: Sent notification with id %d and value %d\n", mpi_world_rank, mpi_world_rank, 1);
    }
    else
    {
        for (int k = 1; k < mpi_world_size; k++)
        {
            gaspi_notification_id_t id;
            gaspi_notification_t val = 0;
            SUCCESS_OR_DIE(gaspi_notify_waitsome(predictions_seg, 1, mpi_world_size-1, &id, GASPI_BLOCK));
            SUCCESS_OR_DIE(gaspi_notify_reset(predictions_seg, id, &val));
            // printf("%d: predictions: Collected %d-th notification with id %d and value %d\n", mpi_world_rank, k, id, val);
        }
    }

    perf_end(__FUNCTION__);
}

void barrier() {
    gaspi_barrier(GASPI_GROUP_ALL, GASPI_BLOCK);
}

#elif defined(USE_ARGO)

#include "argo.h"

void mpi_init() {
    argo_init(256*1024*1024UL,
              256*1024*1024UL);

    mpi_world_size = argo_number_of_nodes();
    mpi_world_rank = argo_node_id();
}

void mpi_finit() {
    argo_finalize();
}

void barrier() {
    argo_barrier(1);
}

void send_predictions(int compound, const P_base data[num_proteins]) {}
void send_features(int compound, const F_base data[num_features]) {}

void send_inputs(int num_compounds,  F_flx features) { barrier(); }
void combine_results(int num_compounds,  P_flx predictions) { barrier(); }

#else

void mpi_init() {
    mpi_world_size = 1;
    mpi_world_rank = 0;
}

void mpi_finit() {}

void barrier() {
#ifdef USE_OMPSS
#pragma oss taskwait
#endif
}

void send_predictions(int compound, const P_base data[num_proteins]) {}
void send_features(int compound, const F_base data[num_features]) {}

void send_inputs(int num_compounds,  F_flx features) {}
void combine_results(int num_compounds,  P_flx predictions) {}
#endif
