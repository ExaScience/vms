
#include "predict.fpga.h"

#ifdef OMPSS_SMP
#pragma omp target device(smp) copy_deps
#endif
#ifdef OMPSS_FPGA
#pragma omp target device(smp) copy_deps
#endif
#pragma omp task \
    in([block_size*num_features]features, \
       [num_samples*num_proteins*num_latent]U_in,\
       [num_samples*num_latent]M_in,\
       [num_samples*num_features*num_latent]B_in) \
    out([block_size*num_proteins]predictions)
void predict_or_update_model(
		bool update_model,
		int num_compounds,
		const F_flat features,    //[block_size*num_features]
		      P_flat predictions, //[block_size*num_proteins]
		const U_flat U_in,        //[num_samples][num_proteins][num_latent]
		const M_flat M_in,        //[num_samples][num_latent]
		const B_flat B_in);        //[num_samples][num_features][num_latent]

void update_model(
    const  U_arr U_in,
    const  M_arr M_in,
    const  B_arr B_in)
{
    F_flat  in_block;
	int c = 0;
	for (int i=0; i<block_size; i++)
		for (int j=0; j<num_features; j++)
		{
			in_block[c] = c;
			c++;
		}

    P_flat out_block_1;
    P_flat out_block_2;

    predict_or_update_model(true, 0, in_block, out_block_1, &U_in[0][0][0], &M_in[0][0], &B_in[0][0][0]);
#pragma omp taskwait

#ifdef CHECKSUM_MODEL
	checksum_model(in_block, out_block_2, U_in, M_in, B_in);
    printf("FPGA checksums U, M, B, F: ");
	print_checksum(out_block_1);
    printf("CPU  checksums U, M, B, F: ");
	print_checksum(out_block_2);
#endif
}

void predict_compounds(int num_compounds, const F_flx in, P_flx out)
{
    const U_flat empty_U = {0};
    const M_flat empty_mu = {0};
    const B_flat empty_B = {0};

    int i;
    for(i=0; i<=num_compounds - block_size; i+=block_size)
    {
        predict_or_update_model(false, block_size, &in[i][0], &out[i][0], empty_U, empty_mu, empty_B);
    }

    // last block left-overs
    int nc = num_compounds - i;
    if (nc == 0) {
#pragma omp taskwait
    } else {
		F_flat in_block;
		P_flat out_block;

		memcpy(in_block, &in[i][0], nc*num_features*sizeof(F_base));
        predict_or_update_model(false, nc, in_block, out_block, empty_U, empty_mu, empty_B);
#pragma omp taskwait
        memcpy(&out[i][0], out_block, nc*num_proteins*sizeof(P_base));
    }
}

