
#include "fxp.h"

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