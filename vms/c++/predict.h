#include "smurff_const.h"

#include "smurff_types.h"

void predict_or_update_model(
    const F_base  in[num_features],
          P_base  out[num_proteins],

    bool update_model,
    const U_base  U  [num_samples][num_proteins][num_latent],
    const mu_base mu [num_samples][num_latent],
    const B_base  B  [num_samples][num_features][num_latent]
);

void update_model(
    const U_base  U  [num_samples][num_proteins][num_latent],
    const mu_base mu [num_samples][num_latent],
    const B_base  B  [num_samples][num_features][num_latent]
);

void predict_compound(
    const F_base  in[num_features],
          P_base  out[num_proteins]
);
