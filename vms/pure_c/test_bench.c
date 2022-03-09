#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#ifdef USE_OPENMP
#include <omp.h>
#endif

#include "predict.h"
#include "vms_tb.h"

const float epsilon = 0.5;
const int float_size = sizeof(float) * 8;

#ifdef USE_OMPSS
#pragma oss task out(out[0;num_compounds]) label("prepare_tb_input")
#endif
void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][num_features])
{
    perf_start(__FUNCTION__);

    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c][p] = in [c%tb_num_compounds][p];

    perf_end(__FUNCTION__);
}

#ifdef USE_OMPSS
#pragma oss task out(out[0;num_compounds]) label("prepare_tb_output")
#endif
void prepare_tb_output(
    int num_compounds,
    P_base out[][num_proteins])
{
    perf_start(__FUNCTION__);

    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
            out[c][p] = 0;

    perf_end(__FUNCTION__);
}

struct Model *prepare_model(const U_arr U, const M_arr M, const B_arr B)
{
    struct Model *m = lmalloc(sizeof(struct Model), model_seg);
    memcpy(&m->U, U, sizeof(m->U));
    memcpy(&m->M, M, sizeof(m->M));
    memcpy(&m->B, B, sizeof(m->B));

    return m;
}



int check_compound(
    int c,
    P_base out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;

    for (int p = 0; p < num_proteins; p++)
    {
        float o = out[c][p];
        float r = ref[c % tb_num_compounds][p];
        if (fabs(o - r) < epsilon)
        {
            //printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
        }
        else
        {
            if (nerrors <= 10) printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
            if (nerrors == 10) printf("too many errors - stop printing\n");
            nerrors++;
        }
    }

    return nerrors;
}

static int *errors_per_compound;

int check_result(
    int num_compounds,
    P_base out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    if (mpi_world_rank != 0) return 0;

#ifdef USE_OPENMP
#pragma omp parallel for
#endif
#ifdef USE_OMPSS
#pragma oss task for in(out[0;num_compounds]) out(errors_per_compound[0;num_compounds]) \
                        node(nanos6_cluster_no_offload) chunksize(100) label("check_results")
#endif
    for (int c = 0; c < num_compounds; c++)
    {
        errors_per_compound[c] = check_compound(c, out, ref);
    }

#ifdef USE_OMPSS
#pragma oss taskwait
#endif

    int total_errors = 0;
    for (int c = 0; c < num_compounds; ++c) total_errors += errors_per_compound[c];
    printf("%d errors (out of %d)\n", total_errors, num_compounds * num_proteins);
    return total_errors;
}

int main(int argc, char *argv[])
{
    int num_repeat = 1;
    int num_compounds = 2;

    mpi_init();

    if (argc > 1 && atoi(argv[1]))
    {
        num_repeat = atoi(argv[1]);
    }

    if (argc > 2 && atoi(argv[2]))
    {
        num_compounds = atoi(argv[2]);
    }

    if (num_compounds % mpi_world_size)
        num_compounds = ((num_compounds / mpi_world_size) + 1) * mpi_world_size;

    size_t num_compounds_per_rank = num_compounds / mpi_world_size;
    size_t block_start = num_compounds_per_rank * mpi_world_rank;

    if (mpi_world_size > 1)
    {
        printf( "%d:  mpi_ncomp: %lu\n",  mpi_world_rank, num_compounds_per_rank);
        printf( "%d:  mpi_start: %lu\n",  mpi_world_rank, block_start);
    }

    P_base (*tb_output_block)[num_proteins] = (P_base (*)[num_proteins])lmalloc(sizeof(P_base) * num_compounds * num_proteins, predictions_seg);
    F_base (*tb_input_block)[num_features]  = (F_base (*)[num_features])lmalloc(sizeof(F_base) * num_compounds * num_features, features_seg); 
                        errors_per_compound =                     (int*)lmalloc(sizeof(int) * num_compounds, errors_seg);

    int nerrors = 0;

    printf("Prepare model\n");
    struct Model *m = prepare_model(U, M, B);

    if (mpi_world_rank == 0) 
    {
        printf("  dt:    %s\n", DT_NAME);
        printf("  nrep:  %d\n", num_repeat);
        printf("  nprot: %d\n", num_proteins);
        printf("  ncmpd: %d\n", num_compounds);
        printf("  nfeat: %d\n", num_features);
        printf("  nlat:  %d\n", num_latent);
        printf("  nsmpl: %d\n", num_samples);
#ifdef USE_OPENMP
        printf("  nthrds: %d\n", omp_get_max_threads());
        printf("  nnodes: %d\n", mpi_world_size);
#endif
    }

    double min_time = 1e6;
    for(int n=0; n<num_repeat; n++)
    {
        barrier();

        perf_start("main");

        double start = tick();

        if (mpi_world_rank == 0)
        {
            printf("Prepare input\n");
            prepare_tb_input(num_compounds, tb_input, tb_input_block);
            printf("Prepare output\n");
            prepare_tb_output(num_compounds, tb_output_block);
            printf("Predicting\n");
        }

        send_inputs(num_compounds, tb_input_block);
        predict_compounds(block_start, num_compounds_per_rank, tb_input_block, tb_output_block, m);
        combine_results(num_compounds, tb_output_block);
        nerrors += check_result(num_compounds, tb_output_block, tb_ref);
        double stop = tick();
        double elapsed = stop - start;
        printf("%d: iteration %d took %.2f sec; %.2f compounds/sec\n", mpi_world_rank, n, elapsed, num_compounds / elapsed);
        if (elapsed < min_time) min_time = stop-start;

        perf_end("main");

    }


    // terra-ops aka 10^12 ops
    double tops = (double)num_samples * (double)num_compounds * (double)num_latent * (double)(num_features + num_proteins) / 1e12;
    printf("%d: min time was %.2f sec; %.2f compounds/sec\n", mpi_world_rank, min_time, num_compounds / min_time);
    printf("%d: %.4f tera-ops; %.4f tera-ops/second (%d-bit floating point ops)\n",  mpi_world_rank, tops, tops/min_time, float_size);
    printf("%d: %.4f giga-ops; %.4f giga-ops/second (%d-bit floating point ops)\n",  mpi_world_rank, 1e3 * tops, 1e3 * tops/min_time, float_size);
    
    mpi_finit();

    return nerrors;
}
