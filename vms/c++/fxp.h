
#include <utility>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

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
    return mul_expr<T1, IWL1, T2, IWL2>{a,b};
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
    return add_expr<T1, IWL1, T2, IWL2>{a,b};
}

template<typename T, int IWL>
struct fxp
{
	static const int wl = sizeof(T) * 8;
    static const int iwl = IWL;
    static const int shift = wl - iwl;
	T val;

	float to_float() const {
		return ((float)val) / (float)(1L<<shift);
	}

    fxp() : val(0xdead) {}

    void check() const
    {
        if(fabs(val >> shift) > (1L<<(iwl-1)))
        {
            std::cerr << to_float() << " does not fit in fxp<" << wl << "," << iwl << ">" << std::endl;
            abort();
        }
    }

    fxp(float v) : val(v*(1L<<shift)) { check(); } 

    fxp(T v) : val(v) { check(); } 

    template<typename otherT, int otherIWL>
    fxp(fxp<otherT, otherIWL> v)
    {
        val = v.val >> (v.shift - shift);
        check();
    }

    template<typename T1, int IWL1, typename T2, int IWL2>
    fxp(const mul_expr<T1, IWL1, T2, IWL2> &mul)
    {
        int s = mul.a.shift + mul.b.shift - shift;

        if (s > 0)
            val = (mul.a.val * mul.b.val) >> s;
        else
            val = (mul.a.val * mul.b.val) << -s;
    }

    template<typename T1, int IWL1, typename T2, int IWL2>
    fxp(const add_expr<T1, IWL1, T2, IWL2> &add)
    {
        fxp<T, IWL> a =  add.a; // convert to target type
        fxp<T, IWL> b =  add.b; // idem
        val = a.val + b.val;
    }
    void print(const char *name) const
    {
        printf("%s\n val = %.2f\n", name, to_float());
        printf(" wl = %d\n", wl);
        printf(" iwl = %d\n", iwl);
    }
};

#define SHOW(F) F.print(#F)
