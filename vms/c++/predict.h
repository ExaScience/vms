#if defined(DT_FIXED) || defined(DT_FLOAT) || defined(DT_HALF) || defined(DT_MIXED)
#else
#define DT_FLOAT
#endif

#include "vms_const.h"

static const int block_size = 1000;

const int L_wl = 32;
const int L_iwl = 12;
const int S_wl = 32;
const int S_iwl = 12;

#ifdef DT_FIXED

#define DT_NAME "fxp<T,I>"

#include "fxp.h"

template<int N> struct int_type;
template<> struct int_type< 8> { typedef signed char T; };
template<> struct int_type<16> { typedef signed short T; };
template<> struct int_type<32> { typedef signed int T; };

typedef int_type<U_wl>::T U_base;
typedef int_type<M_wl>::T M_base;
typedef int_type<B_wl>::T B_base;
typedef int_type<F_wl>::T F_base;
typedef int_type<P_wl>::T P_base;
typedef int_type<L_wl>::T L_base;
typedef int_type<S_wl>::T S_base;

typedef fxp<U_base, U_iwl> U_type;
typedef fxp<M_base, M_iwl> M_type;
typedef fxp<B_base, B_iwl> B_type;
typedef fxp<F_base, F_iwl> F_type;
typedef fxp<P_base, P_iwl> P_type;
typedef fxp<L_base, L_iwl> L_type;
typedef fxp<S_base, S_iwl> S_type;

const float epsilon = 0.5;

#define CRC_INIT(crc) (crc) = 0xd1
#define CRC_ADD(crc, value) (crc) ^= (value)
#define CRC_FMT "0x%04x"

#elif defined(DT_MIXED)

#define DT_NAME "float + sc_fixed<WL,IWL>"

#define SC_INCLUDE_FX
#include <systemc.h>

typedef float U_base;
typedef float M_base;
typedef float B_base;
typedef float F_base;
typedef float P_base;

typedef sc_fixed<U_wl, U_iwl> U_type;
typedef sc_fixed<M_wl, M_iwl> M_type;
typedef sc_fixed<B_wl, B_iwl> B_type;
typedef sc_fixed<F_wl, F_iwl> F_type;
typedef sc_fixed<P_wl, P_iwl> P_type;

typedef sc_fixed<L_wl, L_iwl> L_type;
typedef sc_fixed<S_wl, S_iwl> S_type;

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

#elif defined(DT_HALF)

#define DT_NAME "half"

#include “hls_half.h”

typedef half U_base;
typedef half M_base;
typedef half B_base;
typedef half F_base;
typedef half P_base;
typedef float L_base;
typedef float S_base;

typedef half U_type;
typedef half M_type;
typedef half B_type;
typedef half F_type;
typedef half P_type;
typedef float L_type;
typedef float S_type;

const float epsilon = 0.5;

#define CRC_INIT(crc) (crc) = .5
#define CRC_ADD(crc, value) (crc) += (value)
#define CRC_FMT "%.2f"

#else
#error Need to defined DT_FIXED, DT_FLOAT or DT_HALF
#endif

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
