#include <gmpfrxx_mkII.h>

int main()
{
    auto f = gmpxx::mpf_class::with_precision(128, 1.0);
    mpfrxx::mpc_class z;
    auto expr = z + f;
    (void)expr;
    return 0;
}
