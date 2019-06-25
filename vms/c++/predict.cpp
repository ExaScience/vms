#include <cstdio>
#include <cstring>

#define SHOWFLOAT(F) printf("%s = %.4f\n", #F, (float)(F))

#include "predict.fpga.h"

void load_model(
        const U_base  *U_in,  //[num_samples][num_proteins][num_latent],
        const mu_base *mu_in, //[num_samples][num_latent],
        const B_base  *B_in)  //[num_samples][num_features][num_latent])
{
#ifdef USE_MEMCPY
	std::memcpy(mu, mu_in, sizeof(mu));
	std::memcpy(U,  U_in, sizeof(U));
	std::memcpy(B,  B_in, sizeof(B));
#else
    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
                U[i][j][k] = *(U_in++);

        for (int j = 0; j < num_latent; j++)
           mu[i][j] = *(mu_in++);

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
                B[i][j][k] = *(B_in++);
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
				L_type  v;
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
		S_type sum(.0F);
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
		const F_base  *features,   //[num_compounds][num_features],
		      P_base  *predictions //[num_compounds][num_proteins]
)
{
    L_type latents[num_compounds][num_samples][num_latent];
    for (int i=0; i<num_compounds; ++i)
    {
#pragma HLS DATAFLOW
        features_loop(features + (i*num_features), latents[i]);
        proteins_loop(predictions + (i*num_proteins), latents[i]);
    }
}


#ifdef OMPSS_FPGA
#pragma omp target device(fpga) copy_deps localmem()
#elif defined(OMPSS_SMP)
#pragma omp target device(smp) copy_deps
#else
#warning Neither OMPSS_FPGA nor OMPSS_SMP defined
#endif
#pragma omp task \
    in([num_compounds*num_features]features, \
       [num_samples*num_proteins*num_latent]U_in,\
       [num_samples*num_latent             ]mu_in,\
       [num_samples*num_features*num_latent]B_in) \
    out([num_compounds*num_proteins]predictions)
void predict_or_update_model(
                bool update_model,
		const F_base  *features,    //[num_compounds*num_features]
		      P_base  *predictions, //[num_compounds*num_proteins]
		const U_base  *U_in,        //[num_samples][num_proteins][num_latent]
		const mu_base *mu_in,       //[num_samples][num_latent]
		const B_base  *B_in)        //[num_samples][num_features][num_latent]
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



void update_model(
    const U_base  U  [num_samples][num_proteins][num_latent],
    const mu_base mu [num_samples][num_latent],
    const B_base  B  [num_samples][num_features][num_latent]
)
{
    static const F_base  empty_in [num_compounds][num_features] = {{0}};
    static       P_base  empty_out[num_compounds][num_proteins];

    predict_or_update_model(true, &empty_in[0][0], &empty_out[0][0], &U[0][0][0], &mu[0][0], &B[0][0][0]);
#pragma omp taskwait
}

void predict_compound(
    const F_base  in [num_compounds][num_features],
          P_base  out[num_compounds][num_proteins]
)
{
    static const U_base  empty_U  [num_samples][num_proteins][num_latent] = {{{0}}};
    static const mu_base empty_mu [num_samples][num_latent] = {{0}};
    static const B_base  empty_B  [num_samples][num_features][num_latent] = {{{0}}};

    predict_or_update_model(false, &in[0][0], &out[0][0], &empty_U[0][0][0], &empty_mu[0][0], &empty_B[0][0][0]);
}

