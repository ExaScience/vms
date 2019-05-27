
#include <cassert>
#include <cmath>
#include <cstdio>

template<typename T>
struct fxp
{
	static const int wl = sizeof(T) * 8;

	T val;
    int iwl;

    int shift() const { return wl - iwl; }

	float to_float() const {
		return ((float)val) / (float)(1L<<shift());
	}

    fxp(int i, float v = .0)
        : iwl(i)
    {
        val = v*(1L<<shift()); 
        assert(fabs(v) < (1L<<(iwl-1)));
    }

    fxp(int i, T v)
        : val(v), iwl(i)
    {
        assert(fabs(v) < (1L<<(wl-1)));
    }

    template<typename otherT>
    fxp(int i, fxp<otherT> v)
        : iwl(i)
    {
        mul_type temp_val = v.val >> (v.shift() - shift());
        printf("temp_val = %lld\n", temp_val);
        assert(abs(temp_val) < (1L<<(wl-1)));
        val = temp_val;
    }

    typedef int mul_type;

    fxp<mul_type> operator*(const fxp<T> other) const
    {
        fxp<mul_type> ret(iwl + other.iwl);

        int s = shift() + other.shift() - ret.shift();
        if (s > 0) ret.val = (mul_type)(val * other.val) >> s;
        else     ret.val = (mul_type)(val * other.val) << -s;
        return ret;
    }

    void print(const char *name) const
    {
        printf("%s\n val = %.2f\n", name, to_float());
        printf(" wl = %d\n", wl);
        printf(" iwl = %d\n", iwl);
    }
};

#define SHOW(F) F.print(#F)

int main()
{
    fxp<short> f1(7, 20.2F);
    fxp<short> f2(5, 2.1F);

    SHOW(f1);
    SHOW(f2);

    auto m = f1 * f2;
    SHOW(m);

    fxp<short> n(10, m);
    SHOW(n);


    return 0;

}