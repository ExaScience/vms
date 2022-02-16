
#ifdef USE_MPI
#include <mpi.h>
#endif

#include "predict.h"


static void features_loop(
        const F_base features[num_features],
        L_base latents[num_samples][num_latent],
		const M_base M[num_samples][num_latent],
		const B_base B[num_samples][num_features][num_latent]
		)
{
	for (int d = 0; d < num_features; d++)
	{
		const F_type feature = features[d];
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type  v;
				if (d==0) v = M[s][k];
				else      v = latents[s][k];
				L_type prod = feature * B[s][d][k];
				latents[s][k] = v + prod;
			}
	}
}

static void proteins_loop(
	P_base predictions[num_proteins],
	L_base latents[num_samples][num_latent],
    const U_base U[num_samples][num_proteins][num_latent]
	)
{
	for (int d = 0; d < num_proteins; d++)
	{
		S_type sum = .0F;
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				S_type prod = latents[s][k] * U[s][d][k];
				sum = sum + prod;
			}

		P_type aggr = sum / num_samples;
		predictions[d] = aggr;
	} // end proteins
}

int block_size = 100;

void predict_compounds(
		int start,
		int num_compounds, 
		const F_flx features,
		      P_flx predictions,
		const struct Model *m)
{

#ifdef USE_OPENMP
    #pragma omp parallel for
#endif
	for (int i=start; i<start+num_compounds; i+=block_size)
    {
#ifdef USE_OMPSS
#pragma oss task in(features[i;block_size]) in(*m) out(predictions[i;block_size])
#endif
		for(int j=i; j<i+block_size; j++)
		{
			L_base latents[num_samples][num_latent];
			features_loop(features[j], latents, m->M, m->B);
			proteins_loop(predictions[j], latents, m->U);
			mpi_send_compound(j, predictions);
		}
	}

#ifdef USE_OMPSS
#pragma oss taskwait
#endif
} // end function
