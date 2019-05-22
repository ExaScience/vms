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


// U; size: 1049856; min: -18.8395; max: 14.6594 
// F; size: 825909; min: -12.1851; max: 10.8723
// P; size: 2280; min: 0; max: 8.98226
// B; size: 82544; min: -0.51856; max: 0.469575
// S; size: 229254; min: -1.31922; max: 98.8048    

typedef sc_fixed<16,6>    U_type;
typedef sc_fixed<16,4>   mu_type;
typedef sc_fixed<16,5>    F_type;
typedef sc_fixed<16,5>    P_type;
typedef sc_fixed<16,5>    B_type;
typedef sc_fixed<20,9>  S_type;
typedef sc_fixed<20,9>  T_type;

const double epsilon = 1./8.;

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
