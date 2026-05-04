#include <gmpxx_mkII.h>

#include <algorithm>
#include <cstdlib>

int main()
{
    auto low = gmpxx::mpfc_class::with_precision(96, 128);
    auto high = gmpxx::mpfc_class::with_precision(192, 224);

    gmpxx::mpfc_class materialized = low + high;
    if (materialized.real_precision() != high.real_precision() ||
        materialized.imag_precision() != high.imag_precision()) {
        std::abort();
    }

    auto dst = gmpxx::mpfc_class::with_precision(80, 88);
    const auto dst_real_precision = dst.real_precision();
    const auto dst_imag_precision = dst.imag_precision();
    dst = low + high;
    if (dst.real_precision() != dst_real_precision ||
        dst.imag_precision() != dst_imag_precision) {
        std::abort();
    }

    auto real = gmpxx::mpf_class::with_precision(160, 2.0);
    gmpxx::mpfc_class mixed = real + low;
    if (mixed.real_precision() != real.precision() ||
        mixed.imag_precision() != std::max(real.precision(), low.imag_precision())) {
        std::abort();
    }

    return 0;
}
