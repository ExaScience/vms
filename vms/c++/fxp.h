
#include <utility>
#include <cassert>
#include <cmath>
#include <cstdio>



template<typename T>
struct fxp
{
	static const int wl = sizeof(T) * 8;

    int iwl;
	T val;

    int shift() const { return wl - iwl; }

	float to_float() const {
		return ((float)val) / (float)(1L<<shift());
	}

    fxp() : iwl(0xdead), val(0xdead) {}

    fxp(int i) : iwl(i), val(0xdead) {}

    fxp(int i, float v)
        : iwl(i)
    {
        val = v*(1L<<shift()); 
        assert(fabs(v) < (1L<<(iwl-1)));
    }

    fxp(int i, T v)
        : iwl(i), val(v)
    {
        assert(fabs(v) < (1L<<(wl-1)));
    }

/*
    template<typename otherT>
    fxp(int i, fxp<otherT> v)
        : iwl(i)
    {
        mul_type temp_val = v.val >> (v.shift() - shift());
        assert(abs(temp_val) < (1L<<(wl-1)));
        val = temp_val;
    }

*/
    template<typename otherT>
    fxp(fxp<otherT> v)
    {
        iwl = v.iwl + wl - v.wl;
        int temp_val = v.val >> (v.shift() - shift());
        assert(abs(temp_val) < (1L<<(wl-1)));
        val = temp_val;
    }

    typedef std::pair<fxp<T>, fxp<T> > mul_expr;


    mul_expr operator*(const fxp<T> &other) const
    {
        return std::make_pair(*this, other);
    }

    fxp(int i, const mul_expr &mul)
        : iwl(i)
    {
        int s = mul.first.shift() + mul.second.shift() - i;
        if (s > 0) val = (mul.first.val * mul.second.val) >> s;
        else       val = (mul.first.val * mul.second.val) << -s;
    }

    fxp<T> operator+(const fxp<T> other) const
    {
        assert(iwl == other.iwl);
        return fxp<T>(iwl, (T)(val + other.val));
    }

    void print(const char *name) const
    {
        printf("%s\n val = %.2f\n", name, to_float());
        printf(" wl = %d\n", wl);
        printf(" iwl = %d\n", iwl);
    }
};

#define SHOW(F) F.print(#F)

template<typename T>
fxp<T> make_fxp(int iwl, const T &val)
{
    return fxp<T>(iwl, val);
}