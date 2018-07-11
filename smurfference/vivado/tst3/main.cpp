#include <iostream>

#include "hls/linear_algebra/utils/x_hls_matrix_utils.h"
#include "hls/linear_algebra/utils/x_hls_matrix_tb_utils.h"

#include "smurff_const.h"
#include "smurff_tb.h"

int main()
{
    double tb_output[tb_num_comp][tb_num_proteins];

    predict_compound_block_c(tb_input, tb_output);

    printf("output = \n");
    hls::print_matrix<num_comp, num_proteins, double, hls::NoTranspose>(output_pred2, "   ");

    // Generate error ratio and compare against threshold value
    // - LAPACK error measurement method
    // - Threshold taken from LAPACK test functions
    if ( difference_ratio<tb_num_comp, num_proteins>(tb_output,tb_ref) > 30.0 ) {
      printf("  --> error\n");
      return(1);
    }
    return(0);


    return 0;
}
