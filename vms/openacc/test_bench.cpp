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
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    float out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c][p] = in[c%tb_num_compounds][p];
}


int check_result(
    int num_compounds,
    const float out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {
            float o = out[c][p];
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

    float (*tb_output)[num_proteins] = new float[block_size][num_proteins];
    float (*tb_inpufloat)[num_features] = new float[block_size][num_features];

    prepare_tb_input(block_size, tb_input, tb_inpufloat);

    int nerrors = 0;

    printf("Predicting\n");
    double elapsed = 1e10;
    for (int i = 0; i< num_repeat; ++i)
    {
        double start = tick();
        predict_block(num_blocks, tb_inpufloat, tb_output, U, M, B);
        double stop = tick();
        elapsed = std::min(stop-start, elapsed);
        printf("%d: took %.2f sec; %.2f compounds/sec\n", i, stop-start, block_size * num_blocks / elapsed);
    }
    nerrors += check_result(block_size, tb_output, tb_ref);
    printf("minimum time %.2f sec; %.2f compounds/sec\n", elapsed, block_size * num_blocks / elapsed);

    // terra-ops aka 10^12 ops
    double tops = (double)num_blocks * (double)num_samples * (double)block_size * (double)num_latent * (double)(num_features + num_proteins) / 1e12;
    const int op_size = sizeof(float) * 8;

    printf("%.4f tera-ops; %.4f tera-ops/second (%d-bit float ops)\n", tops, tops/elapsed, op_size);
    printf("%.4f giga-ops; %.4f giga-ops/second (%d-bit float ops)\n", 1e3 * tops, 1e3 * tops/elapsed, op_size);

    delete[] tb_output;
    delete[] tb_inpufloat;

    return nerrors;
}
