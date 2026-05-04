#include <mpfrxx_mkII.h>

#include <cstdlib>

namespace {

void clear_mpc_environment()
{
    unsetenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_REAL_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_IMAG_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_REAL_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_IMAG_ROUNDING_MODE");
}

} // namespace

int main()
{
    clear_mpc_environment();

    mpfrxx::set_default_precision_bits(512);
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    mpfrxx::reload_mpc_defaults_from_environment();

    auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 512 || defaults.imag_precision_bits != 512) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDN || defaults.imag_rounding_mode != MPFR_RNDN) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RNDNN) {
        std::abort();
    }

    mpfrxx::set_default_mpc_precision_bits(160, 192);
    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDU, MPFR_RNDD);

    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 160 || defaults.imag_precision_bits != 192) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDU || defaults.imag_rounding_mode != MPFR_RNDD) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RND(MPFR_RNDU, MPFR_RNDD)) {
        std::abort();
    }

    mpfrxx::mpc_class value;
    if (value.real_precision() != 160 || value.imag_precision() != 192) {
        std::abort();
    }

    return 0;
}
