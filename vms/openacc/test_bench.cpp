#include <cstdio>
#include <cmath>
#include <iostream>
#include <chrono>
#include <cstdlib>


#include "predict.h"
#include "vms_tb.h"

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

double tick() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return ms / 1000.;
}

void prepare_tb_input(
    int num_blocks,
    const float in[][num_features],
    float out[][block_size][num_features])
{
    for (int b = 0; b < num_blocks; b++)
        for (int c = 0; c < block_size; c++)
            for (int p = 0; p < num_features; p++)
                out[b][c][p] = in[c % tb_num_compounds][p];
}


int check_result(
    int num_blocks,
    const float out[][block_size][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int b = 0; b < num_blocks; b++)
        for (int c = 0; c < block_size; c++)
            for (int p = 0; p < num_proteins; p++)
            {
                float o = out[b][c][p];
                float r = ref[c % tb_num_compounds][p];
                if (std::abs(o - r) < epsilon)
                {
                    ; //printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
                }
                else
                {
                    if (nerrors < 10)
                        printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                    else if (nerrors == 10)
                        printf("More than 10 errors. Printing stopped.\n");
                    nerrors++;
                }
            }

    int num_compounds = num_blocks*block_size;
    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);
    return nerrors;
}

int main(int argc, char *argv[])
{
    int num_repeat = 3;
    int num_blocks = 10;

    if (argc > 1 && std::atoi(argv[1]))
    {
        num_repeat = std::atoi(argv[1]);
    }

    printf("  nrep:  %d\n", num_repeat);
    printf("  nblk:  %d\n", num_blocks);
    printf("  blksz: %d\n", block_size);
    printf("  nprot: %d\n", num_proteins);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    float (*tb_output_blocks)[block_size][num_proteins] = new float[num_blocks][block_size][num_proteins];
    float (*tb_input_blocks)[block_size][num_features] = new float[num_blocks][block_size][num_features];

    prepare_tb_input(num_blocks, tb_input, tb_input_blocks);

    int nerrors = 0;

    printf("Predicting\n");
    double elapsed = 1e10;
    for (int i = 0; i< num_repeat; ++i)
    {
        double start = tick();
        predict_blocks(num_blocks, tb_input_blocks, tb_output_blocks, U, M, B);
        double stop = tick();
        elapsed = std::min(stop-start, elapsed);
        printf("%d: took %.2f sec; %.2f compounds/sec\n", i, stop-start, block_size * num_blocks / elapsed);
    }
    nerrors += check_result(num_blocks, tb_output_blocks, tb_ref);
    printf("minimum time %.2f sec; %.2f compounds/sec\n", elapsed, block_size * num_blocks / elapsed);

    // terra-ops aka 10^12 ops
    double tops = (double)num_blocks * (double)num_samples * (double)block_size * (double)num_latent * (double)(num_features + num_proteins) / 1e12;
    const int op_size = sizeof(float) * 8;

    printf("%.4f tera-ops; %.4f tera-ops/second (%d-bit float ops)\n", tops, tops/elapsed, op_size);
    printf("%.4f giga-ops; %.4f giga-ops/second (%d-bit float ops)\n", 1e3 * tops, 1e3 * tops/elapsed, op_size);

    delete[] tb_output_blocks;
    delete[] tb_input_blocks;

    return nerrors;
}
