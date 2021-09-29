#include "predict.fpga.h"

void predict_compounds(
		int num_compounds,
		const F_flx features,    //[][num_features]
		      P_flx predictions, //[][num_proteins]
		const U_arr U,           //[num_samples][num_proteins][num_latent]
		const M_arr M,           //[num_samples][num_latent]
		const B_arr B)           //[num_samples][num_features][num_latent]
{
    static int model_counter = 0;
    for(int i=0; i<num_compounds; i+=block_size)
    {
        int left = std::min(block_size, num_compounds-i);
        predict_one_block(model_counter, left, &features[i], &predictions[i], U, M, B);
    }

    model_counter++;
}

