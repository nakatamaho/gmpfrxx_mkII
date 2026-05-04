#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "144", 1);
    setenv("MPFRXX_ROUNDING_MODE", "RNDD", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    unsetenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_REAL_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_IMAG_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_REAL_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_IMAG_ROUNDING_MODE");
    mpfrxx::reload_mpc_defaults_from_environment();

    auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 144 || defaults.imag_precision_bits != 144) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDD || defaults.imag_rounding_mode != MPFR_RNDD) {
        std::abort();
    }

    setenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS", "160", 1);
    setenv("MPFRXX_MPC_REAL_PRECISION_BITS", "192", 1);
    setenv("MPFRXX_MPC_IMAG_PRECISION_BITS", "224", 1);
    setenv("MPFRXX_MPC_ROUNDING_MODE", "RNDU", 1);
    setenv("MPFRXX_MPC_REAL_ROUNDING_MODE", "RNDD", 1);
    setenv("MPFRXX_MPC_IMAG_ROUNDING_MODE", "RNDZ", 1);
    mpfrxx::reload_mpc_defaults_from_environment();

    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 224) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDD || defaults.imag_rounding_mode != MPFR_RNDZ) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RND(MPFR_RNDD, MPFR_RNDZ)) {
        std::abort();
    }

    mpfrxx::mpc_class value;
    if (value.real_precision() != 192 || value.imag_precision() != 224) {
        std::abort();
    }

    return 0;
}
