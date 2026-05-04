#include <mpfrxx_mkII.h>

#include <cmath>
#include <cstdlib>

namespace {

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(160, 3.0, 4.0);
    auto r = mpfrxx::mpfr_class::with_precision(160, 5.0);
    mpfrxx::mpz_class exact_z(7);
    mpfrxx::mpq_class exact_q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    mpfrxx::mpc_class result = z + w;
    require_close(result.real_to_double(), 4.0);
    require_close(result.imag_to_double(), 6.0);
    if (result.precision() != 160) {
        std::abort();
    }

    result = z * w;
    require_close(result.real_to_double(), -5.0);
    require_close(result.imag_to_double(), 10.0);

    result = z + r - exact_z + exact_q + 2;
    require_close(result.real_to_double(), 1.5);
    require_close(result.imag_to_double(), 2.0);

    result = -z;
    require_close(result.real_to_double(), -1.0);
    require_close(result.imag_to_double(), -2.0);

    return 0;
}
