#include "smurff_const.h"

void predict_compound_block_c(
    const float in[num_compounds][num_features],
    float out     [num_compounds][num_proteins],
    const float U [num_samples][num_latent][num_proteins],
    const float mu[num_samples][num_latent],
    const float B [num_samples][num_latent][num_features]
);
