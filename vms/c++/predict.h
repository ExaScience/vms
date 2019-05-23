#include "smurff_const.h"
#include "types.h"

void predict_compound_block_c(
    const F_type  in[][num_features],
          P_type  out[][num_proteins],

    int num_compounds,

    const U_type  U  [num_samples][num_proteins][num_latent],
    const mu_type mu [num_samples][num_latent],
    const B_type  B  [num_samples][num_features][num_latent]
);
