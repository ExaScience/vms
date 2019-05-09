#include <cstdio>
#include <cmath>
#include <iostream>

#include "smurff_const.h"
#include "smurff_types.h"
#include "smurff_tb.h"

#include "predict.h"

#ifdef DT_OBSERVED_DOUBLE
const char *typenames[] = { "U", "mu", "F", "P", "B", "S", "T" };
std::vector<double> values[ntypes];
#endif

int main()
{
    P_type tb_output[num_compounds][num_proteins];

    predict_compound_block_c(tb_input, tb_output);

    int nerrors = 0;
    for(int c=0; c<num_compounds; c++)
        for(int p=0; p<num_proteins; p++)
        {
            double o = (double)tb_output[c][p];
            double r = (double)tb_ref[c][p];
            // printf("[%d][%d]: out is %f -- ref is %f\n", c, p, tb_output[c][p], tb_ref[c][p]);
            if (std::abs(o - r) > 0.0001)
            {
                printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                nerrors++;
            }
        }

    printf("%d errors\n", nerrors);

#ifdef DT_OBSERVED_DOUBLE
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
