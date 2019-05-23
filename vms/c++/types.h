#pragma once

#ifdef DT_FLOAT

#define DT_NAME "float"

typedef float U_type;
typedef float mu_type;
typedef float F_type;
typedef float P_type;
typedef float B_type;
typedef float S_type;
typedef float T_type;

const float epsilon = 0.0001;

#elif defined(DT_FIXED)
#define SC_INCLUDE_FX
#include <systemc.h>

// these seem to work with normally distributed
// inputs and models

#define DT_NAME "fixed"

typedef sc_fixed<24,6>  U_type;
typedef sc_fixed<8,1>  mu_type;
typedef sc_fixed<20,1>  B_type;
typedef sc_fixed<20,10>  F_type;
typedef sc_fixed<16,10>  P_type;
typedef sc_fixed<32,10>  S_type;
typedef float  T_type;

const float epsilon = 1./8.;

#elif defined(DT_OBSERVED_FLOAT)
#include "observed_float.h"

#define DT_NAME "observed_float"

typedef ::od<U_id> U_type;
typedef ::od<mu_id> mu_type;
typedef ::od<F_id> F_type;
typedef ::od<P_id> P_type;
typedef ::od<B_id> B_type;
typedef ::od<S_id> S_type;
typedef ::od<T_id> T_type;

const float epsilon = 0.0001;

#else
#error Need to define datatype
#endif
