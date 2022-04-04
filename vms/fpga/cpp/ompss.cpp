#include "predict.fpga.h"

#ifdef OMPSS_SMP
#pragma omp target device(smp)
#pragma omp task \
    in ([block_size*num_features]features, \
        [num_samples*num_proteins*num_latent]U,\
        [num_samples*num_latent]M,\
        [num_samples*num_features*num_latent]B) \
    out([block_size*num_proteins*num_samples]predictions)
#endif
#ifdef OMPSS_FPGA
#pragma omp target device(fpga) no_localmem_copies num_instances(3)
#pragma omp task \
    in ([block_size*num_features]features, \
        [num_samples*num_proteins*num_latent]U,\
        [num_samples*num_latent]M,\
        [num_samples*num_features*num_latent]B) \
    out([block_size*num_proteins*num_samples]predictions)
#endif
#ifdef OMPSS_TWO
#pragma oss task \
    in ([block_size*num_features]features, \
        [num_samples*num_proteins*num_latent]U,\
        [num_samples*num_latent]M,\
        [num_samples*num_features*num_latent]B) \
    out([block_size*num_proteins*num_samples]predictions)
#endif
void predict_one_block(
		const F_base *features,    //[block_size][num_features]
		      P_base *predictions, //[block_size][num_proteins]
        const int model_nr, 
        const U_base *U,           //[num_samples][num_proteins][num_latent]
        const M_base *M,           //[num_samples][num_latent]
        const B_base *B            //[num_samples][num_features][num_latent]
)
{
	load_model(model_nr, U, M, B);
	predict_dataflow(features, predictions, model_cache);
} // end function

void predict_compounds(
		int num_blocks,
		const F_blks features,    //[][num_features]
		      P_blks predictions, //[][num_proteins][num_samples]
		const Model &m)
{
    static int model_counter = 1;

    for(int i=0; i<num_blocks; i++)
    {
        printf("  Block %d/%d\n", i, num_blocks);
        predict_one_block(&features[i][0][0], &predictions[i][0][0][0],
            m.nr, &m.U[0][0][0], &m.M[0][0], &m.B[0][0][0]);
    }
#pragma omp taskwait
#pragma oss taskwait

    model_counter++;
}

