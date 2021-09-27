#include "predict.fpga.h"

#ifdef OMPSS_SMP
#pragma omp target device(smp)
#endif
#ifdef OMPSS_FPGA
#pragma omp target device(fpga) copy_deps(U_in, M_in, B_in)
#endif
#pragma omp task \
    in ([block_size*num_features]features, m) \
    out([block_size*num_proteins*num_samples]predictions)
extern "C" void predict_one_block(
		int num_compounds,
		const F_base *features,    //[block_size][num_features]
		      P_base *predictions, //[block_size][num_proteins]
		const Model *m);

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
        predict_one_block(left, &features[i][0], &predictions[i][0][0], &m);
    }
#pragma omp taskwait

    model_counter++;
}

