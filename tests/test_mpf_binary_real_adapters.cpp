#include <gmpfrxx_mkII/adapters/binary80_real.hpp>
#include <gmpfrxx_mkII/adapters/binary128_real.hpp>
#include <gmpfrxx_mkII/adapters/mpf_real.hpp>
#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>

#include <cmath>

static bool above_one(const mpfrxx::mpfr_class& value)
{
    return mpfr_cmp_ui(value.mpfr_data(), 1) > 0;
}

int main()
{
    gmpxx::mpf_class f(512);
    mpf_set_d(f.mpf_data(), 1.0);
    mpf_t low;
    mpf_init2(low, 512);
    mpf_set_d(low, std::ldexp(1.0, -100));
    mpf_add(f.mpf_data(), f.mpf_data(), low);
    const mpfrxx::mpfr_class from_mpf = f;
    mpf_clear(low);
    if (!above_one(from_mpf)) {
        return 1;
    }

#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY80
    {
        const long double value = 1.0L + 0x1p-60L;
        if (!above_one(mpfrxx::mpfr_class(
                gmpfrxx_mkII::adapters::make_binary80_source(value)))) {
            return 2;
        }
    }
#endif

#if GMPFRXX_MKII_ADAPTERS_HAVE_FLOAT64X_BINARY80
    {
        const _Float64x value = static_cast<_Float64x>(1.0L) +
                                static_cast<_Float64x>(0x1p-60L);
        if (!above_one(mpfrxx::mpfr_class(
                gmpfrxx_mkII::adapters::make_binary80_source(value)))) {
            return 3;
        }
    }
#endif

#if GMPFRXX_MKII_ADAPTERS_HAVE_GNU_FLOAT128 && GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128
    {
        const __float128 value = static_cast<__float128>(1.0L) +
                                 static_cast<__float128>(0x1p-100L);
        if (!above_one(mpfrxx::mpfr_class(
                gmpfrxx_mkII::adapters::make_binary128_source(value)))) {
            return 4;
        }
    }
#endif

#if GMPFRXX_MKII_ADAPTERS_HAVE_ISO_FLOAT128 && GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128
    {
        const _Float128 value = static_cast<_Float128>(1.0L) +
                                static_cast<_Float128>(0x1p-100L);
        if (!above_one(mpfrxx::mpfr_class(
                gmpfrxx_mkII::adapters::make_binary128_source(value)))) {
            return 5;
        }
    }
#endif

#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128
    {
        const long double value = 1.0L + 0x1p-100L;
        if (!above_one(mpfrxx::mpfr_class(
                gmpfrxx_mkII::adapters::make_binary128_source(value)))) {
            return 6;
        }
    }
#endif

    return 0;
}
