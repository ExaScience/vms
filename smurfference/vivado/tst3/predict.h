#include "smurff_const.h"


#if 1
#define SC_INCLUDE_FX
#include <systemc.h>


typedef sc_fixed<16,2>  Uin_type;
typedef sc_fixed<16,2>  Fin_type;
typedef sc_fixed<16,2>  Pout_type;
typedef sc_fixed<16,2>  Tin_type;
typedef sc_fixed<32,4>  S_type;

#else

typedef double  Uin_type;
typedef double  Fin_type;
typedef double  Pout_type;

#endif

void predict_compound_block_c(
   const Tin_type in[num_compounds][num_features],
   Pout_type out[num_compounds][num_proteins]
);
