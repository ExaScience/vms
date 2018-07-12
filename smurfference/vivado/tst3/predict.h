#include "smurff_const.h"

typedef double  Uin_type;
typedef double  Fin_type;
typedef double  Pout_type;

void predict_compound_block_c(
   const double in[num_compounds][num_features],
   double out[num_compounds][num_proteins]
);