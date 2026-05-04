#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    mpfrxx::set_default_precision_bits(128);
    mpfrxx::set_default_rounding_mode(MPFR_RNDA);

    setenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS", "0", 1);
    setenv("MPFRXX_MPC_REAL_PRECISION_BITS", "not-a-number", 1);
    setenv("MPFRXX_MPC_IMAG_PRECISION_BITS", "0", 1);
    setenv("MPFRXX_MPC_ROUNDING_MODE", "SIDEWAYS", 1);
    setenv("MPFRXX_MPC_REAL_ROUNDING_MODE", "NOPE", 1);
    setenv("MPFRXX_MPC_IMAG_ROUNDING_MODE", "", 1);
    mpfrxx::reload_mpc_defaults_from_environment();

    const auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 128 || defaults.imag_precision_bits != 128) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDA || defaults.imag_rounding_mode != MPFR_RNDA) {
        std::abort();
    }

    return 0;
}
