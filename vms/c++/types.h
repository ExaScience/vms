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
