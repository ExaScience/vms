#include "vms_const.h"

static const int block_size = 4096; // align to page size

const int L_wl = 32;
const int L_iwl = 12;
const int S_wl = 32;
const int S_iwl = 12;

const float epsilon = 0.5;

#if defined(DT_FIXED)
#define DT_NAME "fxp<T,I>"
#define DT_FIXED_INT
#define DT_FIXED_EXT
#elif defined(DT_MIXED)
#define DT_NAME "float + fxp<WL,IWL>"
#define DT_FIXED_INT
#define DT_FLOAT_EXT
#elif defined(DT_FLOAT)
#define DT_NAME "float"
#define DT_FLOAT_INT
#define DT_FLOAT_EXT
#elif defined(DT_HALF)
#define DT_NAME "half"
#define DT_FLOAT_EXT
#define DT_HALF_INT
#else
#error Need to define DT_FIXED, DT_MIXED, DT_FLOAT or DT_HALF
#endif

#ifdef DT_FIXED_EXT

#include "fxp.h"

typedef int_type<U_wl>::T U_base;
typedef int_type<M_wl>::T M_base;
typedef int_type<B_wl>::T B_base;
typedef int_type<F_wl>::T F_base;
typedef int_type<P_wl>::T P_base;
typedef int_type<L_wl>::T L_base;
typedef int_type<S_wl>::T S_base;

#define CRC_INIT(crc) (crc) = 0xd1
#define CRC_ADD(crc, value) (crc) ^= (value)
#define CRC_FMT "0x%04x"

const bool dt_fixed = true;

#else // DT_FLOAT_EXT

typedef float U_base;
typedef float M_base;
typedef float B_base;
typedef float F_base;
typedef float P_base;

#define CRC_INIT(crc) (crc) = .5
#define CRC_ADD(crc, value) (crc) += (value)
#define CRC_FMT "%.2f"

const bool dt_fixed = false;

#endif

#ifdef DT_FIXED_INT

#include "fxp.h"

typedef fxp<int_type<U_wl>::T, U_iwl> U_type;
typedef fxp<int_type<M_wl>::T, M_iwl> M_type;
typedef fxp<int_type<B_wl>::T, B_iwl> B_type;
typedef fxp<int_type<F_wl>::T, F_iwl> F_type;
typedef fxp<int_type<P_wl>::T, P_iwl> P_type;
typedef fxp<int_type<L_wl>::T, L_iwl> L_type;
typedef fxp<int_type<S_wl>::T, S_iwl> S_type;

#elif defined(DT_FLOAT_INT)

typedef float U_type;
typedef float M_type;
typedef float B_type;
typedef float F_type;
typedef float P_type;
typedef float L_type;
typedef float S_type;

#elif defined(DT_HALF_INT)

#include "hls_half.h"

typedef half U_type;
typedef half M_type;
typedef half B_type;
typedef half F_type;
typedef half P_type;
typedef float L_type;
typedef float S_type;

#else
#error Need to defined DT_FIXED, DT_MIXED, DT_FLOAT or DT_HALF
#endif

typedef F_base F_flx[][num_features];
typedef P_base P_flx[][num_proteins];

typedef F_base F_arr[block_size][num_features];
typedef P_base P_arr[block_size][num_proteins];

typedef U_base U_arr[num_samples][num_proteins][num_latent];
typedef M_base M_arr[num_samples][num_latent];
typedef B_base B_arr[num_samples][num_features][num_latent];

typedef U_base U_flat[num_samples*num_proteins*num_latent];
typedef M_base M_flat[num_samples*num_latent];
typedef B_base B_flat[num_samples*num_features*num_latent];

void update_model(const U_arr U, const M_arr mu, const B_arr B);
void predict_compounds(int num_compounds, const F_flx in, P_flx out);

extern "C"
void predict_or_update_model(
               bool update_model,
               int num_compounds,
               const F_arr features,     //[block_size*num_features]
                     P_arr predictions,  //[block_size*num_proteins]
               const U_flat U_in,        //[num_samples][num_proteins][num_latent]
               const M_flat M_in,        //[num_samples][num_latent]
               const B_flat B_in);       //[num_samples][num_features][num_latent]