#include <cstdio>

#include "smurff_const.h"
#include "predict.h"
#include "types.h"


static U_type U[num_samples][num_proteins][num_latent];
static mu_type mu[num_samples][num_latent];
static B_type B[num_samples][num_features][num_latent];

void load_model(
		const U_type U_in[num_samples][num_proteins][num_latent],
	    const mu_type mu_in[num_samples][num_latent],
	    const B_type B_in[num_samples][num_features][num_latent])
{
#define MEMCPY2(N) memcpy(&(N[0][0]), &(N##_in[0][0]), sizeof(N)/sizeof(N[0][0]))
#define MEMCPY3(N) memcpy(&(N[0][0][0]), &(N##_in[0][0][0]), sizeof(N)/sizeof(N[0][0][0]))

	//MEMCPY2(mu);

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
                U[i][j][k] = U_in[i][j][k];

        for (int j = 0; j < num_latent; j++)
           mu[i][j] = mu_in[i][j];

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
                B[i][j][k] = B_in[i][j][k];
    }
}

void features_loop(
	    const F_type features[num_features],
		L_type latents[num_samples][num_latent])
{
	for (int d = 0; d < num_features; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B complete dim = 1

#pragma HLS ARRAY_PARTITION variable = mu complete dim = 2
#pragma HLS ARRAY_PARTITION variable = mu complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 2

		const F_type feature = features[d];
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type v;
				if (d==0)
					v = (L_type)mu[s][k] / (L_type)(1<<mu_shift);
				else
					v = latents[s][k];

				latents[s][k] = v + feature * (L_type)B[s][d][k] / (L_type)(1<<B_shift);
			}
	}
}

void proteins_loop(
	    P_type predictions[num_proteins],
		const L_type latents[num_samples][num_latent])
{

	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
		S_type sum = .0;
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
				sum = sum + latents[s][k] * ((S_type)U[s][d][k] / (S_type)(1<<U_shift));

		predictions[d] = sum;
	} // end proteins
}


void predict(
		const F_type  features[num_features],
		      P_type  predictions[num_proteins]
)
{
#pragma HLS DATAFLOW
		L_type latents[num_samples][num_latent];
		features_loop(features, latents);
		proteins_loop(predictions, latents);
}


void predict_or_update_model(
		const F_type  features[num_features],
		      P_type  predictions[num_proteins],
		bool  update_model,
		const U_type U_in[num_samples][num_proteins][num_latent],
		const mu_type mu_in[num_samples][num_latent],
		const B_type B_in[num_samples][num_features][num_latent])
{
	if (update_model)
	{
		load_model(U_in, mu_in, B_in);
	} 
	else 
	{
		predict(features, predictions);
	}

} // end function
