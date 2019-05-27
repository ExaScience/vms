
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
		return ((float)val) / (1<<shift());
	}

    fxp(int i, float v = .0)
        : val(v*(1<<shift())), iwl(i)
    {
        assert(fabs(v) < (1<<(iwl-1)));
    }

    fxp(int i, T v)
        : val(v), iwl(i)
    {
        assert(fabs(v) < (1<<(wl-1)));
    }

    fxp<long long> operator*(const fxp<T> other) const
    {
        fxp<long long> ret(iwl + other.iwl);
        ret.val = (val * other.val) >> (shift() + other.shift() - ret.shift());
        return ret;
    }
};

int main()
{
    fxp<short> f1(10, 20.2F);
    fxp<short> f2(4, 2.1F);

    printf("f1 = %.2f\n", f1.to_float());
    printf("f2 = %.2f\n", f2.to_float());

    return 0;

}