#ifndef GMPFRXX_MKII_TEST_HAVE_QD
#define GMPFRXX_MKII_TEST_HAVE_QD 0
#endif

#include <gmpfrxx_mkII/adapters/binary128_complex.hpp>
#include <gmpfrxx_mkII/adapters/binary80_complex.hpp>
#include <gmpfrxx_mkII/adapters/mpfc_complex.hpp>
#if GMPFRXX_MKII_TEST_HAVE_QD
#include <gmpfrxx_mkII/adapters/dd_complex.hpp>
#include <gmpfrxx_mkII/adapters/qd_complex.hpp>
#endif

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
class test_complex {
public:
    test_complex(const Real& real, const Real& imag)
        : real_(real), imag_(imag)
    {
    }

    const Real& real() const { return real_; }
    const Real& imag() const { return imag_; }

private:
    Real real_;
    Real imag_;
};

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

#if GMPFRXX_MKII_TEST_HAVE_QD
// COMPLEX-DD
void test_complex_dd()
{
    const test_complex<dd_real> source(
        dd_real(1.0, std::ldexp(1.0, -100)),
        dd_real(-2.0, std::ldexp(1.0, -101)));
    const mpfrxx::mpc_class target(source);
    require_components(target, source.real(), source.imag());
    require_below_binary64_components(target, 1.0, -2.0);
}

// COMPLEX-QD
void test_complex_qd()
{
    const test_complex<qd_real> source(
        qd_real(3.0, std::ldexp(1.0, -100), 0.0, 0.0),
        qd_real(-4.0, std::ldexp(1.0, -101), 0.0, 0.0));
    mpfrxx::mpc_class target;
    target = source;
    require_components(target, source.real(), source.imag());
    require_below_binary64_components(target, 3.0, -4.0);
}

#endif

// COMPLEX-BINARY80
template <typename Native>
void test_complex_binary80_native()
{
    const Native real = static_cast<Native>(1.0L) +
                        static_cast<Native>(0x1p-60L);
    const Native imag = static_cast<Native>(-2.0L) +
                        static_cast<Native>(0x1p-61L);
    const auto source = gmpfrxx_mkII::adapters::make_binary80_complex_source(real, imag);
    const mpfrxx::mpc_class target(source);
    const auto expected_real = gmpfrxx_mkII::adapters::make_binary80_source(real);
    const auto expected_imag = gmpfrxx_mkII::adapters::make_binary80_source(imag);
    require_components(target, expected_real, expected_imag);
    require_below_binary64_components(target, 1.0, -2.0);
}

void test_complex_binary80()
{
#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY80
    test_complex_binary80_native<long double>();
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_FLOAT64X_BINARY80
    test_complex_binary80_native<_Float64x>();
#endif
}

// COMPLEX-BINARY128
template <typename Native>
void test_complex_binary128_native()
{
    const Native real = static_cast<Native>(1.0L) +
                        static_cast<Native>(0x1p-100L);
    const Native imag = static_cast<Native>(-2.0L) +
                        static_cast<Native>(0x1p-101L);
    const auto source = gmpfrxx_mkII::adapters::make_binary128_complex_source(real, imag);
    const mpfrxx::mpc_class target(source);
    const auto expected_real = gmpfrxx_mkII::adapters::make_binary128_source(real);
    const auto expected_imag = gmpfrxx_mkII::adapters::make_binary128_source(imag);
    require_components(target, expected_real, expected_imag);
    require_below_binary64_components(target, 1.0, -2.0);
}

void test_complex_binary128()
{
#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128
    test_complex_binary128_native<long double>();
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_ISO_FLOAT128
    test_complex_binary128_native<_Float128>();
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_GNU_FLOAT128
    test_complex_binary128_native<__float128>();
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
#if GMPFRXX_MKII_TEST_HAVE_QD
    test_complex_dd();
    test_complex_qd();
#endif
    test_complex_binary80();
    test_complex_binary128();
    test_complex_gmp();
    return 0;
}
