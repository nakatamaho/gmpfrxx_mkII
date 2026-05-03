#include <mpfrxx_mkII.h>

#include <cstdlib>

namespace {

void clear_mpfr_environment()
{
    unsetenv("MPFRXX_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_EMIN");
    unsetenv("MPFRXX_EMAX");
    unsetenv("MPFRXX_ROUNDING_MODE");
    mpfrxx::reload_mpfr_defaults_from_environment();
}

} // namespace

int main()
{
    clear_mpfr_environment();

    if (mpfrxx::default_precision_bits() != 512) {
        std::abort();
    }
    if (mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        std::abort();
    }

    mpfrxx::set_default_precision_bits(113);
    mpfrxx::mpfr_class default_prec_value;
    if (default_prec_value.precision() != 113) {
        std::abort();
    }

    mpfrxx::set_default_rounding_mode(MPFR_RNDU);
    auto rounded_up = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded_up.to_double() != 1.5) {
        std::abort();
    }

    mpfrxx::set_default_rounding_mode(MPFR_RNDD);
    auto rounded_down = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded_down.to_double() != 1.0) {
        std::abort();
    }

    mpfrxx::set_default_exponent_range(-20, 20);
    if (mpfrxx::default_emin() != -20 || mpfrxx::default_emax() != 20) {
        std::abort();
    }

    return 0;
}
