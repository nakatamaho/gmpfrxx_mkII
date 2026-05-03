#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);

    const mpfrxx::mpq_class one_third(mpfrxx::mpz_class(1), mpfrxx::mpz_class(3));
    const auto zero = mpfrxx::mpfr_class::with_precision(2, 0.0);

    const auto exact_sum =
        one_third + one_third + one_third + one_third + one_third;
    mpfrxx::mpfr_class result = exact_sum + zero;

    if (result.precision() != 2) {
        std::abort();
    }
    if (result.to_double() != 1.5) {
        std::abort();
    }

    return 0;
}
