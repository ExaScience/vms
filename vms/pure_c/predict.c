#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "predict.h"

static U_base U_local[num_samples][num_proteins][num_latent];
static M_base M_local[num_samples][num_latent];
static B_base B_local[num_samples][num_features][num_latent];

void load_model(
		const U_flat U_in,   //[num_samples][num_proteins][num_latent],
		const M_flat M_in, //[num_samples][num_latent],
		const B_flat B_in)   //[num_samples][num_features][num_latent])
	{
#ifdef USE_MEMCPY
	memcpy(M_local, M_in, sizeof(M_local));
	memcpy(U_local, U_in, sizeof(U_local));
	memcpy(B_local, B_in, sizeof(B_local));
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
		L_base latents[num_samples][num_latent])
{
	for (int d = 0; d < num_features; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 1

#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 2
#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 2

		const F_type feature = features[d];
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type  v;
				if (d==0) v = M_local[s][k];
				else      v = latents[s][k];
				L_type prod = feature * B_local[s][d][k];
				latents[s][k] = v + prod;
			}
	}
}

void proteins_loop(
	P_base predictions[num_proteins],
	const L_base latents[num_samples][num_latent])
{
	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 3
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 3
		S_type sum = .0F;
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				S_type prod = latents[s][k] * U_local[s][d][k];
				sum = sum + prod;
			}

		P_type aggr = sum / num_samples;
		predictions[d] = aggr;
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
#pragma HLS DATAFLOW
		L_base latents[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 2		

        features_loop(&features[i*num_features], latents);
        proteins_loop(&predictions[i*num_proteins], latents);
    }
}

void predict_or_update_model(
		int update_model,
		int num_compounds,
		const F_flat features,    //[block_size*num_features]
		      P_flat predictions, //[block_size*num_proteins]
		const U_flat U_in,        //[num_samples][num_proteins][num_latent]
		const M_flat M_in,        //[num_samples][num_latent]
		const B_flat B_in)        //[num_samples][num_features][num_latent]
{
//#pragma HLS INTERFACE m_axi port=features depth=block_size*num_features
//#pragma HLS INTERFACE m_axi port=predictions depth=block_size*num_proteins
//#pragma HLS INTERFACE m_axi port=U_in depth=num_samples*num_proteins*num_latent
//#pragma HLS INTERFACE m_axi port=M_in depth=num_samples*num_latent
//#pragma HLS INTERFACE m_axi port=B_in depth=num_samples*num_features*num_latent

	if (update_model)
	{
		load_model(U_in, M_in, B_in);
	} 
        else
	{
		predict_one_block(num_compounds, features, predictions);
	}

} // end function



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

    predict_or_update_model(1, 0, in_block, out_block_1, &U_in[0][0][0], &M_in[0][0], &B_in[0][0][0]);
}


void predict_compounds(int num_compounds, const F_flx in, P_flx out)
{
    const U_flat empty_U = {0};
    const M_flat empty_mu = {0};
    const B_flat empty_B = {0};

    int i;
    for(i=0; i<=num_compounds - block_size; i+=block_size)
    {
        predict_or_update_model(0, block_size, &in[i][0], &out[i][0], empty_U, empty_mu, empty_B);
    }

    // last block left-overs
    int nc = num_compounds - i;
    if (nc > 0) {
		F_flat in_block;
		P_flat out_block;

		memcpy(in_block, &in[i][0], nc*num_features*sizeof(F_base));
        predict_or_update_model(0, nc, in_block, out_block, empty_U, empty_mu, empty_B);
        memcpy(&out[i][0], out_block, nc*num_proteins*sizeof(P_base));
    }
}

