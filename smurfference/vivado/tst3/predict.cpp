#include "smurff_const.h"
#include "smurff_model.h"

#define F0 s1_F0
#define U1 s1_U1

void predict_compound_block_c(
   const double in[tb_num_compounds][num_features],
   double out[tb_num_compounds][num_proteins]
)
{
	int c, d, k;
    double tmp[tb_num_compounds][num_latent];

    //       ((nc x nf) * (nf * nl)) * (nl * np)
    //       (nc        x        nl) * (nl * np)
    //       (nc             x               np)
    // out = (in * f0.transpose()) * u1;


    // tmp = in * F0
    predict_compound_block_c_loop11:
	for (c = 0; c < tb_num_compounds; c++)
    {
#pragma HLS PIPELINE II=1
        predict_compound_block_c_loop12:
		for (d = 0; d < num_latent; d++)
        {
#pragma HLS UNROLL
            double sum = .0;
            predict_compound_block_c_loop13:
			for (k = 0; k < num_features; k++)
            {
#pragma HLS UNROLL
                sum = sum + in[c][k] * F0[d][k];
            }

            tmp[c][d] = sum;
        }

    	predict_compound_block_c_loop22:
        for (d = 0; d < num_proteins; d++)
        {
#pragma HLS UNROLL
            double sum = .0;
            predict_compound_block_c_loop23:
            for (k = 0; k < num_latent; k++)
            {
#pragma HLS UNROLL
                sum = sum + tmp[c][k] * U1[k][d];
            }

            out[c][d] = sum;
        }
    }
}
