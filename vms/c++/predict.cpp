#include <cstdio>
#include <cstring>


#include "predict.h"

static U_base U[num_samples][num_proteins][num_latent];
static mu_base mu[num_samples][num_latent];
static B_base B[num_samples][num_features][num_latent];

void load_model(
		const U_base U_in[num_samples][num_proteins][num_latent],
	    const mu_base mu_in[num_samples][num_latent],
	    const B_base B_in[num_samples][num_features][num_latent])
{
#ifdef USE_MEMCPY
	std::memcpy(mu, mu_in, sizeof(mu));
	std::memcpy(U, U_in, sizeof(U));
	std::memcpy(B, B_in, sizeof(B));
#else
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
#endif
}

void features_loop(
	    const F_base features[num_features],
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

		const F_type feature(features[d]);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type  v(0xdead);
				if (d==0) v = mu_type(mu[s][k]);
				else      v = latents[s][k];
				L_type prod = feature * B_type(B[s][d][k]);
				latents[s][k] = v + prod;
			}
	}
}

void proteins_loop(
	    P_base predictions[num_proteins],
		const L_type latents[num_samples][num_latent])
{
	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
		S_type sum(0);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				S_type prod = latents[s][k] * U_type(U[s][d][k]);
				sum = sum + prod;
			}

printf("sum[%d]: ", d );
		//SHOW(sum);
		predictions[d] = sum / num_samples; // >> (log_num_samples - P_shift + U_shift);
		//SHOW(predictions[d]);
	} // end proteins
}


void predict(
		const F_base  features[num_features],
		      P_base  predictions[num_proteins]
)
{
#pragma HLS DATAFLOW
		L_type latents[num_samples][num_latent];
		features_loop(features, latents);
		proteins_loop(predictions, latents);
}


void predict_or_update_model(
		const F_base  features[num_features],
		      P_base  predictions[num_proteins],
		bool  update_model,
		const U_base U_in[num_samples][num_proteins][num_latent],
		const mu_base mu_in[num_samples][num_latent],
		const B_base B_in[num_samples][num_features][num_latent])
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
