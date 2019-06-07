#include "smurff_const.h"
#include "smurff_types.h"

void predict_with_model(
		F_base  features[num_features],
		P_base  predictions[num_proteins],
		U_base U_in[num_samples][num_proteins][num_latent],
		mu_base mu_in[num_samples][num_latent],
		B_base B_in[num_samples][num_features][num_latent]);
