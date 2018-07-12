#include <cstdio>

#include "smurff_const.h"
#include "smurff_tb.h"

#include "predict.h"

int main()
{
    double tb_output[num_compounds][num_proteins];

    printf("num_compounds = %d\n", num_compounds);
    printf("num_proteins  = %d\n", num_proteins);
    printf("num_features  = %d\n", num_features);
    printf("num_latent   = %d\n", num_latent);
    printf("um_samples   = %d\n", num_samples);

    predict_compound_block_c(tb_input, tb_output);

    int nerrors = 0;
    for(int c=0; c<num_compounds; c++)
        for(int p=0; p<num_proteins; p++)
            if (tb_output[c][p] - s1_tb_ref[c][p] > 0.0001)
            {
                printf("error at [%d][%d]: %f != %f\n", c, p, tb_output[c][p], tb_ref[c][p]);
                nerrors++;
            }

    printf("%d errors\n", nerrors);

    return nerrors;
}
