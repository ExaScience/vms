#include "vms_const.h"

#define epsilon 0.5

typedef float F_blk[block_size][num_features];
typedef float P_blk[block_size][num_proteins];

typedef float U_arr[num_proteins][num_samples][num_latent];
typedef float M_arr              [num_samples][num_latent];
typedef float B_arr[num_features][num_samples][num_latent];

#define MAX_NUM_DEVICES 10

struct predict_data
{
	F_blk *features;
	P_blk *predictions;
	U_arr U;
	M_arr M;
	B_arr B;
};


void predict_blocks(
		int num_blocks,
		int num_devices,
		struct predict_data *data[]
);

#define UNUSED(x) (void)(x)
