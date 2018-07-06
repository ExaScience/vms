
// size of latent protein representation
const int num_latent = 4;
const int num_proteins = 3;

const double U1[num_latent][num_proteins] = {
    { 0.40, 0.36, 0.34 },
    { 1.33, 0.90, 0.94 },
    { 0.14, 0.06, -0.3 },
    { 0.96, 1.29, 1.64 },
};

const int num_feat = 2;

const double F0[num_latent][num_feat] = {
    { 0.40, 0.36 },
    { 1.33, 0.905 },
    { 0.14, 0.061 },
    { 0.96, 1.29 },
};

const int num_comp = 8;

void predict_compound_block_hls(
   const double in[num_comp][num_feat],
   double out[num_comp][num_proteins]
);


void predict_compound_block_c(
   const double in[num_comp][num_feat],
   double out[num_comp][num_proteins]
);
