#include "vms_const.h"

static const int block_size = 1000;

#define DT_NAME "float"

typedef float U_base;
typedef float M_base;
typedef float B_base;
typedef float F_base;
typedef float P_base;
typedef float L_base;
typedef float S_base;

typedef float U_type;
typedef float M_type;
typedef float B_type;
typedef float F_type;
typedef float P_type;
typedef float L_type;
typedef float S_type;

const float epsilon = 0.5;

typedef F_base F_flx[][num_features];
typedef P_base P_flx[][num_proteins];

typedef F_base F_flat[block_size*num_features];
typedef P_base P_flat[block_size*num_proteins];

typedef U_base U_arr[num_samples][num_proteins][num_latent];
typedef M_base M_arr[num_samples][num_latent];
typedef B_base B_arr[num_samples][num_features][num_latent];

typedef U_base U_flat[num_samples*num_proteins*num_latent];
typedef M_base M_flat[num_samples*num_latent];
typedef B_base B_flat[num_samples*num_features*num_latent];

void update_model(const U_arr U, const M_arr mu, const B_arr B);

void predict_compounds(int num_compounds, const F_flx in, P_flx out);