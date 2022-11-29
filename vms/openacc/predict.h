#include "vms_const.h"

#define epsilon 0.5

typedef float F_blk[block_size][num_features];
typedef float P_blk[block_size][num_proteins];

typedef float U_arr[num_proteins][num_samples][num_latent];
typedef float M_arr              [num_samples][num_latent];
typedef float B_arr[num_features][num_samples][num_latent];

struct predict_data
{
    float (*output)[block_size][num_proteins];
    float (*input)[block_size][num_features];
};


void predict_blocks(
		int num_blocks,
		int num_devices,
		struct predict_data *data,
		const U_arr U,
		const M_arr M,
		const B_arr B
);
