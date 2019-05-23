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
	    const F_type features[num_compounds][num_features],
		int c,
		S_type tmp[num_samples][num_latent])
{
	for (int d = 0; d < num_features; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B complete dim = 1

#pragma HLS ARRAY_PARTITION variable = mu complete dim = 2
#pragma HLS ARRAY_PARTITION variable = mu complete dim = 1
#pragma HLS ARRAY_PARTITION variable = tmp complete dim = 1
#pragma HLS ARRAY_PARTITION variable = tmp complete dim = 2

		F_type feature;
		feature = features[c][d];
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				S_type v;
				if (d==0) v = mu[s][k]; else v = tmp[s][k];
				tmp[s][k] = v + feature * B[s][d][k];
			}
	}
}

void predict_compound_block_c(
    const F_type features[num_compounds][num_features],
    P_type predictions[num_compounds][num_proteins],
    const U_type U_in[num_samples][num_proteins][num_latent],
    const mu_type mu_in[num_samples][num_latent],
    const B_type B_in[num_samples][num_features][num_latent])
{
    //#pragma HLS ARRAY_RESHAPE variable = predictions complete dim = 2
    //#pragma HLS ARRAY_RESHAPE variable = features complete dim = 2
    //#pragma HLS ARRAY_PARTITION variable=predictions complete dim=2
    //#pragma HLS ARRAY_PARTITION variable=features complete dim=2
    //#pragma HLS INTERFACE ap_fifo port = predictions
    //#pragma HLS INTERFACE ap_fifo port = features


	load_model(U_in, mu_in, B_in);


    //       ((nc x nf) * (nf * nl)) * (nl * np)
    //       (nc        x        nl) * (nl * np)
    //       (nc             x               np)
    // predictions = ((features * f0.transpose()) + mu) * u1;

predict_loop:
    for (int c = 0; c < num_compounds; c++)
    {
//#pragma HLS DATAFLOW


    S_type tmp[num_samples][num_latent];

    features_loop(features, c, tmp);

        for (int d = 0; d < num_proteins; d++)
        {
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
            S_type sum = .0;
            for (int s = 0; s < num_samples; s++)
                for (int k = 0; k < num_latent; k++)
                    sum = sum + tmp[s][k] * U[s][d][k];

            predictions[c][d] = sum;
        } // end proteins
    }     // end compounds
}
