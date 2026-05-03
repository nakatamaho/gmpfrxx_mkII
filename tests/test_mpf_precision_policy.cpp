#include <gmpxx_mkII.h>

#include <algorithm>
#include <cstdlib>

int main()
{
    unsetenv("MPFXX_DEFAULT_PREC_BITS");
    gmpxx::reload_default_mpf_precision_bits_from_environment();

    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    gmpxx::mpf_class builtin_default;
    if (builtin_default.precision() < 512) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "320", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 320) {
        std::abort();
    }
    gmpxx::mpf_class env_default;
    if (env_default.precision() < 320) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "0", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    gmpxx::set_default_mpf_precision_bits(160);
    gmpxx::mpf_class default_value;
    if (default_value.precision() < 160) {
        std::abort();
    }

    const auto low = gmpxx::mpf_class::with_precision(80, 1.0);
    const auto high = gmpxx::mpf_class::with_precision(192, 2.0);

    gmpxx::mpf_class materialized = low + high;
    if (materialized.precision() != std::max(low.precision(), high.precision())) {
        std::abort();
    }

    auto destination = gmpxx::mpf_class::with_precision(96);
    const auto destination_precision = destination.precision();
    destination = high + materialized;
    if (destination.precision() != destination_precision) {
        std::abort();
    }

    return 0;
}
