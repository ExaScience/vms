#include <cstdio>
#include <cstring>

#define SHOWFLOAT(F) printf("%s = %.4f\n", #F, (float)(F))

#include "predict.fpga.h"

void load_model(
	U_base U_in[num_samples][num_proteins][num_latent],
	mu_base mu_in[num_samples][num_latent],
	B_base B_in[num_samples][num_features][num_latent])
{
#ifdef USE_MEMCPY
	std::memcpy(mu, mu_in, sizeof(mu));
	std::memcpy(U, U_in, sizeof(U));
	std::memcpy(B, B_in, sizeof(B));
#else
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
#endif
}

void features_loop(
	    const F_base features[num_features],
		L_type latents[num_samples][num_latent])
{

	for (int d = 0; d < num_features; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B complete dim = 1

#pragma HLS ARRAY_PARTITION variable = mu complete dim = 2
#pragma HLS ARRAY_PARTITION variable = mu complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 2

		const F_type feature(features[d]);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type  v(0xdead);
				if (d==0) v = mu_type(mu[s][k]);
				else      v = latents[s][k];
				L_type prod = feature * B_type(B[s][d][k]);
				latents[s][k] = v + prod;
			}
	}
}

void proteins_loop(
	    P_base predictions[num_proteins],
		const L_type latents[num_samples][num_latent])
{
	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
		S_type sum(0);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				S_type prod = latents[s][k] * U_type(U[s][d][k]);
				sum = sum + prod;
			}

		P_type aggr(
#ifdef DT_FLOAT
			sum / num_samples
#else
			sum >> log_num_samples
#endif
			);
		predictions[d] = aggr;
	} // end proteins
}


void predict(
		const F_base  features[num_features],
		      P_base  predictions[num_proteins]
)
{
#pragma HLS DATAFLOW
		L_type latents[num_samples][num_latent];
		features_loop(features, latents);
		proteins_loop(predictions, latents);
}


#pragma omp target device(fpga) copy_deps localmem()
#pragma omp task \
    in([num_features]features, [num_samples][num_proteins][num_latent]U_in,\
       [num_samples][num_latent]mu_in,\
       [num_samples][num_features][num_latent]B_in) \
    out([num_proteins]predictions)
void predict_with_model_task(
		F_base  features[num_features],
		P_base  predictions[num_proteins],
		U_base U_in[num_samples][num_proteins][num_latent],
		mu_base mu_in[num_samples][num_latent],
		B_base B_in[num_samples][num_features][num_latent])
{
        load_model(U_in, mu_in, B_in);
        predict(features, predictions);
} // end function


void predict_with_model(
        F_base  features[num_features],
        P_base  predictions[num_proteins],
        U_base U_in[num_samples][num_proteins][num_latent],
        mu_base mu_in[num_samples][num_latent],
        B_base B_in[num_samples][num_features][num_latent])
{
    predict_with_model_task(features, predictions, U_in, mu_in, B_in);
}
