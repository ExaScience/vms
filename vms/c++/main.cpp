#include <cstdio>
#include <cmath>
#include <iostream>

#include "smurff_const.h"
#include "predict.h"
#include "smurff_tb.h"
#include "types.h"


#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = { "U", "mu", "F", "P", "B", "S", "T" };
std::vector<float> values[ntypes];
#endif

int main()
{
    printf("using datatype: %s\n", DT_NAME);
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %d\n", num_compounds);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    P_type tb_output[num_compounds][num_proteins];

    predict_compound_block_c(tb_input, tb_output, U, mu, B);

    int nerrors = 0;
    for(int c=0; c<num_compounds; c++)
        for(int p=0; p<num_proteins; p++)
        {
            float o = (float)tb_output[c][p];
            float r = (float)tb_ref[c][p];
            // printf("[%d][%d]: out is %f -- ref is %f\n", c, p, tb_output[c][p], tb_ref[c][p]);
            if (std::abs(o - r) > epsilon)
            {
                printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                nerrors++;
            }
        }

    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);

#ifdef DT_OBSERVED_FLOAT
    for(int i=0; i<ntypes; ++i) {
        std::cout << typenames[i] << "; size: " << values[i].size();
        if (values[i].size() > 0) {
            std::cout << "; min: " << *std::min_element(values[i].begin(), values[i].end())
                      << "; max: " << *std::max_element(values[i].begin(), values[i].end());
        }
        std::cout << std::endl;
    }
#endif

    return nerrors;
}
