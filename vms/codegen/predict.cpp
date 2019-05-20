#include "smurff_const.h"
#include "smurff_types.h"

#include "predict.h"
#include "smurff_model.h"

void predict_compound_block_c(
    const F_type in[num_compounds][num_features],
    P_type out[num_compounds][num_proteins])
{
#pragma HLS ARRAY_RESHAPE variable = out complete dim = 2
#pragma HLS ARRAY_RESHAPE variable = in complete dim = 2
//#pragma HLS ARRAY_PARTITION variable=out complete dim=2
//#pragma HLS ARRAY_PARTITION variable=in complete dim=2
#pragma HLS INTERFACE ap_fifo port = out
#pragma HLS INTERFACE ap_fifo port = in

    int c, d, k;
    U_type tmp[num_latent];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

    F_type in_buf[num_features];
#pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1
    S_type out_buf[num_proteins];
#pragma HLS ARRAY_PARTITION variable=out_buf complete dim=1

//       ((nc x nf) * (nf * nl)) * (nl * np)
//       (nc        x        nl) * (nl * np)
//       (nc             x               np)
// out = ((in * f0.transpose()) + mu) * u1;

predict_compound_block_c_loop11:
    for (c = 0; c < num_compounds; c++)
    {
        for (k = 0; k < num_features; k++)
        {
#pragma HLS UNROLL
            in_buf[k] = in[c][k];
        }

        for (d = 0; d < num_proteins; d++)
        {
#pragma HLS UNROLL
            out_buf[d] = .0;
        }

        for (int s = 0; s < num_samples; s++)
        {
            for (d = 0; d < num_latent; d++)
            {
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=B complete dim=3

                U_type sum = .0;
                for (k = 0; k < num_features; k++)
                {
                    sum = sum + in_buf[k] * B[s][d][k];
                }

                tmp[d] = sum + mu[s][d];
            }

            for (d = 0; d < num_proteins; d++)
            {
#pragma HLS PIPELINE II=1
                S_type sum = .0;
                for (k = 0; k < num_latent; k++)
                {
                    sum = sum + tmp[k] * U[s][k][d];
                }

                out_buf[d] += sum;
            } // end proteins
        } // end samples

        for (d = 0; d < num_proteins; d++)
        {
#pragma HLS UNROLL
            out[c][d] = out_buf[d] / (S_type)num_samples;
        }
    } // end compounds
}
