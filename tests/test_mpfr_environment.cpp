#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "160", 1);
    setenv("MPFRXX_EMIN", "-64", 1);
    setenv("MPFRXX_EMAX", "64", 1);
    setenv("MPFRXX_ROUNDING_MODE", "RNDU", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    const auto defaults = mpfrxx::default_options();
    if (defaults.precision_bits != 160) {
        std::abort();
    }
    if (defaults.emin != -64 || defaults.emax != 64) {
        std::abort();
    }
    if (defaults.rounding_mode != MPFR_RNDU) {
        std::abort();
    }

    mpfrxx::mpfr_class default_prec_value;
    if (default_prec_value.precision() != 160) {
        std::abort();
    }

    auto rounded_up = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded_up.to_double() != 1.5) {
        std::abort();
    }

    const mpfr_exp_t old_emin = mpfr_get_emin();
    const mpfr_exp_t old_emax = mpfr_get_emax();
    auto x = mpfrxx::mpfr_class::with_precision(128, 2.0);
    auto y = mpfrxx::mpfr_class::with_precision(128, 3.0);
    mpfrxx::mpfr_class z = x + y;
    (void)z;
    if (mpfr_get_emin() != old_emin || mpfr_get_emax() != old_emax) {
        std::abort();
    }

    return 0;
}
