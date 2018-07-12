#include <iostream>
#include <cmath>

#include "predict.h"

#include "smurff_const.h"
#include "smurff_tb.h"
#include "smurff_model.h"


using namespace sample_0;

#ifdef DT_OBSERVED_DOUBLE
std::vector<double> values[ntypes];
const char* typenames[] = { "U", "F", "P", "B", "S" };
#endif

int main()
{
    P_type tb_output[num_compounds][num_proteins];

    printf("num_compounds = %d\n", num_compounds);
    printf("num_proteins  = %d\n", num_proteins);
    printf("num_features  = %d\n", num_features);
    printf("num_latent   = %d\n", num_latent);
    printf("num_samples   = %d\n", num_samples);

    predict_compound_block_c(tb_input, tb_output);

    int nerrors = 0;
    for(int c=0; c<num_compounds; c++)
        for(int p=0; p<num_proteins; p++)
        {
            double tb_val = (double)tb_output[c][p];
            double p_val = (double)P[c][p];
            double diff = std::abs(tb_val - p_val);
            if (diff > epsilon)
            {
                std::cerr<< "error at [" << c << "][" << p << "]: " << tb_val << " != " <<  p_val << std::endl;
                nerrors++;
            }
        }

    printf("%d errors\n", nerrors);

#ifdef DT_OBSERVED_DOUBLE
    for(int i=0; i<ntypes; ++i) {
        std::cout << typenames[i] << "; size: " << values[i].size()
            << "; min: " << *std::min_element(values[i].begin(), values[i].end())
            << "; max: " << *std::max_element(values[i].begin(), values[i].end())
            << std::endl;
    }
#endif

    return nerrors;
}
