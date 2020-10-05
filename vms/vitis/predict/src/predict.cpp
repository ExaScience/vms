
#define SHOWFLOAT(F) printf("%s = %.4f\n", #F, (float)(F))

#include "predict.h"
#include <hls_stream.h>


static U_base U_local[num_samples][num_proteins][num_latent];
static M_base M_local[num_samples][num_latent];
static B_base B_local[num_samples][num_features][num_latent];

void checksum_model(const F_flat features,
					      P_flat out,
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
                    CRC_ADD(F_check, features[c++]);
        }

        {
            for (int c = 0; c < block_size * num_proteins - 3;)
            {
                out[c++] = U_check;
                out[c++] = M_check;
                out[c++] = B_check;
                out[c++] = F_check;
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

void features_loop(
	    const F_base features[num_features],
		hls::stream<L_type> latents[num_samples][num_latent])
{
	L_type latents_acc[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 2

	for (int d = 0; d < num_features; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 1

#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 2
#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 1

		const F_type feature(features[d]);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type  v;
				if (d==0) v = M_type(M_local[s][k]);
				else      v = L_type(latents_acc[s][k]);
				L_type prod = feature * B_type(B_local[s][d][k]);
				latents_acc[s][k] = L_type(v + prod);
			}
	}

#pragma HLS UNROLL
	for (int s = 0; s < num_samples; s++)
		for (int k = 0; k < num_latent; k++)
				latents[s][k] << latents_acc[s][k];
}

void proteins_loop(
	P_base predictions[num_proteins],
	hls::stream<L_type> latents[num_samples][num_latent])
{
	L_type latents_cache[num_samples][num_latent];
	#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 1
	#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 2

	#pragma HLS UNROLL
	for (int s = 0; s < num_samples; s++)
		for (int k = 0; k < num_latent; k++)
				latents_cache[s][k] = latents[s][k].read();


	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 3
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 3
		S_type sum(.0F);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
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
		predictions[d] = P_base(aggr);
	} // end proteins
}


void predict_one_block(
		int num_compounds, // <= block_size
		const F_flat features,   //[block_size*num_features],
		      P_flat predictions //[block_size*num_proteins]
)
{
    for (int i=0; i<num_compounds; ++i)
    {
		hls::stream<L_type> latents[num_samples][num_latent];

#pragma HLS DATAFLOW
        features_loop(&features[i*num_features], latents);
        proteins_loop(&predictions[i*num_proteins], latents);
    }
}

void predict_or_update_model(
		bool update_model,
		int num_compounds,
		const F_flat features,    //[block_size*num_features]
		      P_flat predictions, //[block_size*num_proteins]
		const U_flat U_in,        //[num_samples][num_proteins][num_latent]
		const M_flat M_in,        //[num_samples][num_latent]
		const B_flat B_in)        //[num_samples][num_features][num_latent]
{
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


