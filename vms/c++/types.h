#pragma once

#ifdef DT_DOUBLE

#define DT_NAME "double"

typedef double U_type;
typedef double mu_type;
typedef double F_type;
typedef double P_type;
typedef double B_type;
typedef double S_type;
typedef double T_type;

const double epsilon = 0.00001;

#elif defined(DT_FIXED)
#define SC_INCLUDE_FX
#include <systemc.h>

// these seem to work with normally distributed
// inputs and models

#define DT_NAME "fixed"

const int WL = 16;
typedef sc_fixed<WL,4>    U_type;
typedef sc_fixed<WL,4>   mu_type;
typedef sc_fixed<WL,4>    F_type;
typedef sc_fixed<WL,7>    P_type;
typedef sc_fixed<WL,4>    B_type;
typedef sc_fixed<4+WL,7>  S_type;
typedef sc_fixed<4+WL,7>  T_type;

const double epsilon = 1./(WL-8);

#elif defined(DT_OBSERVED_DOUBLE)
#include "observed_double.h"

#define DT_NAME "observed_double"

typedef ::od<U_id> U_type;
typedef ::od<U_id> mu_type;
typedef ::od<F_id> F_type;
typedef ::od<P_id> P_type;
typedef ::od<B_id> B_type;
typedef ::od<S_id> S_type;
typedef ::od<T_id> T_type;

const double epsilon = 0.00001;

#else
#error Need to define datatype
#endif