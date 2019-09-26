#if defined(DT_FIXED) || defined(DT_FLOAT)
#else
#define DT_FIXED
#endif

#include "vms_const.h"

static const int block_size = 1000;

#ifdef DT_FIXED

#define DT_NAME "fxp<T,I>"

#include "fxp.h"

typedef fxp<U_base, U_iwl> U_type;
typedef fxp<M_base, M_iwl> M_type;
typedef fxp<B_base, B_iwl> B_type;
typedef fxp<F_base, F_iwl> F_type;
typedef fxp<P_base, P_iwl> P_type;

// -- fixed fixed

typedef signed int L_base;
const int L_iwl = 10;
typedef fxp<L_base, L_iwl> L_type;

typedef signed int S_base;
const int S_iwl = 10;
typedef fxp<S_base, S_iwl> S_type;

const float epsilon = 0.5;

#define CRC_INIT(crc) (crc) = 0xd1
#define CRC_ADD(crc, value) (crc) ^= (value)
#define CRC_FMT "0x%04x"

#elif defined(DT_FLOAT)

#define DT_NAME "float"

typedef float U_base;
typedef float M_base;
typedef float B_base;
typedef float F_base;
typedef float P_base;
typedef float L_base;
typedef float S_base;

#if 0
typedef observable<float, U_id> U_type;
typedef observable<float, M_id> M_type;
typedef observable<float, B_id> B_type;
typedef observable<float, F_id> F_type;
typedef observable<float, P_id> P_type;
typedef observable<float, L_id> L_type;
typedef observable<float, S_id> S_type;
#else
typedef float U_type;
typedef float M_type;
typedef float B_type;
typedef float F_type;
typedef float P_type;
typedef float L_type;
typedef float S_type;
#endif

const float epsilon = 0.5;

#define CRC_INIT(crc) (crc) = .5
#define CRC_ADD(crc, value) (crc) += (value)
#define CRC_FMT "%.2f"

#else
#error Need to defined DT_FIXED or DT_FLOAT
#endif

typedef F_base F_flx[][num_features];
typedef P_base P_flx[][num_proteins];

typedef F_base F_blk[block_size][num_features];
typedef P_base P_blk[block_size][num_proteins];

typedef U_base U_arr[num_samples][num_proteins][num_latent];
typedef M_base M_arr[num_samples][num_latent];
typedef B_base B_arr[num_samples][num_features][num_latent];

typedef U_base U_flat[num_samples][num_proteins][num_latent];
typedef M_base M_flat[num_samples][num_latent];
typedef B_base B_flat[num_samples][num_features][num_latent];

void update_model(const U_arr U, const M_arr mu, const B_arr B);

void predict_compounds(int num_compounds, const F_flx in, P_flx out);
