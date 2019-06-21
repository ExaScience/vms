#include <cstdio>
#include <cmath>
#include <iostream>

#include "predict.h"
#include "smurff_tb.h"

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

#define CONVERT2(N) convert(&(N[0][0]), &(N##_fx[0][0]), sizeof(N) / sizeof(N[0][0]))
#define CONVERT3(N) convert(&(N[0][0][0]), &(N##_fx[0][0][0]), sizeof(N) / sizeof(N[0][0][0]))

template <typename F, typename T>
void convert(const F *in, T *out, int size)
{
    for (int i = 0; i < size; ++i)
        out[i] = in[i];
}

template <typename F, typename T>
int check_result(const F out[num_compounds][num_proteins],
             const T ref[num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {
            float o = P_type(out[c][p]);
            float r = P_type(ref[c][p]);
            printf("[%d][%d]: out is %f -- ref is %f\n", c, p, o, r);
            if (std::abs(o - r) < epsilon)
            {
                printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
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

int main()
{
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %d\n", num_compounds);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    static P_base tb_output_fx[num_compounds][num_proteins];
    static F_base tb_input_fx[num_compounds][num_features];
    static U_base U_fx[num_samples][num_proteins][num_latent];
    static mu_base mu_fx[num_samples][num_latent];
    static B_base B_fx[num_samples][num_features][num_latent];

    CONVERT2(tb_input);
    CONVERT3(U);
    CONVERT2(mu);
    CONVERT3(B);

    int nerrors = 0;

    printf("Updating model\n");
    update_model(U_fx, mu_fx, B_fx);

    printf("Predicting\n");
    for(int c=0; c<num_compounds; c++)
    {
        predict_compound(tb_input_fx[c], tb_output_fx[c]);
    }
#pragma omp taskwait
    nerrors += check_result(tb_output_fx, tb_ref);

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
