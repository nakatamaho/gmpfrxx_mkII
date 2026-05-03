#include <mpfrxx_mkII.h>

#include <cstdlib>

int main()
{
    const auto low = mpfrxx::mpfr_class::with_precision(80);
    const auto high = mpfrxx::mpfr_class::with_precision(192);

    mpfrxx::mpfr_class materialized = low + high;
    if (materialized.precision() != 192) {
        std::abort();
    }

    auto destination = mpfrxx::mpfr_class::with_precision(96);
    destination = high + materialized;
    if (destination.precision() != 96) {
        std::abort();
    }

    mpfrxx::mpfr_class unary = -high;
    if (unary.precision() != 192) {
        std::abort();
    }

    return 0;
}
