#include "vms_const.h"

extern int verbose;

const int L_wl = 32;
const int L_iwl = 12;
const int S_wl = 32;
const int S_iwl = 12;

const float epsilon = 1.0F;

#if defined(VMS_DT_FIXED)
#define VMS_DT_NAME "fxp<T,I>"
#define VMS_DT_FIXED_INT
#define VMS_DT_FIXED_EXT
#elif defined(VMS_DT_MIXED)
#define VMS_DT_NAME "float + fxp<WL,IWL>"
#define VMS_DT_FIXED_INT
#define VMS_DT_FLOAT_EXT
#elif defined(VMS_DT_FLOAT)
#define VMS_DT_NAME "float"
#define VMS_DT_FLOAT_INT
#define VMS_DT_FLOAT_EXT
#elif defined(VMS_DT_HALF)
#define VMS_DT_NAME "half"
#define VMS_DT_FLOAT_EXT
#define VMS_DT_HALF_INT
#else
#error Need to define VMS_DT_FIXED, VMS_DT_MIXED, VMS_DT_FLOAT or VMS_DT_HALF
#endif

#ifdef VMS_DT_FIXED_EXT

#include "fxp.h"
#include "arr.h"

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

#else // VMS_DT_FLOAT_EXT

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

#ifdef VMS_DT_FIXED_INT

#include "fxp.h"

typedef fxp<int_type<U_wl>::T, U_iwl> U_type;
typedef fxp<int_type<M_wl>::T, M_iwl> M_type;
typedef fxp<int_type<B_wl>::T, B_iwl> B_type;
typedef fxp<int_type<F_wl>::T, F_iwl> F_type;
typedef fxp<int_type<P_wl>::T, P_iwl> P_type;
typedef fxp<int_type<L_wl>::T, L_iwl> L_type;
typedef fxp<int_type<S_wl>::T, S_iwl> S_type;

#elif defined(VMS_DT_FLOAT_INT)

typedef float U_type;
typedef float M_type;
typedef float B_type;
typedef float F_type;
typedef float P_type;
typedef float L_type;
typedef float S_type;

#elif defined(VMS_DT_HALF_INT)

#include "hls_half.h"

typedef half U_type;
typedef half M_type;
typedef half B_type;
typedef half F_type;
typedef half P_type;
typedef float L_type;
typedef float S_type;

#else
#error Need to defined VMS_DT_FIXED, VMS_DT_MIXED, VMS_DT_FLOAT or VMS_DT_HALF
#endif


typedef F_base F_one[num_features];
typedef P_base P_one[num_proteins][num_samples];

typedef F_base F_blk[block_size][num_features];
typedef P_base P_blk[block_size][num_proteins][num_samples];

typedef F_blk *F_blks;
typedef P_blk *P_blks;

typedef U_base U_arr[num_samples][num_proteins][num_latent];
typedef M_base M_arr[num_samples][num_latent];
typedef B_base B_arr[num_samples][num_features][num_latent];

typedef U_base U_one[num_proteins][num_latent];
typedef M_base M_one[num_latent];
typedef B_base B_one[num_features][num_latent];

typedef U_one *U_flx;
typedef M_one *M_flx;
typedef B_one *B_flx;

#ifdef VMS_HOSTIF_OPENCL
#define ALIGN_MODEL_ATTR __attribute__((aligned (4096)))
#else
#define ALIGN_MODEL_ATTR
#endif

struct Model {
	int nr = -1;
	ALIGN_MODEL_ATTR U_arr U;   //[num_samples][num_proteins][num_latent]
	ALIGN_MODEL_ATTR M_arr M;   //[num_samples][num_latent]
	ALIGN_MODEL_ATTR B_arr B;   //[num_samples][num_features][num_latent]
};

void predict_compounds(
    int num_blocks,
    const F_blks features,
    P_blks predictions,
    const Model &m);

void mpi_init(int &mpi_world_size, int &mpi_world_rank);
void mpi_finit();
void mpi_combine_results(int num_blocks, P_blks predictions);