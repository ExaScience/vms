#include <cassert>

#include "../fxp.h"

int main()
{
    auto f0  = fxp<short, 11>((short)0x200);
    SHOW(f0);

    fxp<short, 7> f1(20.2F);
    fxp<short, 5> f2( 2.1F);
    fxp<int, 25> f3( 2.1F);

    SHOW(f1);
    SHOW(f2);
    SHOW(f3);

    fxp<int, 20> m = f3 * f2;
    SHOW(m);
    assert((float)m - (float)f3 * (float)(f2) < 0.0000001);

    fxp<short, 10> n(m);
    SHOW(n);

    fxp<short, 10> q = f1 + f1;
    SHOW(q);

    return 0;

}