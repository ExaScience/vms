#include <algorithm>
#include <iostream>
#include <cmath>
#include <chrono>
#include <Eigen/Core>

#include "predict.h"

#include "smurff_const.h"
#include "smurff_tb.h"
#include "smurff_model.h"


double tick() 
{
   return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}


using namespace sample_0;
using namespace Eigen;


int main()
{
    P_type tb_output[num_compounds][num_proteins];

    printf("num_compounds = %d\n", num_compounds);
    printf("num_proteins  = %d\n", num_proteins);
    printf("num_features  = %d\n", num_features);
    printf("num_latent   = %d\n", num_latent);
    printf("num_samples   = %d\n", num_samples);

    typedef Map<Matrix<double, Dynamic, Dynamic, RowMajor>> MapT;
    typedef Map<const Matrix<double, Dynamic, Dynamic, RowMajor>> CMapT;
    
    MapT tb_output_map(&tb_output[0][0], num_compounds, num_proteins);
    CMapT tb_input_map(&tb_input[0][0], num_compounds, num_features);
    CMapT B_map(&B[0][0], num_latent, num_features);
    CMapT U_map(&U[0][0], num_latent, num_proteins);

    double start = tick();
#pragma omp parallel for
    for(int i=0; i<1000000; ++i)
        // tb_output_map = tb_input_map * B_map.transpose() * U_map;
        predict_compound_block_c(tb_input, tb_output);
    double stop = tick();

    int nerrors = 0;
    double diff_sum = .0;
    for(int c=0; c<num_compounds; c++)
        for(int p=0; p<num_proteins; p++)
        {
            double tb_val = (double)tb_output[c][p];
            double p_val = (double)P[c][p];
            double diff = std::abs(tb_val - p_val);
            diff_sum += diff;
            if (diff > epsilon)
            {
                std::cerr<< "error at [" << c << "][" << p << "]: " << tb_val << " != " <<  p_val << std::endl;
                nerrors++;
            }
        }

    printf("%f seconds\n", stop - start);
    printf("%f epsilon\n", epsilon);
    printf("%f average diff\n", diff_sum / num_compounds  / num_proteins);
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
