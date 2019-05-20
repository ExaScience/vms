#pragma once


#define SC_INCLUDE_FX
#include <systemc.h>

#include "observed_double.h"

namespace smurff_vms
{

    namespace dbl
    {
        typedef double U_type;
        typedef double mu_type;
        typedef double F_type;
        typedef double P_type;
        typedef double B_type;
        typedef double S_type;
        typedef double T_type;

        const double epsilon = 0.00001;
    } // namespace dbl

    namespace fixed
    {

        // these seem to work with normally distributed
        // inputs and models

        const int WL = 16;
        typedef sc_fixed<WL,4>    U_type;
        typedef sc_fixed<WL,4>   mu_type;
        typedef sc_fixed<WL,4>    F_type;
        typedef sc_fixed<WL,7>    P_type;
        typedef sc_fixed<WL,4>    B_type;
        typedef sc_fixed<4+WL,7>  S_type;
        typedef sc_fixed<4+WL,7>  T_type;

        const double epsilon = 1./(WL-8);
    } // namespace fixed

    namespace od
    {

        typedef od<U_id> U_type;
        typedef od<U_id> mu_type;
        typedef od<F_id> F_type;
        typedef od<P_id> P_type;
        typedef od<B_id> B_type;
        typedef od<S_id> S_type;
        typedef od<T_id> T_type;

        const double epsilon = 0.00001
    } // namespace od

} // end namespace smurff_vms
