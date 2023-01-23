
#define SHOWFLOAT(F) printf("%s = %.4f\n", #F, (float)(F))

#include "predict.h"

#ifdef OMPSS_SMP
#pragma oss task \
	in(U[:num_proteins]) \
	in(M[:num_samples]) \
	in(B[:num_features]) \
	in(features[:block_size]) \
	out(predictions[:block_size])
#elif defined(OMPSS_OPENACC)
#pragma oss task device(openacc) \
	in(U[:num_proteins]) \
	in(M[:num_samples]) \
	in(B[:num_features]) \
	in(features[:block_size]) \
	out(predictions[:block_size])
#else
#warning Not using OMPSS (define either OMPSS_SMP or OMPSS_OPENACC)
#endif
void predict_block(
		const F_blk features,
		      P_blk predictions,
		const U_arr U,
		const M_arr M,
		const B_arr B
)
{
#ifndef OMPSS_OPENACC
#pragma acc data \
	copyin( \
		U[:num_proteins][:num_samples][:num_latent], \
		M[:num_samples][:num_latent], \
		B[:num_features][:num_samples][:num_latent], \
		features[:block_size][:num_features]) \
	copyout(predictions[:block_size][:num_proteins])
#endif /* OMPSS_OPENACC */
#pragma acc parallel loop
		for (int i = 0; i < block_size; ++i)
		{
			float latents[num_samples][num_latent];
#pragma acc cache(latents)
			{
#pragma acc loop collapse(2) independent vector
				for (int s = 0; s < num_samples; s++)
					for (int k = 0; k < num_latent; k++)
					{
						float sum = M[s][k];
#pragma acc loop seq
						for (int d = 0; d < num_features; d++)
							sum += features[i][d] * B[d][s][k];

						latents[s][k] = sum;
					}
			}
			{
#pragma acc loop independent worker
				for (int d = 0; d < num_proteins; d++)
				{
					float sum = .0F;
#pragma acc loop collapse(2) vector
					for (int s = 0; s < num_samples; s++)
						for (int k = 0; k < num_latent; k++)
							sum += latents[s][k] * U[d][s][k];

					predictions[i][d] = sum / num_samples;
				} // end proteins
			}
		}
}

void predict_blocks(
		int num_blocks,
		int num_devices,
		struct predict_data *data[]
)
{
	for(int i=0; i<num_blocks; ++i)
		for(int d=0; d<num_devices; ++d)
		{
#ifdef OPENACC_ONLY
#pragma acc set device_num(d)
#endif
			predict_block(data[d]->features[i], data[d]->predictions[i], data[d]->U, data[d]->M, data[d]->B);
		}

#pragma oss taskwait
}
