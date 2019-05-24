#include <cstdio>

#include "smurff_const.h"
#include "predict.h"
#include "types.h"

#define SC_INCLUDE_FX
#include <systemc.h>


template<int wl, int iwl, typename T>
sc_fixed<wl, iwl> to_fx(const T v) 
{
    sc_fixed<wl, wl> f_sc_int = v;
    sc_fixed<wl, iwl> f_sc = f_sc_int >> (wl - iwl);
	return f_sc;
}

#define TO_FX8(V, IWL) to_fx<8, IWL, std::int8_t>(V)
#define TO_FX16(V, IWL) to_fx<16, IWL, std::int16_t>(V)
#define TO_FX32(V, IWl) to_fx<32, IWL, std::int32_t>(V)

static U_type U[num_samples][num_proteins][num_latent];
static mu_type mu[num_samples][num_latent];
static B_type B[num_samples][num_features][num_latent];

void load_model(
		const U_type U_in[num_samples][num_proteins][num_latent],
	    const mu_type mu_in[num_samples][num_latent],
	    const B_type B_in[num_samples][num_features][num_latent])
{
#define MEMCPY2(N) memcpy(&(N[0][0]), &(N##_in[0][0]), sizeof(N)/sizeof(N[0][0]))
#define MEMCPY3(N) memcpy(&(N[0][0][0]), &(N##_in[0][0][0]), sizeof(N)/sizeof(N[0][0][0]))

	//MEMCPY2(mu);

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
	    const F_type features[num_features],
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

		const sc_fixed<F_wl, F_iwl> feature = TO_FX16(features[d], F_iwl);
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type v;
				if (d==0)
					v = TO_FX8(mu[s][k], mu_iwl);
				else
					v = latents[s][k];

				latents[s][k] = v + feature * TO_FX16(B[s][d][k], B_iwl);
			}
	}
}

void proteins_loop(
	    P_type predictions[num_proteins],
		const L_type latents[num_samples][num_latent])
{

	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
		S_type sum = .0;
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
				sum = sum + latents[s][k] * TO_FX16(U[s][d][k], U_iwl);

		predictions[d] = sum / num_samples * (1 << P_shift);
	} // end proteins
}


void predict(
		const F_type  features[num_features],
		      P_type  predictions[num_proteins]
)
{
#pragma HLS DATAFLOW
		L_type latents[num_samples][num_latent];
		features_loop(features, latents);
		proteins_loop(predictions, latents);
}


void predict_or_update_model(
		const F_type  features[num_features],
		      P_type  predictions[num_proteins],
		bool  update_model,
		const U_type U_in[num_samples][num_proteins][num_latent],
		const mu_type mu_in[num_samples][num_latent],
		const B_type B_in[num_samples][num_features][num_latent])
{
	if (update_model)
	{
		load_model(U_in, mu_in, B_in);
	} 
	else 
	{
		predict(features, predictions);
	}

} // end function
