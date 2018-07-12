#include "predict.h"

#include "smurff_const.h"
#include "smurff_model.h"

using namespace sample_0;

void predict_compound_block_c(
        const F_type in[num_compounds][num_features],
        P_type out[num_compounds][num_proteins]
        )
{
#pragma HLS ARRAY_RESHAPE variable=out complete dim=2
#pragma HLS ARRAY_RESHAPE variable=in complete dim=2
    //#pragma HLS ARRAY_PARTITION variable=out complete dim=2
    //#pragma HLS ARRAY_PARTITION variable=in complete dim=2
#pragma HLS INTERFACE ap_fifo port=out
#pragma HLS INTERFACE ap_fifo port=in

    int c, d, k;
    U_type tmp[num_latent];
    //#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

    F_type in_buf[num_features];
    //#pragma HLS ARRAY_PARTITION variable=in_buf complete dim=1

    //       ((nc x nf) * (nf * nl)) * (nl * np)
    //       (nc        x        nl) * (nl * np)
    //       (nc             x               np)
    // out = (in * f0.transpose()) * u1;


    // tmp = in * F0
    for (c = 0; c < num_compounds; c++)
    {
#pragma HLS PIPELINE II=1

        for (k = 0; k < num_features; k++) {
            in_buf[k] = in[c][k];
        }

        for (d = 0; d < num_latent; d++)
        {
            S_type sum = .0;
            for (k = 0; k < num_features; k++)
            {
                sum = sum + in_buf[k] * B[d][k];
            }

            tmp[d] = sum;
        }

        for (d = 0; d < num_proteins; d++)
        {
            S_type sum = .0;
            for (k = 0; k < num_latent; k++)
            {
                sum = sum + tmp[k] * U[k][d];
            }

            out[c][d] = sum;
        }
    }
}
