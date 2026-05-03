#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    const mpfr_exp_t initial_emin = mpfr_get_emin();
    const mpfr_exp_t initial_emax = mpfr_get_emax();

    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "0", 1);
    setenv("MPFRXX_EMIN", "100", 1);
    setenv("MPFRXX_EMAX", "-100", 1);
    setenv("MPFRXX_ROUNDING_MODE", "SIDEWAYS", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    const auto invalid_defaults = mpfrxx::default_options();
    if (invalid_defaults.precision_bits != 512) {
        std::abort();
    }
    if (invalid_defaults.rounding_mode != MPFR_RNDN) {
        std::abort();
    }
    if (invalid_defaults.emin != initial_emin || invalid_defaults.emax != initial_emax) {
        std::abort();
    }

    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "96", 1);
    setenv("MPFRXX_EMIN", "-200", 1);
    setenv("MPFRXX_EMAX", "200", 1);
    setenv("MPFRXX_ROUNDING_MODE", "MPFR_RNDD", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    const auto valid_defaults = mpfrxx::default_options();
    if (valid_defaults.precision_bits != 96 ||
        valid_defaults.emin != -200 ||
        valid_defaults.emax != 200 ||
        valid_defaults.rounding_mode != MPFR_RNDD) {
        std::abort();
    }

    return 0;
}
