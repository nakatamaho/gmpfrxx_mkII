/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <gmpxx_mkII.h>

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
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

gmpxx::mpf_class parse_decimal_literal(const char* text, mp_bitcnt_t precision)
{
    return gmpxx::mpf_class(text, precision);
}

gmpxx::mpf_class two_to_minus(mp_bitcnt_t bits, mp_bitcnt_t precision)
{
    gmpxx::mpf_class result(1, precision);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), bits);
    return result;
}

gmpxx::mpf_class rounded_to(const gmpxx::mpf_class& value, mp_bitcnt_t precision)
{
    return gmpxx::mpf_math_detail::set_prec_copy(value, precision);
}

gmpxx::mpf_class ulp_for_value(const gmpxx::mpf_class& value, mp_bitcnt_t precision)
{
    mp_exp_t exponent = 0;
    mpf_get_d_2exp(&exponent, value.mpf_data());

    gmpxx::mpf_class ulp = gmpxx::mpf_math_detail::make_ui(1, precision + 8);
    if (exponent >= static_cast<mp_exp_t>(precision)) {
        mpf_mul_2exp(ulp.mpf_data(), ulp.mpf_data(),
                     static_cast<mp_bitcnt_t>(exponent - static_cast<mp_exp_t>(precision)));
    } else if (precision < 2) {
        mpf_mul_2exp(ulp.mpf_data(), ulp.mpf_data(), 2 - precision);
    } else {
        mpf_div_2exp(ulp.mpf_data(), ulp.mpf_data(),
                     static_cast<mp_bitcnt_t>(static_cast<mp_exp_t>(precision) - exponent));
    }
    return ulp;
}

void require_within_ulp(const gmpxx::mpf_class& got,
                        const gmpxx::mpf_class& expected,
                        mp_bitcnt_t precision,
                        unsigned long max_ulps)
{
    gmpxx::mpf_class diff = gmpxx::mpf_math_detail::abs_prec(
        gmpxx::mpf_math_detail::sub(got, expected, precision + 8), precision + 8);
    gmpxx::mpf_class bound = ulp_for_value(expected, precision);
    mpf_mul_ui(bound.mpf_data(), bound.mpf_data(), max_ulps);
    if (mpf_cmp(diff.mpf_data(), bound.mpf_data()) > 0) {
        std::cerr << "values differ by more than " << max_ulps << " ulps at precision "
                  << precision << '\n';
        std::abort();
    }
}

using UnaryMpfFunction = gmpxx::mpf_class (*)(const gmpxx::mpf_class&);
using BinaryMpfFunction = gmpxx::mpf_class (*)(const gmpxx::mpf_class&, const gmpxx::mpf_class&);

void require_precision_doubling(UnaryMpfFunction function,
                                const char* input,
                                mp_bitcnt_t low_precision,
                                mp_bitcnt_t high_precision,
                                unsigned long max_ulps)
{
    const gmpxx::mpf_class low_input = parse_decimal_literal(input, low_precision);
    const gmpxx::mpf_class high_input = parse_decimal_literal(input, high_precision);
    const gmpxx::mpf_class low_value = function(low_input);
    const gmpxx::mpf_class high_value = function(high_input);
    require_within_ulp(low_value, rounded_to(high_value, low_precision), low_precision, max_ulps);
}

void require_precision_doubling(BinaryMpfFunction function,
                                const char* lhs,
                                const char* rhs,
                                mp_bitcnt_t low_precision,
                                mp_bitcnt_t high_precision,
                                unsigned long max_ulps)
{
    const gmpxx::mpf_class low_lhs = parse_decimal_literal(lhs, low_precision);
    const gmpxx::mpf_class low_rhs = parse_decimal_literal(rhs, low_precision);
    const gmpxx::mpf_class high_lhs = parse_decimal_literal(lhs, high_precision);
    const gmpxx::mpf_class high_rhs = parse_decimal_literal(rhs, high_precision);
    const gmpxx::mpf_class low_value = function(low_lhs, low_rhs);
    const gmpxx::mpf_class high_value = function(high_lhs, high_rhs);
    require_within_ulp(low_value, rounded_to(high_value, low_precision), low_precision, max_ulps);
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
                       tolerance_bits);
    require_close_bits(gmpxx::gamma(gmpxx::mpf_class(3, precision)),
                       two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::gamma(gmpxx::mpf_class("0.5", precision)),
                       gmpxx::sqrt(pi),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::reciprocal_gamma(one),
                       one,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::reciprocal_gamma(gmpxx::mpf_class(0, precision)),
                       gmpxx::mpf_class(0, precision),
                       precision,
                       tolerance_bits);
}

