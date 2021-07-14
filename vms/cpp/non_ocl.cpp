#include "predict.fpga.h"

#ifdef OMPSS_SMP
#pragma omp target device(smp) copy_deps
#endif
#ifdef OMPSS_FPGA
#pragma omp target device(fpga) copy_deps
#endif
#pragma omp task \
    in([block_size]features, \
       [num_samples*num_proteins*num_latent]U_in,\
       [num_samples*num_latent]M_in,\
       [num_samples*num_features*num_latent]B_in) \
    out([block_size]predictions)
void predict_one_block(
		int new_model_no,
		int num_compounds,
		const F_arr features,    //[block_size][num_features]
		      P_arr predictions, //[block_size][num_proteins]
		const U_arr U_in,        //[num_samples][num_proteins][num_latent]
		const M_arr M_in,        //[num_samples][num_latent]
		const B_arr B_in);       //[num_samples][num_features][num_latent]

void predict_compounds(
		int num_compounds,
		const F_flx features,    //[][num_features]
		      P_flx predictions, //[][num_proteins]
		const U_arr U_in,        //[num_samples][num_proteins][num_latent]
		const M_arr M_in,        //[num_samples][num_latent]
		const B_arr B_in)        //[num_samples][num_features][num_latent]
{
    static int model_counter = 0;
    for(int i=0; i<num_compounds; i+=block_size)
    {
        int left = std::min(block_size, num_compounds-i);
        predict_one_block(model_counter, left, &features[i], &predictions[i], U_in, M_in, B_in);
    }
#pragma omp taskwait

    model_counter++;
}

