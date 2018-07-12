#define SC_INCLUDE_FX
#include <systemc.h>

// these seem to work with normally distributed
// inputs and models

const int WL = 16;
typedef sc_fixed<WL,5>    U_type;
typedef sc_fixed<WL,4>    F_type;
typedef sc_fixed<WL,6>    P_type;
typedef sc_fixed<WL,3>    B_type;
typedef sc_fixed<2*WL,6>  S_type;
typedef sc_fixed<2*WL,6>  T_type;

const double epsilon = 1./(WL-8);

