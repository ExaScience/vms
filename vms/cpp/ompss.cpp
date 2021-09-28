#include "predict.fpga.h"

#ifdef OMPSS_SMP
#pragma omp target device(smp)
#endif
#ifdef OMPSS_FPGA
#pragma omp target device(fpga) local_mem([1]m) no_localmem_copies  
#endif
#pragma omp task \
    in ([block_size*num_features]features, [1]m) \
    out([block_size*num_proteins*num_samples]predictions)
extern "C" void predict_one_block(
		int num_compounds,
		const F_base *features,    //[block_size][num_features]
		      P_base *predictions, //[block_size][num_proteins]
        const int model_nr, 
        const U_base *u,           //[num_samples][num_proteins][num_latent]
        const M_base *m,           //[num_samples][num_latent]
        const B_base *b            //[num_samples][num_features][num_latent]
);

void predict_compounds(
		int num_compounds,
		const F_flx features,    //[][num_features]
		      P_flx predictions, //[][num_proteins][num_samples]
		const Model &m)
{
    static int model_counter = 0;

    for(int i=0; i<num_compounds; i+=block_size)
    {
        int left = std::min(block_size, num_compounds-i);
        predict_one_block(left, &features[i][0], &predictions[i][0][0],
            m.nr, &m.U[0][0][0], &m.M[0][0], &m.B[0][0][0]);
    }
#pragma omp taskwait

    model_counter++;
}

