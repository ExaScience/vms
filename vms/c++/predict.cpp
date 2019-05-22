#include <cstdio>

#include "smurff_const.h"
#include "predict.h"
#include "types.h"

void predict_compound_block_c(
    const F_type features[num_compounds][num_features],
          P_type predictions[num_compounds][num_proteins],
    const U_type U_in [num_samples][num_proteins][num_latent],
    const mu_type mu_in[num_samples][num_latent],
    const B_type B_in [num_samples][num_latent][num_features]
    )
{
#pragma HLS ARRAY_RESHAPE variable = predictions complete dim = 2
#pragma HLS ARRAY_RESHAPE variable = features complete dim = 2
//#pragma HLS ARRAY_PARTITION variable=predictions complete dim=2
//#pragma HLS ARRAY_PARTITION variable=features complete dim=2
#pragma HLS INTERFACE ap_fifo port = predictions
#pragma HLS INTERFACE ap_fifo port = features

    U_type   U[num_samples][num_proteins][num_latent];
    mu_type mu[num_samples][num_latent];
    B_type   B[num_samples][num_latent][num_features];

    load_model_loop:
    for(int i=0; i<num_samples; i++)
    {
        for(int j=0; j<num_proteins; j++)
            for(int k=0; k<num_latent; k++)
                 U[i][j][k] = U_in[i][j][k];

        for(int j=0; j<num_latent; j++)
            mu[i][j] = mu_in[i][j];

        for(int j=0; j<num_latent; j++)
            for(int k=0; k<num_features; k++)
                B[i][j][k] = B_in[i][j][k];
    }

    int c, d, k;
    S_type tmp[num_latent];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

#pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1
    S_type out_buf[num_proteins];
#pragma HLS ARRAY_PARTITION variable=out_buf complete dim=1

//       ((nc x nf) * (nf * nl)) * (nl * np)
//       (nc        x        nl) * (nl * np)
//       (nc             x               np)
// predictions = ((features * f0.transpose()) + mu) * u1;

predict_loop:
    for (c = 0; c < num_compounds; c++)
    {
        for (d = 0; d < num_proteins; d++)
        {
#pragma HLS UNROLL
            out_buf[d] = .0;
        }

        for (int s = 0; s < num_samples; s++)
        {
            for (d = 0; d < num_features; d++)
            {
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=B complete dim=3

                S_type feature;
                feature = features[c][d];
                for (k = 0; k < num_latent; k++)
                {

                    S_type prod = feature * B[s][k][d];
                    if (d == 0)
                        tmp[k] = mu[s][k] + prod;
                    else
                        tmp[k] = tmp[k] + prod;
                }
            }

            for (d = 0; d < num_proteins; d++)
            {
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=U complete dim=3
                S_type sum = .0;
                for (k = 0; k < num_latent; k++)
                {
                    sum = sum + tmp[k] * U[s][d][k];
                }

                out_buf[d] += sum;
            } // end proteins
        } // end samples

        for (d = 0; d < num_proteins; d++)
        {
#pragma HLS UNROLL
            predictions[c][d] = out_buf[d] / (S_type)num_samples;
        }
    } // end compounds
}
