#include <iostream>

#include "hls/linear_algebra/utils/x_hls_matrix_utils.h"
#include "hls/linear_algebra/utils/x_hls_matrix_tb_utils.h"

#include "predict_tst2.h"

int main()
{
    double input_features[num_comp][num_feat] = {
        {0.40, 0.36},
        {1.33, 0.905},
        {0.14, 0.061},
        {0.96, 1.29},
        {0.40, 0.36},
        {1.33, 0.905},
        {0.14, 0.061},
        {0.96, 1.29},
    };


    double output_pred1[num_comp][num_proteins] = {
    		{ 2.08209 , 1.97539 , 2.27278},
			{ 6.16534 , 5.80551 , 6.66045},
			{ 0.560084, 0.521615, 0.595898},
			{ 6.10232 , 5.85359 , 6.76267},
			{ 2.08209 , 1.97539 , 2.27278},
			{ 6.16534 , 5.80551 , 6.66045},
			{ 0.560084, 0.521615, 0.595898},
			{ 6.10232 , 5.85359 , 6.76267},
    };

    double output_pred2[num_comp][num_proteins];

    predict_compound_block_c(input_features, output_pred2);

    printf("output = \n");
    hls::print_matrix<num_comp, num_proteins, double, hls::NoTranspose>(output_pred2, "   ");

    // Generate error ratio and compare against threshold value
    // - LAPACK error measurement method
    // - Threshold taken from LAPACK test functions
    if ( difference_ratio<num_comp, num_proteins>(output_pred1,output_pred2) > 30.0 ) {
      printf("  --> error\n");
      return(1);
    }
    return(0);


    return 0;
}
