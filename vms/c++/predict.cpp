
#define SHOWFLOAT(F) printf("%s = %.4f\n", #F, (float)(F))

#include "predict.fpga.h"

void checksum_model(const F_base *features,
					P_base *out,
					const U_base  *U,  //[num_samples][num_proteins][num_latent]
					const mu_base *mu, //[num_samples][num_latent]
					const B_base  *B)  //[num_samples][num_features][num_latent])
{
	P_base U_check;
	P_base mu_check;
	P_base B_check;
	P_base F_check;

	CRC_INIT(U_check);
	CRC_INIT(mu_check);
	CRC_INIT(B_check);
	CRC_INIT(F_check);

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
                CRC_ADD(U_check, (*U++));

        for (int j = 0; j < num_latent; j++)
           CRC_ADD(mu_check, (*mu++));

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
                CRC_ADD(B_check, (*B++));
	}

    for (int i = 0; i < block_size * num_features; i++)
    {
		CRC_ADD(F_check, features[i]);
	}

	out[0] = U_check;
	out[1] = mu_check;
	out[2] = B_check;
	out[3] = F_check;
}

void load_model(
		const U_base *U_in,   //[num_samples][num_proteins][num_latent],
		const mu_base *mu_in, //[num_samples][num_latent],
		const B_base *B_in)   //[num_samples][num_features][num_latent])
	{
#define USE_MEMCPY
#ifdef USE_MEMCPY
	std::memcpy(mu_local, mu_in, sizeof(mu_local));
	std::memcpy(U_local,  U_in, sizeof(U_local));
	std::memcpy(B_local,  B_in, sizeof(B_local));
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
		L_base latents[num_samples][num_latent])
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
				if (d==0) v = mu_type(mu_local[s][k]);
				else      v = L_type(latents[s][k]);
				L_type prod = feature * B_type(B_local[s][d][k]);
				latents[s][k] = L_base(L_type(v + prod));
			}
	}
}

void proteins_loop(
	P_base predictions[num_proteins],
	const L_base latents[num_samples][num_latent])
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
				S_type prod = L_type(latents[s][k]) * U_type(U_local[s][d][k]);
				sum = sum + prod;
			}

		P_type aggr(
#ifdef DT_FLOAT
			sum / num_samples
#else
			sum >> log_num_samples
#endif
			);
		predictions[d] = P_base(aggr);
	} // end proteins
}


void predict(
		int num_compounds, // <= block_size
		const F_base  *features,   //[block_size][num_features],
		      P_base  *predictions //[block_size][num_proteins]
)
{
    for (int i=0; i<num_compounds; ++i)
    {
#pragma HLS DATAFLOW
		L_base latents[num_samples][num_latent];
        features_loop(features + (i*num_features), latents);
        proteins_loop(predictions + (i*num_proteins), latents);
    }
}


#ifdef OMPSS_FPGA
#pragma omp target device(fpga) copy_deps localmem()
#elif defined(OMPSS_SMP)
#pragma omp target device(smp) copy_deps
#else
#endif
#pragma omp task \
    in([block_size*num_features]features, \
       [num_samples*num_proteins*num_latent]U_in,\
       [num_samples*num_latent             ]mu_in,\
       [num_samples*num_features*num_latent]B_in) \
    out([block_size*num_proteins]predictions)
void predict_or_update_model(
		bool update_model,
		int num_compounds,
		const F_base  *features,    //[block_size*num_features]
		      P_base  *predictions, //[block_size*num_proteins]
		const U_base  *U_in,        //[num_samples][num_proteins][num_latent]
		const mu_base *mu_in,       //[num_samples][num_latent]
		const B_base  *B_in)        //[num_samples][num_features][num_latent]
{
//#pragma HLS INTERFACE m_axi port=features depth=block_size*num_features
//#pragma HLS INTERFACE m_axi port=predictions depth=block_size*num_proteins
//#pragma HLS INTERFACE m_axi port=U_in depth=num_samples*num_proteins*num_latent
//#pragma HLS INTERFACE m_axi port=mu_in depth=num_samples*num_latent
//#pragma HLS INTERFACE m_axi port=B_in depth=num_samples*num_features*num_latent
	if (update_model)
	{
		load_model(U_in, mu_in, B_in);
		checksum_model(features, predictions, &U_local[0][0][0], &mu_local[0][0], &B_local[0][0][0]);
	} 
        else
	{
		predict(num_compounds, features, predictions);
	}

} // end function



void update_model(
    const U_base  U_in  [num_samples][num_proteins][num_latent],
    const mu_base mu_in [num_samples][num_latent],
    const B_base  B_in  [num_samples][num_features][num_latent])
{
    static F_base  in_block [block_size*num_features];
    in_block [0] = 1;
    in_block [block_size*num_features - 1] = 2;

    static P_base  out_block_1[block_size*num_proteins];
    static P_base  out_block_2[block_size*num_proteins];

    predict_or_update_model(true, 0, in_block, out_block_1, &U_in[0][0][0], &mu_in[0][0], &B_in[0][0][0]);
#pragma omp taskwait

	checksum_model(in_block, out_block_2, &U_in[0][0][0], &mu_in[0][0], &B_in[0][0][0]);

    printf("Checksums U, mu, B, F\n");
    printf("  Computed: " CRC_FMT ", " CRC_FMT ", " CRC_FMT ", " CRC_FMT  "\n", out_block_1[0], out_block_1[1], out_block_1[2], out_block_1[3]);
    printf("  Expected: " CRC_FMT ", " CRC_FMT ", " CRC_FMT ", " CRC_FMT  "\n", out_block_2[0], out_block_2[1], out_block_2[2], out_block_2[3]);
}


void predict_compound(
	int num_compounds,
    const F_base  in [][num_features],
          P_base  out[][num_proteins]
)
{
    static const U_base  empty_U  [num_samples*num_proteins*num_latent] = {0};
    static const mu_base empty_mu [num_samples*num_latent] = {0};
    static const B_base  empty_B  [num_samples*num_features*num_latent] = {0};

    int i;
    for(i=0; i<=num_compounds - block_size; i+=block_size)
    {
        printf("Full task\n");
        predict_or_update_model(false, block_size, &in[i][0], &out[i][0], empty_U, empty_mu, empty_B);
    }

    // last block left-overs
    int nc = num_compounds - i;
    if (nc == 0) {
#pragma omp taskwait
    } else {
        F_base *in_block = new F_base[block_size*num_features];
        P_base *out_block= new P_base[block_size*num_proteins];

        memcpy(in_block, &in[i][0], nc*num_features*sizeof(F_base));
        printf("Last part task\n");
        predict_or_update_model(false, nc, in_block, out_block, empty_U, empty_mu, empty_B);
#pragma omp taskwait
        memcpy(&out[i][0], out_block, nc*num_proteins*sizeof(P_base));

        delete[] in_block;
        delete[] out_block;
    }
}