void require_gamma_spouge_term_policy()
{
    int previous_terms = 0;
    for (mp_bitcnt_t precision : std::array<mp_bitcnt_t, 4>{{512, 1024, 2048, 4096}}) {
        const int terms = gmpxx::mpf_math_detail::gamma_spouge_terms(precision);
        if (terms <= previous_terms || terms <= 128) {
            std::cerr << "unexpected Spouge term count " << terms
                      << " at precision " << precision << '\n';
            std::abort();
        }
        previous_terms = terms;
    }
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

void require_reference_literal_cases()
{
    constexpr mp_bitcnt_t target_precision = 256;
    constexpr mp_bitcnt_t literal_precision = 768;

    struct UnaryReferenceCase {
        UnaryMpfFunction function;
        const char* input;
        const char* reference;
        unsigned long max_ulps;
    };

    const std::array<UnaryReferenceCase, 10> cases{{
        {static_cast<UnaryMpfFunction>(&gmpxx::log1p), "0.1",
         "0.09531017980432486004395212328076509222060536530864419918523980816300101423588423"
         "283905750291303649307274794184585174988884604369351298063868901502170232637556873",
         2},
        {static_cast<UnaryMpfFunction>(&gmpxx::log), "25",
         "3.21887582486820074920151866645237527905120270853703544382529578294835797541531552"
         "926026775618635922159993260604343112579944801045864935239926723323492741145510435",
         2},
        {static_cast<UnaryMpfFunction>(&gmpxx::log2), "7.99",
         "2.99819550315325208468423790606226744823209212142600192208976212521364807239226348"
         "9504996947139992619474166188136624561747",
         4},
        {static_cast<UnaryMpfFunction>(&gmpxx::log10), "5",
         "0.69897000433601880478626110527550697323181011853789145868957253887289181072557549"
         "0513072747881813827959315522808569004620",
         4},
        {static_cast<UnaryMpfFunction>(&gmpxx::exp), "1",
         "2.71828182845904523536028747135266249775724709369995957496696762772407663035354759"
         "457138217852516642742746639193200305992181741359662904357290033429526059563073814",
         2},
        {static_cast<UnaryMpfFunction>(&gmpxx::expm1), "0.1",
         "0.10517091807564762481170782649024666822454719473751871879286328944096796674765462"
         "180826680334383576123364162622389881639224377083652885920639130690370248999245655",
         2},
        {static_cast<UnaryMpfFunction>(&gmpxx::sin), "1",
         "0.84147098480789650665250232163029899962256306079837106567275170999191040439123966"
         "894863974354305269585434903790792067429325911892099189888119341032772921240948079",
         4},
        {static_cast<UnaryMpfFunction>(&gmpxx::cos), "1",
         "0.54030230586813971740093660744297660373231042061792222767009725538110039477447176"
         "401902652510870988844205719922104139221278325511597186845837433912379536774980850",
         4},
        {static_cast<UnaryMpfFunction>(&gmpxx::tan), "0.5",
         "0.54630248984379051325517946578028538329755172017979124616409138593290751051802581"
         "5715180648270656218589104862600264114264",
         8},
        {static_cast<UnaryMpfFunction>(&gmpxx::atan), "1",
         "0.78539816339744830961566084581987572104929234984377645524373614807695410157155224"
         "965700870633552926699553662053457075766177346115238764555793134795203212028936257",
         4},
    }};

    for (const auto& c : cases) {
        const gmpxx::mpf_class input = parse_decimal_literal(c.input, target_precision);
        const gmpxx::mpf_class reference_hi = parse_decimal_literal(c.reference, literal_precision);
        require_within_ulp(c.function(input), rounded_to(reference_hi, target_precision), target_precision, c.max_ulps);
    }

    const gmpxx::mpf_class atan2_reference_hi = parse_decimal_literal(
        "0.78539816339744830961566084581987572104929234984377645524373614807695410157155224"
        "965700870633552926699553662053457075766177346115238764555793134795203212028936257",
        literal_precision);
    require_within_ulp(gmpxx::atan2(gmpxx::mpf_class(1, target_precision),
                                    gmpxx::mpf_class(1, target_precision)),
                       rounded_to(atan2_reference_hi, target_precision),
                       target_precision,
                       4);

    const gmpxx::mpf_class pow_reference_hi = parse_decimal_literal(
        "1.41421356237309504880168872420969807856967187537694807317667973799073247846210703"
        "885038753432764157273501384623091229702492483605585073721264412149709993583141322",
        literal_precision);
    require_within_ulp(gmpxx::pow(gmpxx::mpf_class(2, target_precision),
                                  parse_decimal_literal("0.5", target_precision)),
                       rounded_to(pow_reference_hi, target_precision),
                       target_precision,
                       4);
}

void require_precision_doubling_cases()
{
    require_within_ulp(gmpxx::pi(128), rounded_to(gmpxx::pi(256), 128), 128, 2);
    require_within_ulp(gmpxx::pi(512), rounded_to(gmpxx::pi(1024), 512), 512, 2);
    require_within_ulp(gmpxx::log_two(128), rounded_to(gmpxx::log_two(256), 128), 128, 2);
    require_within_ulp(gmpxx::log_two(512), rounded_to(gmpxx::log_two(1024), 512), 512, 2);

    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log1p), "0.1", 128, 256, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log1p), "0.1", 512, 1024, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log), "25", 128, 256, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log), "25", 512, 1024, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log2), "7.99", 128, 256, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log2), "7.99", 512, 1024, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log10), "5", 128, 256, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::log10), "5", 512, 1024, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::exp), "1", 128, 256, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::exp), "1", 512, 1024, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::expm1), "0.1", 128, 256, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::expm1), "0.1", 512, 1024, 2);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::sin), "1", 128, 256, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::sin), "1", 512, 1024, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::cos), "1", 128, 256, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::cos), "1", 512, 1024, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::tan), "0.5", 128, 256, 8);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::tan), "0.5", 512, 1024, 8);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::atan), "1", 128, 256, 4);
    require_precision_doubling(static_cast<UnaryMpfFunction>(&gmpxx::atan), "1", 512, 1024, 4);
    require_precision_doubling(static_cast<BinaryMpfFunction>(&gmpxx::atan2), "1", "1", 128, 256, 4);
    require_precision_doubling(static_cast<BinaryMpfFunction>(&gmpxx::pow), "1.1", "4.4", 128, 256, 4);
    require_precision_doubling(static_cast<BinaryMpfFunction>(&gmpxx::pow), "1.1", "4.4", 512, 1024, 4);
}

