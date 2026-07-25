#include <gmpfrxx_mkII/adapters/binary128_complex.hpp>
#include <gmpfrxx_mkII/adapters/binary80_complex.hpp>
#include <gmpfrxx_mkII/adapters/dd_complex.hpp>
#include <gmpfrxx_mkII/adapters/mpfc_complex.hpp>
#include <gmpfrxx_mkII/adapters/qd_complex.hpp>

#include <mplapack/dd_complex.h>
#include <mplapack/qd_complex.h>
#include <mplapack/mplapack_config.h>

#include <cstdlib>
#include <cmath>
#include <complex>

namespace {

void require(bool condition)
{
    if (!condition) {
        std::abort();
    }
}

template <typename Real>
void require_components(const mpfrxx::mpc_class& actual,
                        const Real& expected_real,
                        const Real& expected_imag)
{
    const mpfrxx::mpfr_class real(expected_real);
    const mpfrxx::mpfr_class imag(expected_imag);
    require(mpfr_cmp(mpc_realref(actual.mpc_data()), real.mpfr_data()) == 0);
    require(mpfr_cmp(mpc_imagref(actual.mpc_data()), imag.mpfr_data()) == 0);
}

void require_below_binary64_components(const mpfrxx::mpc_class& actual,
                                       double real_high,
                                       double imag_high)
{
    require(mpfr_cmp_d(mpc_realref(actual.mpc_data()), real_high) > 0);
    require(mpfr_cmp_d(mpc_imagref(actual.mpc_data()), imag_high) > 0);
}

// COMPLEX-DOUBLE
void test_complex_double()
{
    const std::complex<double> source(1.25, -2.5);
    const mpfrxx::mpc_class target(source);
    require_components(target, source.real(), source.imag());
}

// COMPLEX-DD
void test_complex_dd()
{
    dd_complex source;
    source.real(dd_real(1.0, std::ldexp(1.0, -100)));
    source.imag(dd_real(-2.0, std::ldexp(1.0, -101)));
    const mpfrxx::mpc_class target(source);
    require_components(target, source.real(), source.imag());
    require_below_binary64_components(target, 1.0, -2.0);
}

// COMPLEX-QD
void test_complex_qd()
{
    qd_complex source;
    source.real(qd_real(3.0, std::ldexp(1.0, -100), 0.0, 0.0));
    source.imag(qd_real(-4.0, std::ldexp(1.0, -101), 0.0, 0.0));
    mpfrxx::mpc_class target;
    target = source;
    require_components(target, source.real(), source.imag());
    require_below_binary64_components(target, 3.0, -4.0);
}

// COMPLEX-BINARY80
void test_complex_binary80()
{
#if MPLAPACK_BINARY80_MODE == MPLAPACK_BINARY80_MODE_FLOAT64X
    const mplapack_binary80_t real = static_cast<mplapack_binary80_t>(1.0L) +
                                      static_cast<mplapack_binary80_t>(0x1p-60L);
    const mplapack_binary80_t imag = static_cast<mplapack_binary80_t>(-2.0L) +
                                      static_cast<mplapack_binary80_t>(0x1p-61L);
    const std::complex<mplapack_binary80_t> source(real, imag);
    const mpfrxx::mpc_class target(source);
    const gmpfrxx_mkII::adapters::binary80_real expected_real(
        static_cast<long double>(real));
    const gmpfrxx_mkII::adapters::binary80_real expected_imag(
        static_cast<long double>(imag));
    require_components(target, expected_real, expected_imag);
    require_below_binary64_components(target, 1.0, -2.0);
#endif
}

// COMPLEX-BINARY128
void test_complex_binary128()
{
#if MPLAPACK_BINARY128_MODE == MPLAPACK_BINARY128_MODE_FLOAT128
    const mplapack_binary128_t real = static_cast<mplapack_binary128_t>(1.0L) +
                                       static_cast<mplapack_binary128_t>(0x1p-100L);
    const mplapack_binary128_t imag = static_cast<mplapack_binary128_t>(-2.0L) +
                                       static_cast<mplapack_binary128_t>(0x1p-101L);
    const std::complex<mplapack_binary128_t> source(real, imag);
    const mpfrxx::mpc_class target(source);
    using native_type = gmpfrxx_mkII::adapters::binary128_real::value_type;
    const gmpfrxx_mkII::adapters::binary128_real expected_real(
        static_cast<native_type>(real));
    const gmpfrxx_mkII::adapters::binary128_real expected_imag(
        static_cast<native_type>(imag));
    require_components(target, expected_real, expected_imag);
    require_below_binary64_components(target, 1.0, -2.0);
#endif
}

// COMPLEX-GMP
void test_complex_gmp()
{
    gmpxx::mpf_class real;
    gmpxx::mpf_class imag;
    gmpxx::mpf_class low;
    mpf_set_d(real.mpf_data(), 1.0);
    mpf_set_d(imag.mpf_data(), -2.0);
    mpf_set_ui(low.mpf_data(), 1);
    mpf_div_2exp(low.mpf_data(), low.mpf_data(), 100);
    mpf_add(real.mpf_data(), real.mpf_data(), low.mpf_data());
    mpf_div_2exp(low.mpf_data(), low.mpf_data(), 1);
    mpf_add(imag.mpf_data(), imag.mpf_data(), low.mpf_data());
    const gmpxx::mpfc_class source(real, imag);
    const mpfrxx::mpc_class target(source);
    require_components(target, source.real(), source.imag());
    require_below_binary64_components(target, 1.0, -2.0);
}

} // namespace

int main()
{
    test_complex_double();
    test_complex_dd();
    test_complex_qd();
    test_complex_binary80();
    test_complex_binary128();
    test_complex_gmp();
    return 0;
}
