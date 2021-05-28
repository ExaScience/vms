#include <vector>
#include <algorithm>

enum types { U_id, M_id, F_id, P_id, B_id, S_id, T_id, ntypes };

extern std::vector<float> values[ntypes];

template<enum types T>
struct od {

    float v;

    od(float d = .0) : v(d) { values[(int)T].push_back(d); }

    template<enum types S>
    od<T> &operator=(od<S> other)
    {
        v = other.v;
        values[int(T)].push_back(v);
        return *this; 
    }

    explicit operator float() const { return v; }

    template<enum types S>
    od operator-(const od<S> &other) const { return v - other.v; }

    template<enum types S>
    od operator*(const od<S> &other) const { return v * other.v; }
    
    template<enum types S>
    od operator/(const od<S> &other) const { return v / other.v; }

    template<enum types S>
    od operator+(const od<S> &other) const { return v + other.v; }

    template<enum types S>
    od operator+=(const od<S> &other) { return v += other.v; }

    template<enum types S>
    od operator/=(const od<S> &other) { return v /= other.v; }
};
