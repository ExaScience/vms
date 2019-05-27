#include <cstdio>
#include <cstring>

#include "smurff_const.h"
#include "predict.h"
#include "types.h"

#include "fxp.h"

typedef fxp<int> L_type;

static U_type U[num_samples][num_proteins][num_latent];
static mu_type mu[num_samples][num_latent];
static B_type B[num_samples][num_features][num_latent];

void load_model(
		const U_type U_in[num_samples][num_proteins][num_latent],
	    const mu_type mu_in[num_samples][num_latent],
	    const B_type B_in[num_samples][num_features][num_latent])
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

		const fxp<int> feature = make_fxp(F_iwl, features[d]);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type  v;
				if (d==0) v = make_fxp(mu_iwl, mu[s][k]);
				else      v = latents[s][k];
				SHOW(v);
				SHOW(latents[s][k]);
				SHOW(feature);
				L_type prod(25, feature * make_fxp(B_iwl, B[s][d][k]));
				SHOW(prod);
				latents[s][k] = v + prod;
			}
	}
}

void proteins_loop(
	    P_type predictions[num_proteins],
		const L_type latents[num_samples][num_latent])
{
	typedef fxp<int> S_type;

	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
		S_type sum = make_fxp(14, 0);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				S_type prod(16, latents[s][k] * fxp<int>(make_fxp(U_iwl, U[s][d][k])));
				sum = sum + prod;
			}

		predictions[d] = sum.val; // >> (log_num_samples - P_shift + U_shift);
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
