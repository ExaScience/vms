#include "smurff_const.h"
#include "smurff_types.h"

void update_model(
    U_base  U  [num_samples][num_proteins][num_latent],
    mu_base mu [num_samples][num_latent],
    B_base  B  [num_samples][num_features][num_latent]
);

void predict_compound(
    F_base  in[num_features],
    P_base  out[num_proteins]
);
