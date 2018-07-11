#include "smurff_const.h"
#include "smurff_model.h"

#include "hls_linear_algebra.h"

void predict_compound_block_hls(
   const double in[tb_num_comp][num_feat],
   double out[tb_num_comp][num_proteins]
)
{
    int c, d, k;
    double tmp[tb_num_comp][num_latent];

    //       ((nc x nf) * (nf * nl)) * (nl * np)
    //       (nc        x        nl) * (nl * np)
    //       (nc             x               np)
    // out = (in * f0.transpose()) * u1;

    // tmp = in * F0.transpose
    hls::matrix_multiply <hls::NoTranspose, hls::Transpose, tb_num_comp, num_feat, num_latent, num_feat, tb_num_comp, num_latent, double, double> (in, F0, tmp);
    // out = tmp * u1
    hls::matrix_multiply <hls::NoTranspose, hls::NoTranspose, tb_num_comp, num_latent, num_latent, num_proteins, tb_num_comp, num_proteins, double, double> (tmp, U1, out);

}

void predict_compound_block_c(
   const double in[tb_num_comp][num_feat],
   double out[tb_num_comp][num_proteins]
)
{
	int c, d, k;
    double tmp[tb_num_comp][num_latent];

    //       ((nc x nf) * (nf * nl)) * (nl * np)
    //       (nc        x        nl) * (nl * np)
    //       (nc             x               np)
    // out = (in * f0.transpose()) * u1;


    // tmp = in * F0
    predict_compound_block_c_loop11:
	for (c = 0; c < tb_num_comp; c++)
    {
#pragma HLS PIPELINE II=1
        predict_compound_block_c_loop12:
		for (d = 0; d < num_latent; d++)
        {
#pragma HLS UNROLL
            double sum = .0;
            predict_compound_block_c_loop13:
			for (k = 0; k < num_feat; k++)
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