void require_precision_policy_cases()
{
    const gmpxx::mpf_class low("0.25", static_cast<mp_bitcnt_t>(128));
    const gmpxx::mpf_class high("2.0", static_cast<mp_bitcnt_t>(320));

    if (gmpxx::exp(low).precision() != low.precision() ||
        gmpxx::log(high).precision() != high.precision() ||
        gmpxx::log2(high).precision() != high.precision() ||
        gmpxx::log10(high).precision() != high.precision() ||
        gmpxx::sin(low).precision() != low.precision() ||
        gmpxx::tan(low).precision() != low.precision() ||
        gmpxx::atan2(low, high).precision() != high.precision() ||
        gmpxx::pow(low, high).precision() != high.precision()) {
        std::abort();
    }
}

void require_atan2_axis_cases()
{
    constexpr mp_bitcnt_t precision = 256;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class minus_one(-1, precision);
    const gmpxx::mpf_class pi = gmpxx::pi(precision);
    gmpxx::mpf_class pi_over_two = gmpxx::mpf_math_detail::set_prec_copy(pi, precision);
    mpf_div_2exp(pi_over_two.mpf_data(), pi_over_two.mpf_data(), 1);

    require_within_ulp(gmpxx::atan2(zero, one), zero, precision, 1);
    require_within_ulp(gmpxx::atan2(zero, minus_one), pi, precision, 2);
    require_within_ulp(gmpxx::atan2(one, zero), pi_over_two, precision, 2);
    require_within_ulp(gmpxx::atan2(minus_one, zero),
                       gmpxx::mpf_math_detail::sub(zero, pi_over_two, precision),
                       precision,
                       2);
    require_within_ulp(gmpxx::atan2(zero, zero), zero, precision, 1);
}

