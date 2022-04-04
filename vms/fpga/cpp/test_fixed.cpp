
#include <cstdint>
#include <cstdio>

#define SC_INCLUDE_FX
#include <systemc.h>


int main()
{
    float f = 1.234;
    const int shift = 8;
    const int wl = 32;
    const int iwl = wl - shift;

    std::int32_t f_int = round(f  * (1<<shift));

    sc_fixed<wl, wl> f_sc_int = f_int;
    sc_fixed<wl, iwl> f_sc = f_sc_int >> shift;

    printf("f = %.5f\n", f);
    printf("f_int = %d\n", f_int);
    printf("f_sc_int = %f\n", (float)f_sc_int);
    printf("f_sc = %f\n", (float)f_sc);

    return 0;

}