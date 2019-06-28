#include <cstring>

#include "predict.h"


static U_base U[num_samples][num_proteins][num_latent];
static mu_base mu[num_samples][num_latent];
static B_base B[num_samples][num_features][num_latent];

