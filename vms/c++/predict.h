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
typedef fxp<mu_base, mu_iwl> mu_type;
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

#elif defined(DT_FLOAT)

#define DT_NAME "float"

typedef float U_base ;
typedef float U_type;

typedef float mu_base ;
typedef float mu_type;

typedef float B_base ;
typedef float B_type;

typedef float F_base ;
typedef float F_type;

typedef float P_base ;
typedef float P_type;

typedef float L_base;
typedef float L_type;

typedef float S_base;
typedef float S_type;

const float epsilon = 0.5;

#else
#error Need to defined DT_FIXED or DT_FLOAT
#endif

void update_model(
    const U_base U[num_samples][num_proteins][num_latent],
    const mu_base mu[num_samples][num_latent],
    const B_base B[num_samples][num_features][num_latent],
    const P_base U_check,
    const P_base mu_check,
    const P_base B_chec);

void predict_compound(
    int num_compounds,
    const F_base  in [][num_features],
          P_base  out[][num_proteins]
);