void require_expression_overload_value_cases()
{
    constexpr mp_bitcnt_t low_precision = 160;
    constexpr mp_bitcnt_t high_precision = 224;
    const gmpxx::mpf_class a("0.25", low_precision);
    const gmpxx::mpf_class b("0.5", high_precision);
    const gmpxx::mpf_class c("1.25", high_precision);
    const gmpxx::mpf_class offset("0.125", high_precision);
    const auto positive_expr = a + b;
    const auto small_expr = a - offset;
    const auto atan_y = a + b;
    const auto atan_x = c - a;

    const gmpxx::mpf_class positive_value(positive_expr);
    const gmpxx::mpf_class small_value(small_expr);
    const gmpxx::mpf_class atan_y_value(atan_y);
    const gmpxx::mpf_class atan_x_value(atan_x);

    if (gmpxx::log(positive_expr).get_prec() != positive_value.get_prec()) {
        std::abort();
    }
    require_close_bits(gmpxx::log(positive_expr), gmpxx::log(positive_value), high_precision, 150);
    require_close_bits(gmpxx::log2(positive_expr), gmpxx::log2(positive_value), high_precision, 145);
    require_close_bits(gmpxx::log10(positive_expr), gmpxx::log10(positive_value), high_precision, 145);
    require_close_bits(gmpxx::log1p(small_expr), gmpxx::log1p(small_value), high_precision, 145);
    require_close_bits(gmpxx::exp(small_expr), gmpxx::exp(small_value), high_precision, 145);
    require_close_bits(gmpxx::expm1(small_expr), gmpxx::expm1(small_value), high_precision, 145);
    require_close_bits(gmpxx::sin(positive_expr), gmpxx::sin(positive_value), high_precision, 145);
    require_close_bits(gmpxx::cos(positive_expr), gmpxx::cos(positive_value), high_precision, 145);
    require_close_bits(gmpxx::tan(small_expr), gmpxx::tan(small_value), high_precision, 145);
    require_close_bits(gmpxx::atan(positive_expr), gmpxx::atan(positive_value), high_precision, 145);
    require_close_bits(gmpxx::atan2(atan_y, atan_x),
                       gmpxx::atan2(atan_y_value, atan_x_value),
                       high_precision,
                       145);
    require_close_bits(gmpxx::pow(positive_expr, small_expr),
                       gmpxx::pow(positive_value, small_value),
                       high_precision,
                       140);
    require_close_bits(gmpxx::pow(positive_expr, small_value),
                       gmpxx::pow(positive_value, small_value),
                       high_precision,
                       140);
    require_close_bits(gmpxx::pow(positive_value, small_expr),
                       gmpxx::pow(positive_value, small_value),
                       high_precision,
                       140);
}

void require_exact_special_value_cases()
{
    constexpr mp_bitcnt_t precision = 160;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class three(3, precision);

    require_zero(gmpxx::log(one));
    require_zero(gmpxx::log2(one));
    require_zero(gmpxx::log10(one));
    require_zero(gmpxx::log1p(zero));
    require_close_bits(gmpxx::exp(zero), one, precision, 120);
    require_zero(gmpxx::expm1(zero));
    require_zero(gmpxx::sin(zero));
    require_close_bits(gmpxx::cos(zero), one, precision, 120);
    require_zero(gmpxx::tan(zero));
    require_zero(gmpxx::atan(zero));
    require_zero(gmpxx::atan2(zero, one));
    require_close_bits(gmpxx::pow(two, three), gmpxx::mpf_class(8, precision), precision, 120);
    require_close_bits(gmpxx::pow(two, gmpxx::mpf_class(-3, precision)),
                       gmpxx::mpf_class("0.125", precision),
                       precision,
                       120);
}

