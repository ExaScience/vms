
#include "../fxp.h"

int main()
{
    auto f0  = fxp<short, 11>((short)0x200);
    SHOW(f0);

    fxp<short, 7> f1(20.2F);
    fxp<short, 5> f2( 2.1F);

    SHOW(f1);
    SHOW(f2);

    fxp<int, 20> m = f1 * f2;
    SHOW(m);

    fxp<short, 10> n(m);
    SHOW(n);

    fxp<short, 10> q = f1 + f1;
    SHOW(q);

    return 0;

}