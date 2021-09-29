#include "predict.fpga.h"

#ifdef OMPSS_SMP
#pragma omp target device(smp) copy_deps 
#endif
#ifdef OMPSS_FPGA
#pragma omp target device(fpga) copy_deps no_localmem_copies
#endif
#pragma omp task \
    in([block_size*num_features]features, \
       [num_samples*num_proteins*num_latent]U,\
       [num_samples*num_latent]M,\
       [num_samples*num_features*num_latent]B) \
    out([block_size*num_proteins*num_samples]predictions)
void predict_flat(
		int model_no,
		int num_compounds,
		const F_base *features,    //[block_size][num_features]
		      P_base *predictions, //[block_size][num_proteins][num_samples]
		const U_base *U,           //[num_samples][num_proteins][num_latent]
		const M_base *M,           //[num_samples][num_latent]
		const B_base *B)           //[num_samples][num_features][num_latent]
{
	predict_one_block(model_no, num_compounds, (const F_flx)(features), (P_flx)predictions, (const U_flx)U, (const M_flx)M, (const B_flx)B);
}

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
        predict_flat(model_counter, left, &features[i][0], &predictions[i][0][0], &U_in[0][0][0], &M_in[0][0], &B_in[0][0][0]);
    }
#pragma omp taskwait

    model_counter++;
}

