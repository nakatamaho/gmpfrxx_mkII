#include <gmpfrxx_mkII/adapters/binary128_complex.hpp>
#include <gmpfrxx_mkII/adapters/binary128_real.hpp>
#include <gmpfrxx_mkII/adapters/binary80_complex.hpp>
#include <gmpfrxx_mkII/adapters/binary80_real.hpp>
#include <gmpfrxx_mkII/adapters/dd_complex.hpp>
#include <gmpfrxx_mkII/adapters/dd_real.hpp>
#include <gmpfrxx_mkII/adapters/mpfc_complex.hpp>
#include <gmpfrxx_mkII/adapters/mpf_real.hpp>
#include <gmpfrxx_mkII/adapters/qd_complex.hpp>
#include <gmpfrxx_mkII/adapters/qd_real.hpp>

#include <mplapack/dd_complex.h>
#include <mplapack/mplapack_config.h>
#include <mplapack/qd_complex.h>

#include <cmath>
#include <complex>
#include <sstream>

namespace {

bool above(mpfr_srcptr value, double lower)
{
    return mpfr_cmp_d(value, lower) > 0;
}

bool below(mpfr_srcptr value, double upper)
{
    return mpfr_cmp_d(value, upper) < 0;
}

bool check_complex(const mpfrxx::mpc_class& value,
                   double real_lower,
                   double imag_upper)
{
    return above(mpc_realref(value.mpc_data()), real_lower) &&
           above(mpc_imagref(value.mpc_data()), imag_upper);
}

} // namespace

