#include "smurff_const.h"


#ifdef DT_SC_FIXED
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

#elif defined(DT_DOUBLE)

typedef double  U_type;
typedef double  F_type;
typedef double  P_type;
typedef double  B_type;
typedef double  S_type;
typedef double  T_type;

const double epsilon = 0.00001;

#elif defined(DT_OBSERVED_DOUBLE)

#include <vector>

enum types { U_id, F_id, P_id, B_id, S_id, T_id, ntypes };

extern std::vector<double> values[ntypes];

template<enum types T>
struct od {

    double v;

    od(double d = .0) : v(d) { values[(int)T].push_back(d); }

    template<enum types S>
    od<T> &operator=(od<S> other)
    {
        v = other.v;
        values[int(T)].push_back(v);
        return *this; 
    }

    explicit operator double() const { return v; }

    template<enum types S>
    od operator-(const od<S> &other) const { return v - other.v; }

    template<enum types S>
    od operator*(const od<S> &other) const { return v * other.v; }

    template<enum types S>
    od operator+(const od<S> &other) const { return v + other.v; }
};

typedef od<U_id> U_type;
typedef od<F_id> F_type;
typedef od<P_id> P_type;
typedef od<B_id> B_type;
typedef od<S_id> S_type;
typedef od<T_id> T_type;

const double epsilon = 0.00001;

#else
#error "Define DT_SC_FIXED, DT_DOUBLE, or DT_OBSERVED_DOUBLE"
#endif

void predict_compound_block_c(
   const F_type in[num_compounds][num_features],
   P_type out[num_compounds][num_proteins]
);