void require_identity_cases()
{
    constexpr mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class x("0.125", precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);

    require_close_bits(gmpxx::exp(gmpxx::log(two)), two, precision, 120);
    require_close_bits(gmpxx::log(gmpxx::exp(x)), x, precision, 110);
    require_close_bits(gmpxx::log1p(x), gmpxx::log(one + x), precision, 110);
    require_close_bits(gmpxx::expm1(x), gmpxx::exp(x) - one, precision, 110);
    require_close_bits(gmpxx::log2(two), one, precision, 110);
    require_close_bits(gmpxx::log10(gmpxx::mpf_class(100, precision)),
                       two,
                       precision,
                       110);

    const gmpxx::mpf_class pi = gmpxx::pi(precision);
    gmpxx::mpf_class half_pi = pi;
    mpf_div_2exp(half_pi.mpf_data(), half_pi.mpf_data(), 1);
    require_close_bits(gmpxx::sin(half_pi), one, precision, 100);
    require_close_bits(gmpxx::cos(half_pi), gmpxx::mpf_class(0, precision), precision, 100);
    require_close_bits(gmpxx::tan(x) * gmpxx::cos(x), gmpxx::sin(x), precision, 120);
    require_close_bits(gmpxx::atan(one), half_pi / 2, precision, 100);
    require_close_bits(gmpxx::atan2(one, one), half_pi / 2, precision, 100);
}

