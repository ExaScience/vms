#include <cassert>
#include <cstdio>
#include <cstring>

#include "predict.h"
#include "stream.h"

static U_base U_local[num_samples][num_proteins][num_latent];
static M_base M_local[num_samples][num_latent];
static B_base B_local[num_samples][num_features][num_latent];

void checksum_model(const F_arr features,
		    P_arr out,
		    const U_arr U,
		    const M_arr M,
		    const B_arr B)
{
	P_base U_check;
	P_base M_check;
	P_base B_check;
	P_base F_check;

	CRC_INIT(U_check);
	CRC_INIT(M_check);
	CRC_INIT(B_check);
	CRC_INIT(F_check);

        for (int i = 0; i < num_samples; i++)
        {
            for (int j = 0; j < num_proteins; j++)
                for (int k = 0; k < num_latent; k++)
                    CRC_ADD(U_check, U[i][j][k]);

            for (int j = 0; j < num_latent; j++)
                CRC_ADD(M_check, M[i][j]);

            for (int j = 0; j < num_features; j++)
                for (int k = 0; k < num_latent; k++)
                    CRC_ADD(B_check, B[i][j][k]);
        }

        {
            int c = 0;
            for (int i = 0; i < block_size; i++)
                for (int j = 0; j < num_features; j++)
                    CRC_ADD(F_check, features[i][j]);
        }

        {
            for (int c = 0; c < num_proteins - 3;)
            {
                out[0][c++] = U_check;
                out[0][c++] = M_check;
                out[0][c++] = B_check;
                out[0][c++] = F_check;
            }
        }
}



void load_model(
		const U_flat U_in,   //[num_samples][num_proteins][num_latent],
		const M_flat M_in, //[num_samples][num_latent],
		const B_flat B_in)   //[num_samples][num_features][num_latent])
	{
#ifdef USE_MEMCPY
	std::memcpy(M_local, M_in, sizeof(M_local));
	std::memcpy(U_local, U_in, sizeof(U_local));
	std::memcpy(B_local, B_in, sizeof(B_local));
#else
	int f = 0;
	int g = 0;
	int h = 0;
    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
                U_local[i][j][k] = U_in[f++];

        for (int j = 0; j < num_latent; j++)
           M_local[i][j] = M_in[g++];

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
                B_local[i][j][k] = B_in[h++];
    }
#endif
}


void input_loop(
    int num_compounds,
    hls::stream<F_base> &features_stream,
    const F_base features[block_size][num_features])
{
	const F_base *features_flat = &features[0][0];
	const int total = num_compounds * num_features;
	for (int i = 0; i < total; i ++)
	{
#pragma HLS loop_tripcount min = num_features*block_size \
			   max = num_features*block_size
#pragma HLS PIPELINE
			features_stream << features_flat[i];
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
    hls::stream<P_base> &predictions,
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
			S_type sum(.0F);

			for (int s = 0; s < num_samples; s++)
				for (int k = 0; k < num_latent; k++)
				{
					if (d==0) latents_cache[s][k] = latents[s][k].read();
					S_type prod = L_type(latents_cache[s][k]) * U_type(U_local[s][d][k]);
					sum = sum + prod;
				}

			P_type aggr(
#if defined(DT_FLOAT) || defined(DT_HALF)
			    sum / num_samples
#else
			    sum >> log_num_samples
#endif
			);
			predictions << P_base(aggr);
		} // end proteins
	}
}

void output_loop(
    int num_compounds,
    P_base predictions[block_size][num_proteins],
    hls::stream<P_base> &predictions_stream)
{
	for (int i = 0; i < num_compounds; ++i)
	{
#pragma HLS loop_tripcount min = block_size max = block_size
		for (int d = 0; d < num_proteins; d++)
		{
#pragma HLS LOOP_FLATTEN
#pragma HLS PIPELINE II = 1
			predictions[i][d] = predictions_stream.read();
		}
	}
}

void predict_one_block(
		int num_compounds, // <= block_size
		const F_base features[block_size][num_features],
		      P_base predictions[block_size][num_proteins]
)
{
	hls::stream<L_type> latents[num_samples][num_latent];
	hls::stream<F_base> features_stream;
	hls::stream<P_base> predictions_stream;
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
}

void predict_or_update_model(
		bool update_model,
		int num_compounds,
		const F_arr features,    //[block_size*num_features]
		      P_arr predictions, //[block_size*num_proteins]
		const U_flat U_in,        //[num_samples][num_proteins][num_latent]
		const M_flat M_in,        //[num_samples][num_latent]
		const B_flat B_in)        //[num_samples][num_features][num_latent]
{
#ifndef OMPSS_FPGA
#pragma HLS INTERFACE m_axi port=features offset=slave bundle=features_in max_widen_bitwidth=512
#pragma HLS INTERFACE m_axi port=predictions offset=slave bundle=predictions_out max_widen_bitwidth=512
#endif

	if (update_model)
	{
		load_model(U_in, M_in, B_in);
#ifdef CHECKSUM_MODEL
		checksum_model(features, predictions, U_local, M_local, B_local);
#endif
	} 
        else
	{
		predict_one_block(num_compounds, features, predictions);
	}

} // end function