int main()
{
    const mpfrxx::mpfr_class mpfr_default;
    const gmpxx::mpf_class mpf_default;
    if (mpfr_default.precision() != 512 || mpf_default.precision() != 512) {
        return 1;
    }

    const mpfrxx::mpfr_class from_double(1.25);
    if (mpfr_cmp_d(from_double.mpfr_data(), 1.25) != 0) {
        return 2;
    }

    const dd_real dd_source(1.0, std::ldexp(1.0, -100));
    const mpfrxx::mpfr_class from_dd(dd_source);
    if (!above(from_dd.mpfr_data(), 1.0)) {
        return 3;
    }

    const qd_real qd_source(1.0, std::ldexp(1.0, -100), 0.0, 0.0);
    mpfrxx::mpfr_class from_qd;
    from_qd = qd_source;
    if (!above(from_qd.mpfr_data(), 1.0)) {
        return 4;
    }

    gmpxx::mpf_class mpf_source;
    gmpxx::mpf_class mpf_low;
    mpf_set_ui(mpf_source.mpf_data(), 1);
    mpf_set_ui(mpf_low.mpf_data(), 1);
    mpf_div_2exp(mpf_low.mpf_data(), mpf_low.mpf_data(), 100);
    mpf_add(mpf_source.mpf_data(), mpf_source.mpf_data(), mpf_low.mpf_data());
    const mpfrxx::mpfr_class from_mpf(mpf_source);
    if (!above(from_mpf.mpfr_data(), 1.0)) {
        return 5;
    }

#if MPLAPACK_BINARY80_MODE == MPLAPACK_BINARY80_MODE_FLOAT64X
    const mplapack_binary80_t binary80_source =
        static_cast<mplapack_binary80_t>(1.0L) +
        static_cast<mplapack_binary80_t>(0x1p-60L);
    const gmpfrxx_mkII::adapters::binary80_real binary80_boundary(
        static_cast<long double>(binary80_source));
    const mpfrxx::mpfr_class from_binary80(binary80_boundary);
    if (!above(from_binary80.mpfr_data(), 1.0)) {
        return 6;
    }
#endif

#if MPLAPACK_BINARY128_MODE == MPLAPACK_BINARY128_MODE_FLOAT128 && \
    GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128
    const mplapack_binary128_t binary128_source =
        static_cast<mplapack_binary128_t>(1.0L) +
        static_cast<mplapack_binary128_t>(0x1p-100L);
    using binary128_native =
        gmpfrxx_mkII::adapters::binary128_real::value_type;
    const gmpfrxx_mkII::adapters::binary128_real binary128_boundary(
        static_cast<binary128_native>(binary128_source));
    const mpfrxx::mpfr_class from_binary128(binary128_boundary);
    if (!above(from_binary128.mpfr_data(), 1.0)) {
        return 7;
    }
#endif

    const mpfrxx::mpc_class from_double_complex(
        std::complex<double>(1.25, -2.5));
    if (mpfr_cmp_d(mpc_realref(from_double_complex.mpc_data()), 1.25) != 0 ||
        mpfr_cmp_d(mpc_imagref(from_double_complex.mpc_data()), -2.5) != 0) {
        return 8;
    }

    dd_complex dd_complex_source;
    dd_complex_source.real(dd_real(3.0, std::ldexp(1.0, -100)));
    dd_complex_source.imag(dd_real(-4.0, std::ldexp(1.0, -101)));
    const mpfrxx::mpc_class from_dd_complex(dd_complex_source);
    if (!check_complex(from_dd_complex, 3.0, -4.0)) {
        return 9;
    }

    qd_complex qd_complex_source;
    qd_complex_source.real(qd_real(5.0, std::ldexp(1.0, -100), 0.0, 0.0));
    qd_complex_source.imag(qd_real(-6.0, std::ldexp(1.0, -101), 0.0, 0.0));
    const mpfrxx::mpc_class from_qd_complex(qd_complex_source);
    if (!check_complex(from_qd_complex, 5.0, -6.0)) {
        return 10;
    }

#if MPLAPACK_BINARY80_MODE == MPLAPACK_BINARY80_MODE_FLOAT64X
    const std::complex<mplapack_binary80_t> binary80_complex_source(
        static_cast<mplapack_binary80_t>(7.0L) +
            static_cast<mplapack_binary80_t>(0x1p-60L),
        static_cast<mplapack_binary80_t>(-8.0L) +
            static_cast<mplapack_binary80_t>(0x1p-61L));
    const mpfrxx::mpc_class from_binary80_complex(binary80_complex_source);
    if (!check_complex(from_binary80_complex, 7.0, -8.0)) {
        return 11;
    }
#endif

#if MPLAPACK_BINARY128_MODE == MPLAPACK_BINARY128_MODE_FLOAT128 && \
    GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128
    const std::complex<mplapack_binary128_t> binary128_complex_source(
        static_cast<mplapack_binary128_t>(9.0L) +
            static_cast<mplapack_binary128_t>(0x1p-100L),
        static_cast<mplapack_binary128_t>(-10.0L) +
            static_cast<mplapack_binary128_t>(0x1p-101L));
    const mpfrxx::mpc_class from_binary128_complex(binary128_complex_source);
    if (!check_complex(from_binary128_complex, 9.0, -10.0)) {
        return 12;
    }
#endif

    gmpxx::mpf_class mpfc_real = mpf_source;
    gmpxx::mpf_class mpfc_imag;
    mpf_set_si(mpfc_imag.mpf_data(), -2);
    mpf_add(mpfc_imag.mpf_data(), mpfc_imag.mpf_data(), mpf_low.mpf_data());
    const gmpxx::mpfc_class mpfc_source(mpfc_real, mpfc_imag);
    const mpfrxx::mpc_class from_mpfc(mpfc_source);
    if (!check_complex(from_mpfc, 1.0, -2.0)) {
        return 13;
    }

    const mpfrxx::mpfr_class mpfr_value = 2;
    const mpfrxx::mpfr_class mpfr_result =
        mpfr_value * mpfr_value + sqrt(mpfr_value);
    const mpfrxx::mpc_class mpc_value(mpfr_value, -mpfr_value);
    const mpfrxx::mpc_class mpc_result = mpc_value * mpc_value + mpc_value;

    gmpxx::mpf_class tiny = 1;
    tiny.div_2exp(200);
    const gmpxx::mpf_class high = gmpxx::mpf_class(1) + tiny;
    const gmpxx::mpf_class high_squared = high * high;
    const gmpxx::mpfc_class mpfc_value(high, -high);
    const gmpxx::mpfc_class mpfc_result = mpfc_value * mpfc_value + mpfc_value;
    if (high_squared - 1 <= tiny) {
        return 14;
    }

    const mpfrxx::mpfr_class lifetime_real =
        (mpfrxx::mpfr_class(1) + mpfrxx::mpfr_class(2)) *
        (mpfrxx::mpfr_class(3) + mpfrxx::mpfr_class(4));
    if (mpfr_cmp_ui(lifetime_real.mpfr_data(), 21) != 0) {
        return 15;
    }

    std::ostringstream output;
    output << mpfr_result << mpc_result << gmpxx::const_pi() << mpfc_result;
    if (output.str().empty() || !below(mpc_imagref(from_dd_complex.mpc_data()), 0.0)) {
        return 16;
    }
    return 0;
}
