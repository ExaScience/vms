#include <cassert>
#include <cstdio>
#include <cstring>

#include <hls_stream.h>
#include "predict.h"

typedef arr<P_base, num_samples> P_vec;

static Model model_cache;
static int model_cache_nr = 0;

void load_model(const int model_nr, const U_base *u, const M_base *m, const B_base *b)
{
	if (model_nr == model_cache_nr) return;
	#pragma HLS reset variable = model_cache_nr
	#pragma HLS reset variable = model_cache.nr

	model_cache.nr = model_nr;
	model_cache_nr = model_nr;

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

const int vec_size = 512;
const int F_el_size = sizeof(F_base) * 8;
const int F_vec_len = vec_size / F_el_size;

void input_loop(
    int num_compounds,
    hls::stream<F_base> &features_stream,
    const F_base *features)
{
	const int total = num_compounds * num_features;
	int count = 0;
	for (int i = 0; i < total; i+=F_vec_len)
	{
#pragma HLS loop_tripcount min = block_size*num_features/F_vec_len max = block_size*num_features/F_vec_len
#pragma HLS PIPELINE II = F_vec_len
		for (int j = 0; j < F_vec_len; j++)
		{
			if (count < total) features_stream << features[count];
			count++;
		}
	}
}

void features_loop(
    int num_compounds,
    hls::stream<F_base>& features,
    hls::stream<L_type> latents[num_samples][num_latent],
    const Model &m)
{
	L_type latents_acc[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 2

	for (int i = 0; i < num_compounds; ++i)
	{
#pragma HLS loop_tripcount min = block_size max = block_size

		for (int d = 0; d < num_features; d++)
		{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = m.B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = m.B complete dim = 1

#pragma HLS ARRAY_PARTITION variable = m.M complete dim = 2
#pragma HLS ARRAY_PARTITION variable = m.M complete dim = 1

			const F_type feature = features.read();
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


void proteins_loop(
    int num_compounds,
    hls::stream<P_vec> &predictions,
    hls::stream<L_type> latents[num_samples][num_latent],
    const Model &m)
{
	L_type latents_cache[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 2

	for (int i = 0; i < num_compounds; ++i)
	{
#pragma HLS loop_tripcount min = block_size max = block_size

		for (int d = 0; d < num_proteins; d++)
		{
#pragma HLS PIPELINE II=4
#pragma HLS ARRAY_PARTITION variable = m.U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = m.U complete dim = 3

			P_vec pred;

			for (int s = 0; s < num_samples; s++)
			{
				S_type sum = S_type();

				for (int k = 0; k < num_latent; k++)
				{
					if (d==0) latents_cache[s][k] = latents[s][k].read();
					S_type prod = L_type(latents_cache[s][k]) * U_type(m.U[s][d][k]);
					sum = sum + prod;
				}

				pred[s] = P_type(sum);
			}

			predictions << pred;

		} // end proteins
	}
}

void output_loop(
    int num_compounds,
    P_base *predictions,
    hls::stream<P_vec> &predictions_stream)
{
	P_vec padding = {};
	int out = 0;
	// we always write block_size compounds: num_compounds + padding
	// see: https://xilinx.github.io/XRT/2021.1/html/debug-faq.html#memory-read-before-write
	for (int i = 0; i < block_size; ++i)
	{
#pragma HLS loop_tripcount min = block_size max = block_size
		for (int d = 0; d < num_proteins; d++)
		{
#pragma HLS LOOP_FLATTEN
#pragma HLS PIPELINE II = 1
			const P_vec &data = (i < num_compounds) ? predictions_stream.read() : padding;
			for (int s = 0; s < num_samples; s++)
			{
#pragma HLS UNROLL
				predictions[out] = data[s];
				out++;
				
			}
		}
	}
}


void predict_dataflow(
		int num_compounds,
		const F_base *features,    //[block_size*num_features]
		      P_base *predictions, //[block_size*num_proteins*num_samples]
		const Model &m)
{
	hls::stream<L_type> latents[num_samples][num_latent];
	hls::stream<F_base> features_stream;
	hls::stream<P_vec> predictions_stream;
#pragma HLS STREAM variable = latents depth = 16
#pragma HLS STREAM variable = features_stream depth = 16
#pragma HLS STREAM variable = predictions_stream depth = 16

#pragma HLS STABLE variable = m
#pragma HLS STABLE variable = num_compounds
#pragma HLS DATAFLOW
	input_loop(num_compounds, features_stream, features);
	features_loop(num_compounds, features_stream, latents, m);
	proteins_loop(num_compounds, predictions_stream, latents, m);
	output_loop(num_compounds, predictions, predictions_stream);

} // end function



