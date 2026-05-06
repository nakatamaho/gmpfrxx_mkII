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

#include <mpfrxx_mkII.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>

namespace {

constexpr double pi = 3.141592653589793238462643383279502884;

void require_close_1e15(const char* name, const mpfrxx::mpfr_class& got, double expected)
{
    const double actual = got.to_double();
    const double scale = std::max(1.0, std::abs(expected));
    const double tolerance = 1.0e-15 * scale;
    assert(std::isfinite(actual));
    assert(std::isfinite(expected));
    if (std::abs(actual - expected) > tolerance) {
        std::cerr << name << " mismatch: got " << actual
                  << " expected " << expected
                  << " tolerance " << tolerance << '\n';
        std::abort();
    }
}

void test_random_trigonometric_smoke()
{
    std::mt19937_64 rng(0x6d70667274726967ull);
    std::uniform_real_distribution<double> unit_distribution(-0.75, 0.75);
    std::uniform_real_distribution<double> angle_distribution(-0.25, 0.25);
    std::uniform_real_distribution<double> positive_distribution(0.125, 0.75);

    for (int i = 0; i < 96; ++i) {
        const double x = unit_distribution(rng);
        const double y = unit_distribution(rng);
        const double angle = angle_distribution(rng);
        const double positive = positive_distribution(rng);

        const mpfrxx::mpfr_class value(x, 256);
        const mpfrxx::mpfr_class other(y, 256);
        const mpfrxx::mpfr_class angle_value(angle, 256);
        const mpfrxx::mpfr_class positive_value(positive, 256);

        require_close_1e15("sin", mpfrxx::sin(value), std::sin(x));
        require_close_1e15("cos", mpfrxx::cos(value), std::cos(x));
        require_close_1e15("tan", mpfrxx::tan(value), std::tan(x));
        require_close_1e15("asin", mpfrxx::asin(value), std::asin(x));
        require_close_1e15("acos", mpfrxx::acos(value), std::acos(x));
        require_close_1e15("atan", mpfrxx::atan(value), std::atan(x));
        require_close_1e15("atan2", mpfrxx::atan2(value, other), std::atan2(x, y));

        require_close_1e15("sec", mpfrxx::sec(value), 1.0 / std::cos(x));
        require_close_1e15("csc", mpfrxx::csc(positive_value), 1.0 / std::sin(positive));
        require_close_1e15("cot", mpfrxx::cot(positive_value), 1.0 / std::tan(positive));

        require_close_1e15("sinu", mpfrxx::sinu(angle_value, 360ul), std::sin(angle * 2.0 * pi / 360.0));
        require_close_1e15("cosu", mpfrxx::cosu(angle_value, 360ul), std::cos(angle * 2.0 * pi / 360.0));
        require_close_1e15("tanu", mpfrxx::tanu(angle_value, 360ul), std::tan(angle * 2.0 * pi / 360.0));
        require_close_1e15("asinu", mpfrxx::asinu(value, 360ul), std::asin(x) * 360.0 / (2.0 * pi));
        require_close_1e15("acosu", mpfrxx::acosu(value, 360ul), std::acos(x) * 360.0 / (2.0 * pi));
        require_close_1e15("atanu", mpfrxx::atanu(value, 360ul), std::atan(x) * 360.0 / (2.0 * pi));
        require_close_1e15("atan2u", mpfrxx::atan2u(value, other, 360ul), std::atan2(x, y) * 360.0 / (2.0 * pi));

        require_close_1e15("sinpi", mpfrxx::sinpi(angle_value), std::sin(pi * angle));
        require_close_1e15("cospi", mpfrxx::cospi(angle_value), std::cos(pi * angle));
        require_close_1e15("tanpi", mpfrxx::tanpi(angle_value), std::tan(pi * angle));
        require_close_1e15("asinpi", mpfrxx::asinpi(value), std::asin(x) / pi);
        require_close_1e15("acospi", mpfrxx::acospi(value), std::acos(x) / pi);
        require_close_1e15("atanpi", mpfrxx::atanpi(value), std::atan(x) / pi);
        require_close_1e15("atan2pi", mpfrxx::atan2pi(value, other), std::atan2(x, y) / pi);
    }
}

void test_random_hyperbolic_smoke()
{
    std::mt19937_64 rng(0x6d706672687970ull);
    std::uniform_real_distribution<double> small_distribution(-0.6, 0.6);
    std::uniform_real_distribution<double> acosh_distribution(1.0, 2.5);
    std::uniform_real_distribution<double> positive_distribution(0.125, 0.75);

    for (int i = 0; i < 96; ++i) {
        const double x = small_distribution(rng);
        const double acosh_input = acosh_distribution(rng);
        const double positive = positive_distribution(rng);

        const mpfrxx::mpfr_class value(x, 256);
        const mpfrxx::mpfr_class acosh_value(acosh_input, 256);
        const mpfrxx::mpfr_class positive_value(positive, 256);

        require_close_1e15("sinh", mpfrxx::sinh(value), std::sinh(x));
        require_close_1e15("cosh", mpfrxx::cosh(value), std::cosh(x));
        require_close_1e15("tanh", mpfrxx::tanh(value), std::tanh(x));
        require_close_1e15("asinh", mpfrxx::asinh(value), std::asinh(x));
        require_close_1e15("acosh", mpfrxx::acosh(acosh_value), std::acosh(acosh_input));
        require_close_1e15("atanh", mpfrxx::atanh(value), std::atanh(x));
        require_close_1e15("sech", mpfrxx::sech(value), 1.0 / std::cosh(x));
        require_close_1e15("csch", mpfrxx::csch(positive_value), 1.0 / std::sinh(positive));
        require_close_1e15("coth", mpfrxx::coth(positive_value), 1.0 / std::tanh(positive));
        const auto sinh_cosh = mpfrxx::sinh_cosh(value);
        require_close_1e15("sinh_cosh sinh", sinh_cosh.first, std::sinh(x));
        require_close_1e15("sinh_cosh cosh", sinh_cosh.second, std::cosh(x));
    }
}

void test_random_log_exp_pow_smoke()
{
    std::mt19937_64 rng(0x6d7066726c6f67ull);
    std::uniform_real_distribution<double> small_distribution(-0.6, 0.6);
    std::uniform_real_distribution<double> positive_distribution(0.25, 2.5);
    std::uniform_real_distribution<double> base_distribution(0.35, 2.0);
    std::uniform_real_distribution<double> exponent_distribution(-1.25, 1.25);

    for (int i = 0; i < 96; ++i) {
        const double x = small_distribution(rng);
        const double positive = positive_distribution(rng);
        const double base = base_distribution(rng);
        const double exponent = exponent_distribution(rng);

        const mpfrxx::mpfr_class value(x, 256);
        const mpfrxx::mpfr_class positive_value(positive, 256);
        const mpfrxx::mpfr_class base_value(base, 256);
        const mpfrxx::mpfr_class exponent_value(exponent, 256);

        require_close_1e15("exp", mpfrxx::exp(value), std::exp(x));
        require_close_1e15("exp2", mpfrxx::exp2(value), std::exp2(x));
        require_close_1e15("exp10", mpfrxx::exp10(value), std::pow(10.0, x));
        require_close_1e15("expm1", mpfrxx::expm1(value), std::expm1(x));
        require_close_1e15("exp2m1", mpfrxx::exp2m1(value), std::exp2(x) - 1.0);
        require_close_1e15("exp10m1", mpfrxx::exp10m1(value), std::pow(10.0, x) - 1.0);

        require_close_1e15("log", mpfrxx::log(positive_value), std::log(positive));
        require_close_1e15("log2", mpfrxx::log2(positive_value), std::log2(positive));
        require_close_1e15("log10", mpfrxx::log10(positive_value), std::log10(positive));
        require_close_1e15("log1p", mpfrxx::log1p(value), std::log1p(x));
        require_close_1e15("log2p1", mpfrxx::log2p1(value), std::log2(1.0 + x));
        require_close_1e15("log10p1", mpfrxx::log10p1(value), std::log10(1.0 + x));

        require_close_1e15("pow", mpfrxx::pow(base_value, exponent_value), std::pow(base, exponent));
        require_close_1e15("pow_si", mpfrxx::pow_si(base_value, -3l), std::pow(base, -3.0));
        require_close_1e15("pow_ui", mpfrxx::pow_ui(base_value, 5ul), std::pow(base, 5.0));
        require_close_1e15("ui_pow", mpfrxx::ui_pow(3ul, exponent_value), std::pow(3.0, exponent));
        require_close_1e15("ui_pow_ui", mpfrxx::ui_pow_ui(3ul, 5ul, 256), std::pow(3.0, 5.0));
        require_close_1e15("cbrt", mpfrxx::cbrt(value), std::cbrt(x));
        require_close_1e15("hypot", mpfrxx::hypot(value, positive_value), std::hypot(x, positive));
    }
}

void test_random_special_smoke()
{
    std::mt19937_64 rng(0x6d706672737063ull);
    std::uniform_real_distribution<double> small_distribution(-1.25, 1.25);
    std::uniform_real_distribution<double> positive_distribution(0.35, 2.5);

    for (int i = 0; i < 96; ++i) {
        const double x = small_distribution(rng);
        const double positive = positive_distribution(rng);

        const mpfrxx::mpfr_class value(x, 256);
        const mpfrxx::mpfr_class positive_value(positive, 256);

        require_close_1e15("erf", mpfrxx::erf(value), std::erf(x));
        require_close_1e15("erfc", mpfrxx::erfc(value), std::erfc(x));
        require_close_1e15("gamma", mpfrxx::gamma(positive_value), std::tgamma(positive));
        require_close_1e15("reciprocal_gamma", mpfrxx::reciprocal_gamma(positive_value), 1.0 / std::tgamma(positive));
    }
}

} // namespace

int main()
{
    test_random_trigonometric_smoke();
    test_random_hyperbolic_smoke();
    test_random_log_exp_pow_smoke();
    test_random_special_smoke();
    return 0;
}
