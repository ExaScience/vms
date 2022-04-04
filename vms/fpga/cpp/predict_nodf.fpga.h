#pragma once

int features_nodf_loop(
    const F_base *features,
    L_type latents[num_samples][num_latent],
    const Model &m)
{
	L_type latents_acc[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_acc complete dim = 2

	int input_count = 0;
	for (int d = 0; d < num_features; d++)
	{
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = m.B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = m.B complete dim = 1

#pragma HLS ARRAY_PARTITION variable = m.M complete dim = 2
#pragma HLS ARRAY_PARTITION variable = m.M complete dim = 1

		const F_type feature = features[input_count++];
		for (int s = 0; s < num_samples; s++)
			for (int k = 0; k < num_latent; k++)
			{
				L_type v;
				// first iteration of d-loop
				if (d == 0) v = M_type(m.M[s][k]);
				else        v = L_type(latents_acc[s][k]);

				L_type prod = feature * B_type(m.B[s][d][k]);
				latents_acc[s][k] = L_type(v + prod);

				if (d == num_features-1) latents[s][k] = latents_acc[s][k];
			}
	}

	return input_count;
}


int proteins_nodf_loop(
    P_base *predictions,
    L_type latents[num_samples][num_latent],
    const Model &m)
{
	L_type latents_cache[num_samples][num_latent];
#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents_cache complete dim = 2

	int output_count = 0;
	for (int d = 0; d < num_proteins; d++)
	{
#pragma HLS PIPELINE II=4
#pragma HLS ARRAY_PARTITION variable = m.U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = m.U complete dim = 3

		for (int s = 0; s < num_samples; s++)
		{
			S_type sum = S_type();

			for (int k = 0; k < num_latent; k++)
			{
				if (d==0) latents_cache[s][k] = latents[s][k];
				S_type prod = L_type(latents_cache[s][k]) * U_type(m.U[s][d][k]);
				sum = sum + prod;
			}

			predictions[output_count++] = P_type(sum);
		}
	} // end proteins

	return output_count;
}

void predict_dataflow(
		const F_base *features,    //[block_size*num_features]
		      P_base *predictions, //[block_size*num_proteins*num_samples]
		const Model &m)
{
	L_type latents[num_samples][num_latent];
	int num_in = 0;
	int num_out = 0;

	for (int i = 0; i < block_size; ++i)
	{
		num_in += features_nodf_loop(features+num_in, latents, m);
		num_out += proteins_nodf_loop(predictions+num_out, latents, m);
	}

} // end function



