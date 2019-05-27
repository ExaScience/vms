
#include "fxp.h"

int main()
{
    short f0_val= 0x200;
    auto f0  = make_fxp(16-8, f0_val);
    SHOW(f0);

    fxp<short> f1(7, 20.2F);
    fxp<short> f2(5, 2.1F);

    SHOW(f1);
    SHOW(f2);

    auto m = f1 * f2;
    SHOW(m);

    fxp<short> n(10, m);
    SHOW(n);

    auto q = f1 + f1;
    SHOW(q);

    return 0;

}