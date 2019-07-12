#include <cstdio>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>


#include "predict.h"
#include "vms_tb.h"

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

double tick() {
    return (double)clock() / CLOCKS_PER_SEC;
}

const int feat_limit = 1;
const int comp_limit = 1;

void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
        {
            float i = in[c%tb_num_compounds][p];
            if (p < feat_limit && c < comp_limit) 
            {
                out[c][p] = F_type(i);
                printf("F[%d][%d]: %.2f (%d)\n", c, p, i, out[c][p]);
            } else {
                out[c][p] = 0;
            }
        }
}

void prepare_model(
    const float U_in[num_samples][num_proteins][num_latent],
    const float mu_in[num_samples][num_latent],
    const float B_in[num_samples][num_features][num_latent],
    U_base U_out[num_samples][num_proteins][num_latent],
    mu_base mu_out[num_samples][num_latent],
    B_base B_out[num_samples][num_features][num_latent],
    P_base &U_check,
    P_base &mu_check,
    P_base &B_check
    )
{
	CRC_INIT(U_check);
	CRC_INIT(mu_check);
	CRC_INIT(B_check);

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
            {
                U_out[i][j][k] = U_base(U_type(U_in[i][j][k]));
                CRC_ADD(U_check, U_out[i][j][k]);
            }

        for (int j = 0; j < num_latent; j++)
        {
            mu_out[i][j] = mu_base(mu_type(mu_in[i][j]));
            CRC_ADD(mu_check, mu_out[i][j]);
        }

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
            {
                B_out[i][j][k] = B_type(B_in[i][j][k]);
                if (k == 0 && j < num_features) {
                    B_type b = B_base(1024*(j+1));
                    B_out[i][j][k] = b;
                    float f = (float)b;
                    printf("B[%d][%d][%d]: %.8f (%d)\n", i, j, k, B_out[i][j][k], f);
                } else {
                    B_out[i][j][k] = 0;
                }

                CRC_ADD(B_check, B_out[i][j][k]);
            }
    }
}

int check_result(
    int num_compounds,
    const P_base out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < comp_limit; c++)
        for (int p = 0; p < 1; p++)
        {
            float o = P_type(out[c][p]);
            printf("at [%d][%d]: %f (%d) (*8.333=%.2f)\n", c, p, o, out[c][p], o*8.333);
        }

    return nerrors;
}

int main(int argc, char *argv[])
{
    int num_repeat = 1;
    int num_compounds = 10;

    if (argc > 1 && std::atoi(argv[1]))
    {
        num_repeat = std::atoi(argv[1]);
    }

    if (argc > 2 && std::atoi(argv[2]))
    {
        num_compounds = std::atoi(argv[2]);
    }
    
    std::cout << typeid(P_type).name() << std::endl;

    printf("  dt:    %s\n", DT_NAME);
    printf("  nrep:  %d\n", num_repeat);
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %d\n", num_compounds);
    printf("  blks:  %d\n", block_size);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    P_base  tb_output_base[num_compounds][num_proteins];
    F_base  tb_input_base[num_compounds][num_features];
    U_base  U_base[num_samples][num_proteins][num_latent];
    mu_base mu_base[num_samples][num_latent];
    B_base  B_base[num_samples][num_features][num_latent];
    P_base U_check_tb, mu_check_tb, B_check_tb;

    prepare_tb_input(num_compounds, tb_input, tb_input_base);
    prepare_model(U, mu, B, U_base, mu_base, B_base, U_check_tb, mu_check_tb, B_check_tb);

    int nerrors = 0;

    printf("Updating model\n");
    update_model(U_base, mu_base, B_base);

    printf("Predicting\n");
    double start = tick();
    for(int n=0; n<num_repeat; n++)
    {
        predict_compound(num_compounds, tb_input_base, tb_output_base);
    }
#pragma omp taskwait
    double stop = tick();
    nerrors += check_result(num_compounds, tb_output_base, tb_ref);
    double elapsed = stop-start;
    printf("took %.2f sec; %.2f compounds/sec\n", elapsed, num_compounds * num_repeat / elapsed);

    return nerrors;
}
