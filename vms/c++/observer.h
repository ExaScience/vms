#include <vector>
#include <algorithm>
#include <iostream>

enum types { U_id, mu_id, F_id, P_id, B_id, S_id, T_id, L_id, ntypes };

template<typename T>
struct observer  { 
    std::vector<T> values;

    void observe(const T &v)
    {
        values.push_back(v);
    }

    ~observer()
    {
        std::cout << typeid(*this).name() << "; size: " << values.size();
        if (values.size() > 0)
        {
            std::cout << "; min: " << *std::min_element(values.begin(), values.end())
                    << "; max: " << *std::max_element(values.begin(), values.end());
        }
        std::cout << std::endl;
    } 
};

template<typename T, enum types I>
struct observable {
    T v;

    void observe(const T &v)
    {
        //call to observer
    }

    observable(T d = .0) : v(d) 
    {
       observe(d);
    }

    template<typename U, enum types J>
    observable<T,I> &operator=(observable<U,J> other)
    {
        v = other.v;
        observe(v);
        return *this; 
    }

    explicit operator T() const { return v; }

    template<typename U, enum types J>
    auto operator-(const observable<U,J> &other) const { return v - other.v; }

    template<typename U, enum types J>
    auto operator*(const observable<U,J> &other) const { return v * other.v; }
    
    template<typename U, enum types J>
    auto operator/(const observable<U,J> &other) const { return v / other.v; }

    template<typename U>
    auto operator/(const U &other) const { return v / other; }

    template<typename U, enum types J>
    auto operator+(const observable<U,J> &other) const { return v + other.v; }

    template<typename U, enum types J>
    auto operator+=(const observable<U,J> &other) { return v += other.v; }

    template<typename U, enum types J>
    auto operator/=(const observable<U,J> &other) { return v /= other.v; }
};


