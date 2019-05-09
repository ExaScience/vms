#include "smurff_const.h"

struct sample {
  const U_type  (&U)[num_latent][num_proteins];
  const mu_type (&mu)[num_latent];
  const B_type  (&B)[num_latent][num_features];
};

void predict_compound_block_c(
   const F_type in[num_compounds][num_features],
   P_type out[num_compounds][num_proteins]
);
