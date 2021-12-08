#include <cassert>
#include <cstdio>
#include <cstring>

#include "predict.h"

static Model model_cache;

void load_model(const int model_nr, const U_base *u, const M_base *m, const B_base *b)
{
	if (model_nr == model_cache.nr) return;
	#pragma HLS reset variable = model_cache.nr

	model_cache.nr = model_nr;

	int u_count = 0;
	int m_count = 0;
	int b_count = 0;

	for (int i = 0; i < num_samples; i++)
	{
		for (int j = 0; j < num_proteins; j++)
			for (int k = 0; k < num_latent; k++)
				model_cache.U[i][j][k] = u[u_count++];

		for (int j = 0; j < num_latent; j++)
			model_cache.M[i][j] = m[m_count++];

		for (int j = 0; j < num_features; j++)
			for (int k = 0; k < num_latent; k++)
				model_cache.B[i][j][k] = b[b_count++];
	}
}

void features_nodf_loop(
    const F_base *features)
    L_type latents[num_samples][num_latent],
    const Model &m)
{
	L_type latents_acc[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 2

	int input_count = 0;
	for (int i = 0; i < block_size; ++i)
	{
		for (int d = 0; d < num_features; d++)
		{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = m.B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = m.B complete dim = 1

#pragma HLS ARRAY_PARTITION variable = m.M complete dim = 2
#pragma HLS ARRAY_PARTITION variable = m.M complete dim = 1

			const F_type feature = features[input_count++];
			for (int s = 0; s < num_samples; s++)
				for (int k = 0; k < num_latent; k++)
				{
					L_type v;
					// first iteration of d-loop
					if (d == 0) v = M_type(m.M[s][k]);
					else        v = L_type(latents_acc[s][k]);

					L_type prod = feature * B_type(m.B[s][d][k]);
					latents_acc[s][k] = L_type(v + prod);

					if (d == num_features-1) latents[s][k] << latents_acc[s][k];
				}
		}
	}
}


void proteins_nodf_loop(
    P_base *predictions,
    L_type latents[num_samples][num_latent],
    const Model &m)
{
	L_type latents_cache[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 2

	int output_count = 0;
	for (int i = 0; i < block_size; ++i)
	{

		for (int d = 0; d < num_proteins; d++)
		{
#pragma HLS PIPELINE II=4
#pragma HLS ARRAY_PARTITION variable = m.U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = m.U complete dim = 3

			for (int s = 0; s < num_samples; s++)
			{
				S_type sum = S_type();

				for (int k = 0; k < num_latent; k++)
				{
					if (d==0) latents_cache[s][k] = latents[s][k].read();
					S_type prod = L_type(latents_cache[s][k]) * U_type(m.U[s][d][k]);
					sum = sum + prod;
				}

				predictions[output_count++] = P_type(sum);
			}
		} // end proteins
	}
}

void predict_nodf(
		const F_base *features,    //[block_size*num_features]
		      P_base *predictions, //[block_size*num_proteins*num_samples]
		const Model &m)
{
	L_type latents[num_samples][num_latent];

	features_nodf_loop(features, latents, m);
	proteins_nodf_loop(predictions, latents, m);

} // end function



