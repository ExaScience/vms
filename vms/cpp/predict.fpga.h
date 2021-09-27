#include <cassert>
#include <cstdio>
#include <cstring>


#include "predict.h"
#include "stream.h"

typedef arr<P_base, num_samples> P_vec;

static U_base U_local[num_samples][num_proteins][num_latent];
static M_base M_local[num_samples][num_latent];
static B_base B_local[num_samples][num_features][num_latent];


void load_model(
		const int new_model_no,
		const U_arr U_in,   //[num_samples][num_proteins][num_latent],
		const M_arr M_in, //[num_samples][num_latent],
		const B_arr B_in)   //[num_samples][num_features][num_latent])
	{
	static int    cur_model_no = -1;

	if (new_model_no == cur_model_no) return;

	cur_model_no = new_model_no;

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
                U_local[i][j][k] = U_in[i][j][k];

        for (int j = 0; j < num_latent; j++)
           M_local[i][j] = M_in[i][j];

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
                B_local[i][j][k] = B_in[i][j][k];
    }
}


const int vec_size = 512;
const int F_el_size = sizeof(F_base) * 8;
const int F_vec_len = vec_size / F_el_size;

void input_loop(
    int num_compounds,
    hls::stream<F_base> &features_stream,
    const F_base features[block_size][num_features])
{
	const F_base *features_flat = (F_base *)&features[0][0];
	const int total = num_compounds * num_features;
	for (int i = 0; i < total; i+=F_vec_len)
	{
#pragma HLS loop_tripcount min = block_size*num_features/F_vec_len max = block_size*num_features/F_vec_len
#pragma HLS PIPELINE II = F_vec_len
		for (int j = 0; j < F_vec_len; j++)
			if (i+j < total)
				features_stream << features_flat[i+j];
	}
}

void features_loop(
    int num_compounds,
    hls::stream<F_base>& features,
    hls::stream<L_type> latents[num_samples][num_latent])
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
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 1

#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 2
#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 1

			const F_type feature = features.read();
			for (int s = 0; s < num_samples; s++)
				for (int k = 0; k < num_latent; k++)
				{
					L_type v;
					// first iteration of d-loop
					if (d == 0) v = M_type(M_local[s][k]);
					else        v = L_type(latents_acc[s][k]);

					L_type prod = feature * B_type(B_local[s][d][k]);
					latents_acc[s][k] = L_type(v + prod);

					if (d == num_features-1) latents[s][k] << latents_acc[s][k];
				}
		}
	}
}


void proteins_loop(
    int num_compounds,
    hls::stream<P_vec> &predictions,
    hls::stream<L_type> latents[num_samples][num_latent])
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
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 3

			P_vec pred;

			for (int s = 0; s < num_samples; s++)
			{
				S_type sum = S_type();

				for (int k = 0; k < num_latent; k++)
				{
					if (d==0) latents_cache[s][k] = latents[s][k].read();
					S_type prod = L_type(latents_cache[s][k]) * U_type(U_local[s][d][k]);
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
    P_arr predictions,
    hls::stream<P_vec> &predictions_stream)
{
	P_vec padding;
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
				predictions[i][d][s] = data[s];
			}
		}
	}
}


void predict_dataflow(
		int num_compounds,
		const F_arr features,    //[block_size*num_features]
		      P_arr predictions) //[block_size*num_proteins]
{
	hls::stream<L_type> latents[num_samples][num_latent];
	hls::stream<F_base> features_stream;
	hls::stream<P_vec> predictions_stream;
#pragma HLS STREAM variable = latents depth = 16
#pragma HLS STREAM variable = features_stream depth = 16
#pragma HLS STREAM variable = predictions_stream depth = 16

#pragma HLS STABLE variable = U_local
#pragma HLS STABLE variable = M_local
#pragma HLS STABLE variable = B_local
#pragma HLS STABLE variable = num_compounds
#pragma HLS DATAFLOW
	input_loop(num_compounds, features_stream, features);
	features_loop(num_compounds, features_stream, latents);
	proteins_loop(num_compounds, predictions_stream, latents);
	output_loop(num_compounds, predictions, predictions_stream);

} // end function

void predict_one_block(
		int new_model_no,
		int num_compounds,
		const F_arr features,    //[block_size*num_features]
		      P_arr predictions, //[block_size*num_proteins]
		const U_arr U_in,        //[num_samples][num_proteins][num_latent]
		const M_arr M_in,        //[num_samples][num_latent]
		const B_arr B_in)        //[num_samples][num_features][num_latent]
{
#ifndef OMPSS_FPGA
#pragma HLS INTERFACE m_axi port=features    offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=U_in        offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=M_in        offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=B_in        offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=predictions offset=slave bundle=gmem
#endif

	load_model(new_model_no, U_in, M_in, B_in);
	predict_dataflow(num_compounds, features, predictions);
} // end function


