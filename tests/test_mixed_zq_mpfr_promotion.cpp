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
    mpfrxx::mpz_class z(3);
    mpfrxx::mpq_class q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(4));
    auto r = mpfrxx::mpfr_class::with_precision(160, 2.5);

    mpfrxx::mpfr_class result = z + r;
    require_close(result.to_double(), 5.5);
    if (result.precision() != 160) {
        std::abort();
    }

    result = r + q;
    require_close(result.to_double(), 2.75);

    mpfrxx::mpfr_class materialized = (z + q) + r;
    require_close(materialized.to_double(), 5.75);

    return 0;
}
