#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstring>

#include "predict.h"

static U_base U_local[num_samples][num_proteins][num_latent];
static M_base M_local[num_samples][num_latent];
static B_base B_local[num_samples][num_features][num_latent];

