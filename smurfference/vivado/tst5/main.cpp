#include <cstdio>
#include <cmath>

#include "smurff_const.h"
#include "smurff_types.h"
#include "smurff_tb.h"

#include "predict.h"

int main()
{
    double tb_output[num_compounds][num_proteins];

    predict_compound_block_c(tb_input, tb_output);

    int nerrors = 0;
    for(int c=0; c<num_compounds; c++)
        for(int p=0; p<num_proteins; p++)
        {
            printf("[%d][%d]: out is %f -- ref is %f\n", c, p, tb_output[c][p], tb_ref[c][p]);
            if (std::abs(tb_output[c][p] - tb_ref[c][p]) > 0.0001)
            {
                printf("error at [%d][%d]: %f != %f\n", c, p, tb_output[c][p], tb_ref[c][p]);
                nerrors++;
            }
        }

    printf("%d errors\n", nerrors);

    return nerrors;
}
