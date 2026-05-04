#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(96, 128, 1.0, 2.0);
    auto r = mpfrxx::mpfr_class::with_precision(160, 3.0);

    mpfrxx::mpc_class materialized = z + r;
    if (materialized.real_precision() != 160 || materialized.imag_precision() != 128) {
        std::abort();
    }

    auto dst = mpfrxx::mpc_class::with_precision(80, 112);
    dst = z + r;
    if (dst.real_precision() != 80 || dst.imag_precision() != 112) {
        std::abort();
    }

    return 0;
}
