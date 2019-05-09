#define SC_INCLUDE_FX
#include <systemc.h>

// these seem to work with normally distributed
// inputs and models

const int WL = 16;
typedef sc_fixed<WL,3>    U_type;
typedef sc_fixed<WL,3>   mu_type;
typedef sc_fixed<WL,4>    F_type;
typedef sc_fixed<WL,7>    P_type;
typedef sc_fixed<WL,4>    B_type;
typedef sc_fixed<4+WL,7>  S_type;
typedef sc_fixed<4+WL,7>  T_type;

const double epsilon = 1./(WL-8);