void require_near_zero_and_near_one_cases()
{
    constexpr mp_bitcnt_t precision = 256;
    const gmpxx::mpf_class tiny = two_to_minus(120, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class tiny_squared = tiny * tiny;

    if (gmpxx::abs(gmpxx::log1p(tiny) - tiny) >= tiny_squared ||
        gmpxx::abs(gmpxx::expm1(tiny) - tiny) >= tiny_squared ||
        gmpxx::abs(gmpxx::exp(tiny) - (one + tiny)) >= tiny_squared) {
        std::abort();
    }

    const gmpxx::mpf_class near_one = one + two_to_minus(80, precision);
    require_within_ulp(gmpxx::log(near_one), gmpxx::log1p(two_to_minus(80, precision)), precision, 2);
}

void require_trig_reduction_cases()
{
    constexpr mp_bitcnt_t precision = 256;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class pi = gmpxx::pi(precision);
    gmpxx::mpf_class pi_over_two = pi;
    mpf_div_2exp(pi_over_two.mpf_data(), pi_over_two.mpf_data(), 1);

    const gmpxx::mpf_class x = pi_over_two + two_to_minus(80, precision);
    if (gmpxx::abs(gmpxx::cos(x) + two_to_minus(80, precision)) >= two_to_minus(150, precision)) {
        std::abort();
    }
    require_close_bits((gmpxx::sin(x) * gmpxx::sin(x)) + (gmpxx::cos(x) * gmpxx::cos(x)),
                       one,
                       precision,
                       120);
    require_within_ulp(gmpxx::atan2(zero, one), zero, precision, 1);
    require_within_ulp(gmpxx::atan2(zero, -one), pi, precision, 2);
    require_within_ulp(gmpxx::atan2(one, zero), pi_over_two, precision, 2);
    require_within_ulp(gmpxx::atan2(-one, zero), -pi_over_two, precision, 2);
    require_within_ulp(gmpxx::atan2(zero, zero), zero, precision, 1);
}

void require_pow_integer_and_domain_cases()
{
    constexpr mp_bitcnt_t precision = 192;
    require_close_bits(gmpxx::pow(gmpxx::mpf_class(5, precision), gmpxx::mpf_class(3, precision)),
                       gmpxx::mpf_class(125, precision),
                       precision,
                       120);
    require_close_bits(gmpxx::pow(gmpxx::mpf_class(-2, precision), gmpxx::mpf_class(3, precision)),
                       gmpxx::mpf_class(-8, precision),
                       precision,
                       120);

    bool threw = false;
    try {
        (void)gmpxx::pow(gmpxx::mpf_class(-2, precision), gmpxx::mpf_class("0.5", precision));
    } catch (const std::domain_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    threw = false;
    try {
        (void)gmpxx::pow(gmpxx::mpf_class(0, precision), gmpxx::mpf_class(-1, precision));
    } catch (const std::domain_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }
}

void require_exp_scaling_overflow_guard_cases()
{
    const mp_bitcnt_t precision = 128;
    const gmpxx::mpf_class one(1, precision);

    gmpxx::mpf_math_detail::ensure_exp_scaling_exponent_fits(one, 0);
    gmpxx::mpf_math_detail::ensure_exp_scaling_exponent_fits(one, std::numeric_limits<mp_exp_t>::max() - 1);
    if (gmpxx::mpf_math_detail::checked_log_scaling_exponent(16, 8) != 9) {
        std::abort();
    }

    bool threw = false;
    try {
        gmpxx::mpf_math_detail::ensure_exp_scaling_exponent_fits(one, std::numeric_limits<mp_exp_t>::max());
    } catch (const std::overflow_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    gmpxx::mpf_class quarter(1, precision);
    mpf_div_2exp(quarter.mpf_data(), quarter.mpf_data(), 2);
    threw = false;
    try {
        gmpxx::mpf_math_detail::ensure_exp_scaling_exponent_fits(quarter, std::numeric_limits<mp_exp_t>::min());
    } catch (const std::overflow_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    threw = false;
    try {
        (void)gmpxx::mpf_math_detail::checked_log_scaling_exponent(16, std::numeric_limits<mp_exp_t>::min());
    } catch (const std::overflow_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    gmpxx::mpf_class huge("1e100", precision);
    threw = false;
    try {
        (void)gmpxx::exp(huge);
    } catch (const std::overflow_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    mpf_t tiny_raw;
    mpf_init2(tiny_raw, precision);
    mpf_set_ui(tiny_raw, 1);
    mpf_div_2exp(tiny_raw, tiny_raw,
                 gmpxx::mpf_math_detail::checked_mp_exp_magnitude(std::numeric_limits<mp_exp_t>::min()));
    const gmpxx::mpf_class tiny_at_exponent_floor(tiny_raw);
    mpf_clear(tiny_raw);

    threw = false;
    try {
        (void)gmpxx::log(tiny_at_exponent_floor);
    } catch (const std::overflow_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    threw = false;
    try {
        (void)gmpxx::pow(gmpxx::mpf_class(2, precision),
                         gmpxx::mpf_class(std::numeric_limits<mp_exp_t>::max(), precision));
    } catch (const std::overflow_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
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
    static_assert(std::is_same_v<decltype(gmpxx::atan2(std::declval<decltype(expr)>(), std::declval<const gmpxx::mpf_class&>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atan2(std::declval<const gmpxx::mpf_class&>(), std::declval<decltype(expr)>())), gmpxx::mpf_class>);
    static_assert(std::is_same_v<decltype(gmpxx::atan2(std::declval<decltype(expr)>(), std::declval<decltype(expr)>())), gmpxx::mpf_class>);
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

    require_gamma_spouge_term_policy();
    for (mp_bitcnt_t high_precision : std::array<mp_bitcnt_t, 3>{{512, 1024, 2048}}) {
        require_high_precision_log_case(high_precision);
    }
    require_random_trig_smoke();
    require_random_log_exp_smoke();
    require_random_inverse_trig_hyperbolic_smoke();
    require_random_pow_gamma_smoke();
    require_reference_literal_cases();
    require_precision_doubling_cases();
    require_precision_policy_cases();
    require_atan2_axis_cases();
    require_expression_overload_value_cases();
    require_exact_special_value_cases();
    require_identity_cases();
    require_near_zero_and_near_one_cases();
    require_trig_reduction_cases();
    require_pow_integer_and_domain_cases();
    require_exp_scaling_overflow_guard_cases();

    return 0;
}
