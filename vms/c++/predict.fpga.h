#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstring>

#include "predict.h"

static U_base U_local[num_samples][num_proteins][num_latent];
static M_base M_local[num_samples][num_latent];
static B_base B_local[num_samples][num_features][num_latent];

#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = B_local complete dim = 3

#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = M_local complete dim = 2

#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U_local complete dim = 3

