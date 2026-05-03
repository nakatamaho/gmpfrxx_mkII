#include <gmpfrxx_mkII.h>

int main()
{
    auto f = gmpxx::mpf_class::with_precision(128, 1.0);
    auto r = mpfrxx::mpfr_class::with_precision(128, 2.0);
    auto expr = r + f;
    (void)expr;
    return 0;
}
