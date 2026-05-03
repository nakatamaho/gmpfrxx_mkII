#include <mpfrxx_mkII.h>

int main()
{
    auto x = mpfrxx::mpfr_class::with_precision(128);
    auto expr = x + 1.0L;
    (void)expr;
    return 0;
}
