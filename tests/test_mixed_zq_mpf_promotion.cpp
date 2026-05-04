#include <gmpxx_mkII.h>

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
    gmpxx::mpz_class z(3);
    gmpxx::mpq_class q(gmpxx::mpz_class(1), gmpxx::mpz_class(4));
    auto f = gmpxx::mpf_class::with_precision(160, 2.5);

    gmpxx::mpf_class result = z + f;
    require_close(result.to_double(), 5.5);
    if (result.precision() != f.precision()) {
        std::abort();
    }

    result = f + q;
    require_close(result.to_double(), 2.75);

    result = (z + q) * f;
    require_close(result.to_double(), 8.125);

    result = z / q - f;
    require_close(result.to_double(), 9.5);

    return 0;
}
