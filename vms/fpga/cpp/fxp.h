#pragma once

#include <cmath>
#include <algorithm>
#include <cstdio>
#include <limits>
#include <iostream>

template<int N> struct int_type;
template<> struct int_type< 8> { typedef signed char T; };
template<> struct int_type<16> { typedef signed short T; };
template<> struct int_type<32> { typedef signed int T; };
template<> struct int_type<64> { typedef signed long long T; };

template<typename T, int IWL>
struct fxp;

template<typename T1, int IWL1, typename T2, int IWL2>
struct mul_expr
{
    fxp<T1, IWL1> a;
    fxp<T2, IWL2> b;
};

template<typename T1, int IWL1, typename T2, int IWL2>
mul_expr<T1, IWL1, T2, IWL2> operator*(const fxp<T1, IWL1> &a, const fxp<T2, IWL2> &b)
{
    mul_expr<T1, IWL1, T2, IWL2> r = {a,b};
    return r;
}

template<typename T1, int IWL1, typename T2, int IWL2>
struct add_expr
{
    fxp<T1, IWL1> a;
    fxp<T2, IWL2> b;
};

template<typename T1, int IWL1, typename T2, int IWL2>
add_expr<T1, IWL1, T2, IWL2> operator+(const fxp<T1, IWL1> &a, const fxp<T2, IWL2> &b)
{
    add_expr<T1, IWL1, T2, IWL2> r = {a,b};
    return r;
}

#define SHOW(F) (F).print(#F)

template<typename T, int IWL>
struct fxp
{
    static const int wl = sizeof(T) * 8;
    static const int iwl = IWL;
    static const int shift = wl - iwl;
    static const int fudge_factor = 256;
    static float epsilon()
    {
        float eps = ((float)fudge_factor / (float)(1L << shift));
        eps = std::max(eps, (float)fudge_factor * std::numeric_limits<float>::epsilon());
        //std::cerr << "fxp<" << wl << "," << iwl << ">" << " (eps: " << eps << ")" << std::endl;
        return eps;
    }

    typedef T base_type;

	T val;

	operator float() const {
		return ((float)val) / (float)(1L<<shift);
	}

    operator T() const
    {
        return val;
    }


#ifdef VMS_DT_CHECK
    void check(float ref, float eps = -1.) const
    {
        float cur = (float)(*this);
        float diff = std::abs(cur - ref);
        if (eps == -1.) eps = epsilon();

        if(diff < eps) 
        {
            // all clear :)
        }
        else
        {
            std::cerr << cur << " not equal to ref " << ref 
                      << " in fxp<" << wl << "," << iwl << ">"
                      << " (eps: " << eps 
                      << ", diff: " << diff 
                      << ", diff/eps: " << diff/eps << ")" << std::endl;
            //abort();
        }
    }
#else
    void check(float, float = -1.) const {}
#endif

    fxp(float v) : val(v*(1L<<shift)) { check(v); } 
    fxp(T v) : val(v) { } 
    fxp() : val() {} 

    template<typename otherT, int otherIWL>
    fxp(fxp<otherT, otherIWL> v)
    {
        const int s = v.shift - shift;
        if (wl >= v.wl) // assign + shift
        {
            val = v.val; // extend
            if (s > 0) val = val >> s;
            else       val = val << (-s);
        } else { // shift + assign
            if (s > 0) val = v.val >> s;
            else       val = v.val << (-s);
        }


#ifdef VMS_DT_CHECK
        float eps = std::max(fxp<otherT, otherIWL>::epsilon(), epsilon());
        check((float)v, eps);
#endif
    }

    template<typename T1, int IWL1, typename T2, int IWL2>
    fxp(const mul_expr<T1, IWL1, T2, IWL2> &mul)
    {
        int diff = mul.a.shift + mul.b.shift - shift;
        if (diff > 0)
            if (mul.a.shift > mul.b.shift)
                val = (T1)(mul.a.val >> diff) * mul.b.val;
            else 
                val = mul.a.val  * (T2)(mul.b.val >> diff);
        else
            val = (mul.a.val * mul.b.val) << -diff;

#ifdef VMS_DT_CHECK
        float eps;
        eps = std::max(fxp<T1, IWL1>::epsilon(), fxp<T2, IWL2>::epsilon());
        eps = std::max(eps, epsilon());
        check((float)mul.a * (float)mul.b, eps);
#endif
    }


    template<typename T1, int IWL1, typename T2, int IWL2>
    fxp(const add_expr<T1, IWL1, T2, IWL2> &add)
    {
        int diff = add.a.shift - add.b.shift;
        int max = add.a.shift > add.b.shift ? add.a.shift : add.b.shift;
        if (diff > 0)
            val = add.a.val + (add.b.val << diff);
        else
            val = (add.a.val << -diff) + add.b.val;

        int s = max - shift;
        if (s > 0) val = val << s;
        else       val = val >> -s;

#ifdef VMS_DT_CHECK
        float eps;
        eps = std::max(fxp<T1, IWL1>::epsilon(), fxp<T2, IWL2>::epsilon());
        eps = std::max(eps, epsilon());
        check((float)(add.a) + (float)(add.b), eps);
#endif
    }

    fxp<T, IWL> operator>>(const int s)
    {
        return fxp<T, IWL>((T)(val >> s));
    }

    void print(const char *name) const
    {
        printf("%s\n val = %.8f (%d)\n", name, (float)*this, val);
        printf(" wl = %d\n", wl);
        printf(" iwl = %d\n", iwl);
        printf(" shift = %d\n", shift);
    }
};
