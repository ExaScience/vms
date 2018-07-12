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
