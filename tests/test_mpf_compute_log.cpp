#include <gmpxx_mkII.h>

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace {

void require_prefix(const gmpxx::mpf_class& value,
                    int expected_exponent,
                    const char* expected_digits,
                    std::size_t digits)
{
    mp_exp_t exponent = 0;
    const std::string got = value.get_str(exponent, 10, digits);
    if (exponent != expected_exponent) {
        std::cerr << "unexpected exponent: got " << exponent
                  << " expected " << expected_exponent << '\n';
        std::abort();
    }
    if (got.size() < digits || got.substr(0, digits) != std::string(expected_digits).substr(0, digits)) {
        std::cerr << "unexpected digits: got " << got
                  << " expected prefix " << expected_digits << '\n';
        std::abort();
    }
}

void require_zero(const gmpxx::mpf_class& value)
{
    if (mpf_cmp_ui(value.mpf_data(), 0) != 0) {
        std::abort();
    }
}

void require_close_bits(const gmpxx::mpf_class& lhs,
                        const gmpxx::mpf_class& rhs,
                        mp_bitcnt_t precision,
                        mp_bitcnt_t tolerance_bits)
{
    gmpxx::mpf_class diff = gmpxx::mpf_math_detail::abs_prec(
        gmpxx::mpf_math_detail::sub(lhs, rhs, precision), precision);
    gmpxx::mpf_class threshold = gmpxx::mpf_math_detail::make_ui(1, precision);
    mpf_div_2exp(threshold.mpf_data(), threshold.mpf_data(), tolerance_bits);
    if (mpf_cmp(diff.mpf_data(), threshold.mpf_data()) >= 0) {
        std::cerr << "values differ by more than 2^-" << tolerance_bits << '\n';
        std::abort();
    }
}

void require_high_precision_log_case(mp_bitcnt_t precision)
{
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class four(4, precision);
    gmpxx::mpf_class half = gmpxx::mpf_class::with_precision(precision);
    mpf_set_ui(half.mpf_data(), 1);
    mpf_div_2exp(half.mpf_data(), half.mpf_data(), 1);

    const mp_bitcnt_t tolerance_bits = precision - 32;
    const mp_bitcnt_t gamma_tolerance_bits = precision > 160 ? 128 : precision - 32;
    const gmpxx::mpf_class log_two = gmpxx::mpf_math_detail::log_two(precision);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(two, precision),
                       log_two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(four, precision),
                       gmpxx::mpf_math_detail::mul_ui(log_two, 2, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(half, precision),
                       gmpxx::mpf_math_detail::sub(
                           gmpxx::mpf_math_detail::make_ui(0, precision), log_two, precision),
                       precision,
                       tolerance_bits);

    gmpxx::mpf_class tiny = gmpxx::mpf_math_detail::make_ui(1, precision);
    mpf_div_2exp(tiny.mpf_data(), tiny.mpf_data(), precision / 2);
    const gmpxx::mpf_class one_plus_tiny =
        gmpxx::mpf_math_detail::add(one, tiny, precision);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(one_plus_tiny, precision),
                       gmpxx::mpf_math_detail::compute_log1p(tiny, precision),
                       precision,
                       tolerance_bits);

    require_close_bits(gmpxx::log(two),
                       log_two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::log2(two),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::log2(four),
                       two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::log2(half),
                       gmpxx::mpf_math_detail::sub(
                           gmpxx::mpf_math_detail::make_ui(0, precision), one, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::log10(gmpxx::mpf_class(10, precision)),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::log1p(tiny),
                       gmpxx::mpf_math_detail::compute_log1p(tiny, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::exp(gmpxx::log(four)),
                       four,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::exp2(one),
                       two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::exp2(gmpxx::mpf_class(-1, precision)),
                       half,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::exp10(two),
                       gmpxx::mpf_class(100, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::expm1(gmpxx::log(two)),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::expm1(tiny),
                       gmpxx::mpf_math_detail::compute_expm1(tiny, precision),
                       precision,
                       tolerance_bits);

    const gmpxx::mpf_class pi = gmpxx::pi(precision);
    gmpxx::mpf_class pi_over_two = gmpxx::mpf_math_detail::set_prec_copy(pi, precision);
    mpf_div_2exp(pi_over_two.mpf_data(), pi_over_two.mpf_data(), 1);
    gmpxx::mpf_class pi_over_four = gmpxx::mpf_math_detail::set_prec_copy(pi, precision);
    mpf_div_2exp(pi_over_four.mpf_data(), pi_over_four.mpf_data(), 2);
    require_close_bits(gmpxx::sin(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::cos(gmpxx::mpf_class(0, precision)),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::sin(pi_over_two),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::cos(pi),
                       gmpxx::mpf_class(-1, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::tan(pi_over_four),
                       one,
                       precision,
                       tolerance_bits);
    const gmpxx::mpf_class angle("0.125", precision);
    const gmpxx::mpf_class sin_angle = gmpxx::sin(angle);
    const gmpxx::mpf_class cos_angle = gmpxx::cos(angle);
    require_close_bits(gmpxx::mpf_math_detail::add(
                           gmpxx::mpf_math_detail::sqr(sin_angle, precision),
                           gmpxx::mpf_math_detail::sqr(cos_angle, precision),
                           precision),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::tan(angle),
                       gmpxx::mpf_math_detail::div(sin_angle, cos_angle, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::asin(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::acos(one),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::atan(one),
                       pi_over_four,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::atan2(one, one),
                       pi_over_four,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::sinh(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::cosh(gmpxx::mpf_class(0, precision)),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::tanh(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::asinh(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::acosh(one),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::atanh(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::pow(two, gmpxx::mpf_class(10, precision)),
                       gmpxx::mpf_class(1024, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::pow(gmpxx::mpf_class(4, precision), gmpxx::mpf_class("0.5", precision)),
                       two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::pow(gmpxx::mpf_class(-2, precision), gmpxx::mpf_class(3, precision)),
                       gmpxx::mpf_class(-8, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::gamma(one),
                       one,
                       precision,
                       gamma_tolerance_bits);
    require_close_bits(gmpxx::gamma(gmpxx::mpf_class(3, precision)),
                       two,
                       precision,
                       gamma_tolerance_bits);
    require_close_bits(gmpxx::gamma(gmpxx::mpf_class("0.5", precision)),
                       gmpxx::sqrt(pi),
                       precision,
                       gamma_tolerance_bits);
    require_close_bits(gmpxx::reciprocal_gamma(one),
                       one,
                       precision,
                       gamma_tolerance_bits);
    require_close_bits(gmpxx::reciprocal_gamma(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
}

void require_close_double(const char* label, double got, double expected, double tolerance)
{
    if (std::abs(got - expected) > tolerance) {
        std::cerr << label << " mismatch: got " << got << " expected " << expected
                  << " tolerance " << tolerance << '\n';
        std::abort();
    }
}

void require_random_trig_smoke()
{
    constexpr mp_bitcnt_t precision = 256;
    constexpr double tolerance = 1.0e-15;
    const double pi = std::acos(-1.0);
    std::mt19937_64 rng(0x676d706672787875ull);
    std::uniform_real_distribution<double> distribution(-pi, pi);

    for (int i = 0; i < 32; ++i) {
        const double x = distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("sin", gmpxx::sin(value).to_double(), std::sin(x), tolerance);
        require_close_double("cos", gmpxx::cos(value).to_double(), std::cos(x), tolerance);
        if (std::abs(std::cos(x)) > 0.05) {
            require_close_double("tan", gmpxx::tan(value).to_double(), std::tan(x), 1.0e-14);
        }
    }
}

void require_random_log_exp_smoke()
{
    constexpr mp_bitcnt_t precision = 256;
    constexpr double tolerance = 1.0e-14;
    std::mt19937_64 rng(0x6c6f676578706631ull);
    std::uniform_real_distribution<double> positive_distribution(0.01, 10.0);
    std::uniform_real_distribution<double> exp_distribution(-5.0, 5.0);
    std::uniform_real_distribution<double> log1p_distribution(-0.75, 2.0);
    std::uniform_real_distribution<double> tiny_distribution(-1.0e-8, 1.0e-8);

    for (int i = 0; i < 32; ++i) {
        const double x = positive_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("log", gmpxx::log(value).to_double(), std::log(x), tolerance);
        require_close_double("log2", gmpxx::log2(value).to_double(), std::log2(x), tolerance);
        require_close_double("log10", gmpxx::log10(value).to_double(), std::log10(x), tolerance);
    }

    for (int i = 0; i < 32; ++i) {
        const double x = exp_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("exp", gmpxx::exp(value).to_double(), std::exp(x), 1.0e-12);
        require_close_double("exp2", gmpxx::exp2(value).to_double(), std::exp2(x), 1.0e-12);
        require_close_double("exp10", gmpxx::exp10(value).to_double(), std::pow(10.0, x), 1.0e-10);
        require_close_double("expm1", gmpxx::expm1(value).to_double(), std::expm1(x), 1.0e-12);
    }

    for (int i = 0; i < 32; ++i) {
        const double x = log1p_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("log1p", gmpxx::log1p(value).to_double(), std::log1p(x), tolerance);
    }

    for (int i = 0; i < 16; ++i) {
        const double x = tiny_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("log1p tiny", gmpxx::log1p(value).to_double(), std::log1p(x), 1.0e-22);
        require_close_double("expm1 tiny", gmpxx::expm1(value).to_double(), std::expm1(x), 1.0e-22);
    }
}

void require_random_inverse_trig_hyperbolic_smoke()
{
    constexpr mp_bitcnt_t precision = 256;
    std::mt19937_64 rng(0x6174616e68797031ull);
    std::uniform_real_distribution<double> unit_distribution(-0.9, 0.9);
    std::uniform_real_distribution<double> wide_distribution(-5.0, 5.0);
    std::uniform_real_distribution<double> positive_distribution(1.0, 5.0);
    std::uniform_real_distribution<double> hyperbolic_distribution(-3.0, 3.0);

    for (int i = 0; i < 32; ++i) {
        const double x = unit_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("asin", gmpxx::asin(value).to_double(), std::asin(x), 1.0e-15);
        require_close_double("acos", gmpxx::acos(value).to_double(), std::acos(x), 1.0e-15);
        require_close_double("atanh", gmpxx::atanh(value).to_double(), std::atanh(x), 1.0e-14);
    }

    for (int i = 0; i < 32; ++i) {
        const double x = wide_distribution(rng);
        const double y = wide_distribution(rng);
        const gmpxx::mpf_class x_value(x, precision);
        const gmpxx::mpf_class y_value(y, precision);
        require_close_double("atan", gmpxx::atan(x_value).to_double(), std::atan(x), 1.0e-15);
        if (std::abs(x) > 1.0e-9 || std::abs(y) > 1.0e-9) {
            require_close_double("atan2", gmpxx::atan2(y_value, x_value).to_double(), std::atan2(y, x), 1.0e-14);
        }
    }

    for (int i = 0; i < 32; ++i) {
        const double x = hyperbolic_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("sinh", gmpxx::sinh(value).to_double(), std::sinh(x), 1.0e-12);
        require_close_double("cosh", gmpxx::cosh(value).to_double(), std::cosh(x), 1.0e-12);
        require_close_double("tanh", gmpxx::tanh(value).to_double(), std::tanh(x), 1.0e-14);
        require_close_double("asinh", gmpxx::asinh(value).to_double(), std::asinh(x), 1.0e-14);
    }

    for (int i = 0; i < 32; ++i) {
        const double x = positive_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        require_close_double("acosh", gmpxx::acosh(value).to_double(), std::acosh(x), 1.0e-14);
    }
}

void require_random_pow_gamma_smoke()
{
    constexpr mp_bitcnt_t precision = 256;
    std::mt19937_64 rng(0x706f7767616d6d31ull);
    std::uniform_real_distribution<double> base_distribution(0.1, 5.0);
    std::uniform_real_distribution<double> exponent_distribution(-3.0, 3.0);
    std::uniform_real_distribution<double> gamma_distribution(0.5, 6.0);

    for (int i = 0; i < 32; ++i) {
        const double base = base_distribution(rng);
        const double exponent = exponent_distribution(rng);
        const gmpxx::mpf_class base_value(base, precision);
        const gmpxx::mpf_class exponent_value(exponent, precision);
        require_close_double("pow", gmpxx::pow(base_value, exponent_value).to_double(), std::pow(base, exponent), 1.0e-11);
    }

    for (int i = 0; i < 24; ++i) {
        const double x = gamma_distribution(rng);
        const gmpxx::mpf_class value(x, precision);
        const double gamma_reference = std::tgamma(x);
        require_close_double("gamma", gmpxx::gamma(value).to_double(), gamma_reference, 1.0e-9);
        require_close_double("reciprocal_gamma", gmpxx::reciprocal_gamma(value).to_double(), 1.0 / gamma_reference, 1.0e-12);
    }
}

} // namespace

int main()
{
    constexpr mp_bitcnt_t precision = 192;

    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class ten(10, precision);
    const gmpxx::mpf_class small("0.000000000000000000000001", precision);
    auto expr = one + small;

    static_assert(std::is_same_v<decltype(gmpxx::log(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log2(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log10(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log1p(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::exp(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::exp2(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::exp10(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::expm1(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::sin(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::cos(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::tan(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::asin(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::acos(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atan(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atan2(std::declval<const gmpxx::mpf_class&>(), std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::sinh(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::cosh(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::tanh(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::asinh(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::acosh(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atanh(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::pow(std::declval<const gmpxx::mpf_class&>(), std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::gamma(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::reciprocal_gamma(std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log2(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log10(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::log1p(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::exp(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::exp2(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::exp10(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::expm1(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::sin(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::cos(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::tan(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::asin(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::acos(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atan(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::sinh(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::cosh(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::tanh(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::asinh(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::acosh(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atanh(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::pow(std::declval<decltype(expr)>(), std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::pow(std::declval<const gmpxx::mpf_class&>(), std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::pow(std::declval<decltype(expr)>(), std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::gamma(std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::reciprocal_gamma(std::declval<decltype(expr)>())), gmpxx::mpf_class>);

    require_zero(gmpxx::mpf_math_detail::compute_log(one, precision));
    require_zero(gmpxx::log(one));
    require_zero(gmpxx::log2(one));
    require_zero(gmpxx::log1p(gmpxx::mpf_class(0, precision)));
    require_zero(gmpxx::expm1(gmpxx::mpf_class(0, precision)));
    require_prefix(gmpxx::mpf_math_detail::compute_log(two, precision),
                   0,
                   "693147180559945309417232121458176568075500134360255254120680",
                   44);
    require_prefix(gmpxx::log(two),
                   0,
                   "693147180559945309417232121458176568075500134360255254120680",
                   44);
    require_prefix(gmpxx::log(expr),
                   -23,
                   "100000000000000000000000",
                   1);
    require_prefix(gmpxx::mpf_math_detail::compute_log(ten, precision),
                   1,
                   "230258509299404568401799145468436420760110148862877297603333",
                   40);
    require_prefix(gmpxx::log2(ten),
                   1,
                   "332192809488736234787032",
                   24);
    require_zero(gmpxx::log10(one));
    require_close_bits(gmpxx::log10(ten), one, precision, precision - 32);
    require_close_bits(gmpxx::exp(gmpxx::mpf_class(0, precision)), one, precision, precision - 32);
    require_close_bits(gmpxx::exp2(one), two, precision, precision - 32);
    require_close_bits(gmpxx::exp10(gmpxx::mpf_class(2, precision)),
                       gmpxx::mpf_class(100, precision),
                       precision,
                       precision - 32);
    require_prefix(gmpxx::mpf_math_detail::compute_log1p(small, precision),
                   -23,
                   "100000000000000000000000",
                   1);
    require_prefix(gmpxx::log1p(small),
                   -23,
                   "100000000000000000000000",
                   1);

    bool saw_zero_domain_error = false;
    try {
        (void)gmpxx::mpf_math_detail::compute_log(zero, precision);
    } catch (const std::domain_error&) {
        saw_zero_domain_error = true;
    }
    if (!saw_zero_domain_error) {
        std::abort();
    }

    saw_zero_domain_error = false;
    try {
        (void)gmpxx::log2(zero);
    } catch (const std::domain_error&) {
        saw_zero_domain_error = true;
    }
    if (!saw_zero_domain_error) {
        std::abort();
    }

    bool saw_negative_domain_error = false;
    try {
        (void)gmpxx::mpf_math_detail::compute_log(gmpxx::mpf_class(-1, precision), precision);
    } catch (const std::domain_error&) {
        saw_negative_domain_error = true;
    }
    if (!saw_negative_domain_error) {
        std::abort();
    }

    saw_negative_domain_error = false;
    try {
        (void)gmpxx::log(gmpxx::mpf_class(-1, precision));
    } catch (const std::domain_error&) {
        saw_negative_domain_error = true;
    }
    if (!saw_negative_domain_error) {
        std::abort();
    }

    bool saw_log1p_pole = false;
    try {
        (void)gmpxx::mpf_math_detail::compute_log1p(gmpxx::mpf_class(-1, precision), precision);
    } catch (const std::domain_error&) {
        saw_log1p_pole = true;
    }
    if (!saw_log1p_pole) {
        std::abort();
    }

    saw_log1p_pole = false;
    try {
        (void)gmpxx::log1p(gmpxx::mpf_class(-1, precision));
    } catch (const std::domain_error&) {
        saw_log1p_pole = true;
    }
    if (!saw_log1p_pole) {
        std::abort();
    }

    bool saw_inverse_domain_error = false;
    try {
        (void)gmpxx::asin(gmpxx::mpf_class("1.1", precision));
    } catch (const std::domain_error&) {
        saw_inverse_domain_error = true;
    }
    if (!saw_inverse_domain_error) {
        std::abort();
    }

    saw_inverse_domain_error = false;
    try {
        (void)gmpxx::acos(gmpxx::mpf_class("-1.1", precision));
    } catch (const std::domain_error&) {
        saw_inverse_domain_error = true;
    }
    if (!saw_inverse_domain_error) {
        std::abort();
    }

    saw_inverse_domain_error = false;
    try {
        (void)gmpxx::acosh(gmpxx::mpf_class("0.9", precision));
    } catch (const std::domain_error&) {
        saw_inverse_domain_error = true;
    }
    if (!saw_inverse_domain_error) {
        std::abort();
    }

    saw_inverse_domain_error = false;
    try {
        (void)gmpxx::atanh(gmpxx::mpf_class(1, precision));
    } catch (const std::domain_error&) {
        saw_inverse_domain_error = true;
    }
    if (!saw_inverse_domain_error) {
        std::abort();
    }

    bool saw_pow_domain_error = false;
    try {
        (void)gmpxx::pow(gmpxx::mpf_class(0, precision), gmpxx::mpf_class(0, precision));
    } catch (const std::domain_error&) {
        saw_pow_domain_error = true;
    }
    if (!saw_pow_domain_error) {
        std::abort();
    }

    saw_pow_domain_error = false;
    try {
        (void)gmpxx::pow(gmpxx::mpf_class(-2, precision), gmpxx::mpf_class("0.5", precision));
    } catch (const std::domain_error&) {
        saw_pow_domain_error = true;
    }
    if (!saw_pow_domain_error) {
        std::abort();
    }

    bool saw_gamma_domain_error = false;
    try {
        (void)gmpxx::gamma(gmpxx::mpf_class(0, precision));
    } catch (const std::domain_error&) {
        saw_gamma_domain_error = true;
    }
    if (!saw_gamma_domain_error) {
        std::abort();
    }

    for (mp_bitcnt_t high_precision : std::array<mp_bitcnt_t, 3>{{512, 1024, 2048}}) {
        require_high_precision_log_case(high_precision);
    }
    require_random_trig_smoke();
    require_random_log_exp_smoke();
    require_random_inverse_trig_hyperbolic_smoke();
    require_random_pow_gamma_smoke();

    return 0;
}
