#include <cstdio>
#include <cmath>
#include <iostream>
#include <chrono>
#include <cstdlib>

#include "predict.h"
#include "smurff_tb.h"

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

#define CONVERT2(N, M) convert(&(N[0][0]), &(M[0][0]), sizeof(N) / sizeof(N[0][0]))
#define CONVERT3(N, M) convert(&(N[0][0][0]), &(M[0][0][0]), sizeof(N) / sizeof(N[0][0][0]))

double tick() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return ms / 1000.;
}

template <typename F, typename T>
void convert(const F *in, T *out, int size)
{
    for (int i = 0; i < size; ++i)
        out[i] = in[i];
}

template <typename F, typename T>
void convert_input_features(const F in[tb_num_compounds][num_features],
                                  T out[num_compounds][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
        {
            out[c][p] = in [c%tb_num_compounds][p];
        }
}

template <typename F, typename T>
int check_result(const F out[num_compounds][num_proteins],
                 const T ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {
            float o = P_type(out[c][p]);
            float r = P_type(ref[c % tb_num_compounds][p]);
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

    if (argc > 1 && std::atoi(argv[1]))
    {
        num_repeat = std::atoi(argv[1]);
    }

    printf("  nrep: %d\n", num_repeat);
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %d\n", num_compounds);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    static P_base  tb_output_base[num_compounds][num_proteins];
    static F_base  tb_input_base[num_compounds][num_features];
    static U_base  U_base[num_samples][num_proteins][num_latent];
    static mu_base mu_base[num_samples][num_latent];
    static B_base  B_base[num_samples][num_features][num_latent];

    static P_type  tb_output_fx[num_compounds][num_proteins];
    static F_type  tb_input_fx[num_compounds][num_features];
    static U_type  U_fx[num_samples][num_proteins][num_latent];
    static mu_type mu_fx[num_samples][num_latent];
    static B_type  B_fx[num_samples][num_features][num_latent];


    convert_input_features(tb_input, tb_input_fx);
    CONVERT3(U, U_fx);
    CONVERT2(mu, mu_fx);
    CONVERT3(B, B_fx);

    CONVERT3(U_fx, U_base);
    CONVERT2(mu_fx, mu_base);
    CONVERT3(B_fx, B_base);
    CONVERT2(tb_input_fx, tb_input_base);

    int nerrors = 0;

    printf("Updating model\n");
    update_model(U_base, mu_base, B_base);

    printf("Predicting\n");
    double start = tick();
    for(int n=0; n<num_repeat; n++)
    {
        predict_compound(tb_input_base, tb_output_base);
    }
#pragma omp taskwait
    double stop = tick();
    CONVERT2(tb_output_base, tb_output_fx);
    nerrors += check_result(tb_output_fx, tb_ref);
    double elapsed = stop-start;
    printf("took %.2f sec; %.2f compounds/sec\n", elapsed, num_compounds * num_repeat / elapsed);

#ifdef DT_OBSERVED_FLOAT
    for (int i = 0; i < ntypes; ++i)
    {
        std::cout << typenames[i] << "; size: " << values[i].size();
        if (values[i].size() > 0)
        {
            std::cout << "; min: " << *std::min_element(values[i].begin(), values[i].end())
                      << "; max: " << *std::max_element(values[i].begin(), values[i].end());
        }
        std::cout << std::endl;
    }
#endif

    return nerrors;
}
