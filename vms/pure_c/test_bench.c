#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "predict.h"
#include "vms_tb.h"

const float epsilon = 0.5;
const int float_size = sizeof(float) * 8;

double tick() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return  (double)(t.tv_sec) + (double)(t.tv_nsec) / 1e9;
}

void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c][p] = in [c%tb_num_compounds][p];
}

void prepare_tb_output(
    int num_compounds,
    P_base out[][num_proteins])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
            out[c][p] = 0;
}

struct Model *prepare_model(const U_arr U, const M_arr M, const B_arr B)
{
    struct Model *m = lmalloc(sizeof(struct Model), model_seg);
    memcpy(&m->U, U, sizeof(m->U));
    memcpy(&m->M, M, sizeof(m->M));
    memcpy(&m->B, B, sizeof(m->B));

    return m;
}



int check_result(
    int num_compounds,
    P_base out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
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
                printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                nerrors++;
            }
        }

    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);
    return nerrors;
}

int main(int argc, char *argv[])
{
    int num_repeat = 1;
    int num_blocks = 2;

    mpi_init();

    if (argc > 1 && atoi(argv[1]))
    {
        num_repeat = atoi(argv[1]);
    }

    if (argc > 2 && atoi(argv[2]))
    {
        num_blocks = atoi(argv[2]);
    }

    if (num_blocks % mpi_world_size)
        num_blocks = ((num_blocks / mpi_world_size) + 1) * mpi_world_size;

    size_t num_compounds = num_blocks * block_size;
    size_t num_compounds_per_rank = num_compounds / mpi_world_size;
    size_t block_start = num_compounds_per_rank * mpi_world_rank;

    if (mpi_world_size > 1)
    {
        printf( "   mpi_rank:  %d\n", mpi_world_rank);
        printf( "  mpi_ncomp: %lu\n", num_compounds_per_rank);
        printf( "  mpi_start: %lu\n", block_start);
    }

    P_base (*tb_output_block)[num_proteins] = (P_base (*)[num_proteins])lmalloc(sizeof(P_base) * num_compounds * num_proteins, predictions_seg);
    F_base (*tb_input_block)[num_features]  = (F_base (*)[num_features])lmalloc(sizeof(F_base) * num_compounds * num_features, features_seg); 

    printf("  dt:    %s\n", DT_NAME);
    printf("  nrep:  %d\n", num_repeat);
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %lu\n", num_compounds);
    printf("  blks:  %d\n", block_size);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    int nerrors = 0;

    printf("Prepare input\n");
    prepare_tb_input(num_compounds, tb_input, tb_input_block);

    printf("Prepare model\n");
    struct Model *m = prepare_model(U, M, B);

    printf("Predicting\n");
    double start = tick();
    for(int n=0; n<num_repeat; n++)
    {
        prepare_tb_output(num_compounds, tb_output_block);
        predict_compounds(block_start, num_compounds_per_rank, tb_input_block, tb_output_block, m);
        mpi_combine_results(num_compounds, tb_output_block);
    }

    double stop = tick();
    nerrors += check_result(num_compounds, tb_output_block, tb_ref);
    double elapsed = stop-start;
    printf("took %.2f sec; %.2f compounds/sec\n", elapsed, num_compounds * num_repeat / elapsed);

    // terra-ops aka 10^12 ops
    double tops = (double)num_repeat * (double)num_samples * (double)num_compounds * (double)num_latent * (double)(num_features + num_proteins) / 1e12;
    printf("%.4f tera-ops; %.4f tera-ops/second (%d-bit floating point ops)\n", tops, tops/elapsed, float_size);
    printf("%.4f giga-ops; %.4f giga-ops/second (%d-bit floating point ops)\n", 1e3 * tops, 1e3 * tops/elapsed, float_size);
    
    mpi_finit();

    return nerrors;
}
