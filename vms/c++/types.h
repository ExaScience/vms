#pragma once

#include <cstdint>

#ifdef DT_FLOAT

#define DT_NAME "float"

typedef std::int16_t U_type;
typedef std::int8_t  mu_type;
typedef float F_type;
typedef float P_type;
typedef float B_type;
typedef float S_type;
typedef float T_type;

const float epsilon = 0.01;

#elif defined(DT_FIXED)
#define SC_INCLUDE_FX
#include <systemc.h>

// these seem to work with normally distributed
// inputs and models

#define DT_NAME "fixed"

typedef sc_fixed<24,6>  U_type;
typedef std::int8_t   mu_type;
typedef sc_fixed<20,1>  B_type;
typedef sc_fixed<20,10>  F_type;
typedef sc_fixed<16,10>  P_type;
typedef sc_fixed<32,10>  S_type;
typedef float  T_type;

const float epsilon = 1./8.;

#else
#error Need to define datatype
#endif
