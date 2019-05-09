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
    //#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

    F_type in_buf[num_features];
    S_type out_buf[num_proteins];
    //#pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1

//       ((nc x nf) * (nf * nl)) * (nl * np)
//       (nc        x        nl) * (nl * np)
//       (nc             x               np)
// out = ((in * f0.transpose()) + mu) * u1;

// tmp = in * F0
predict_compound_block_c_loop11:
    for (c = 0; c < num_compounds; c++)
    {
#pragma HLS PIPELINE II = 1

        for (k = 0; k < num_features; k++)
        {
            in_buf[k] = in[c][k];
        }

        for (d = 0; d < num_proteins; d++)
        {
            out_buf[d] = .0;
        }

        for (int s = 0; s < num_samples; s++)
        {
        predict_compound_block_c_loop12:
            for (d = 0; d < num_latent; d++)
            {
                U_type sum = .0;
            predict_compound_block_c_loop13:
                for (k = 0; k < num_features; k++)
                {
                    sum = sum + in_buf[k] * samples[s].B[d][k];
                }

                tmp[d] = sum + samples[s].mu[d];
            }

        predict_compound_block_c_loop22:
            for (d = 0; d < num_proteins; d++)
            {
                P_type sum = .0;
            predict_compound_block_c_loop23:
                for (k = 0; k < num_latent; k++)
                {
                    sum = sum + tmp[k] * samples[s].U[k][d];
                }

                out_buf[d] += sum;

            } // end proteins
        } // end samples

        for (d = 0; d < num_proteins; d++)
        {
            out[c][d] = out_buf[d] / (S_type)num_samples;
        }
    } // end compounds
}
