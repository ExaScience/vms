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

#define CONVERT2(N, M) convert(&(N[0][0]), &(M[0][0]), sizeof(N) / sizeof(N[0][0]))
#define CONVERT3(N, M) convert(&(N[0][0][0]), &(M[0][0][0]), sizeof(N) / sizeof(N[0][0][0]))

double tick() {
    return (double)clock() / CLOCKS_PER_SEC;
}

template <typename F, typename T>
void convert(const F *in, T *out, int size)
{
    for (int i = 0; i < size; ++i)
        out[i] = in[i];
}

void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c][p] = F_base(F_type(in [c%tb_num_compounds][p]));
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
                B_out[i][j][k] = B_base(B_type(B_in[i][j][k]));
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
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {
            float o = P_type(out[c][p]);
            float r = ref[c % tb_num_compounds][p];
            if (std::abs(o - r) < epsilon)
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
    P_base U_check = 0, mu_check = 0, B_check = 0;
    update_model(U_base, mu_base, B_base, U_check, mu_check, B_check);
    printf("  Computed checksums " CRC_FMT ", " CRC_FMT ", " CRC_FMT "\n", U_check, mu_check, B_check);
    printf("  Expected checksums " CRC_FMT ", " CRC_FMT ", " CRC_FMT "\n", U_check_tb, mu_check_tb, B_check_tb);
    if (U_check != U_check_tb || mu_check != mu_check_tb || B_check != B_check_tb)
    {
        return -2;
    }


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
