#include "vms_const.h"

static const int block_size = 10000;

const float epsilon = 0.5;

typedef float F_blk[block_size][num_features];
typedef float P_blk[block_size][num_proteins];

typedef float U_arr[num_proteins][num_samples][num_latent];
typedef float M_arr              [num_samples][num_latent];
typedef float B_arr[num_features][num_samples][num_latent];

void predict_block(
		int num_repeat,
		const F_blk in,
		      P_blk out,
		const U_arr U,
		const M_arr M,
		const B_arr B 
);
