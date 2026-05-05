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

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

namespace gmpxx_defaults {

inline mp_bitcnt_t get_default_prec()
{
    return gmpxx::default_mpf_precision_bits();
}

} // namespace gmpxx_defaults

namespace gmpxx_detail {

inline mp_bitcnt_t effective_mpf_prec(mp_bitcnt_t precision)
{
    return gmpxx::mpf_class::with_precision(precision).get_prec();
}

} // namespace gmpxx_detail

gmpxx::mpf_class two_to_minus(mp_bitcnt_t bits, mp_bitcnt_t precision)
{
    gmpxx::mpf_class result(1, precision);
    mpf_div_2exp(result.get_mpf_t(), result.get_mpf_t(), bits);
    return result;
}

void assert_close(const gmpxx::mpf_class& got,
                  const gmpxx::mpf_class& expected,
                  mp_bitcnt_t tolerance_bits)
{
    const mp_bitcnt_t precision = std::max(got.get_prec(), expected.get_prec());
    gmpxx::mpf_class diff(0, precision);
    mpf_sub(diff.get_mpf_t(), got.get_mpf_t(), expected.get_mpf_t());
    mpf_abs(diff.get_mpf_t(), diff.get_mpf_t());
    assert(diff < two_to_minus(tolerance_bits, precision));
}

template <typename Function>
void expect_domain_error(Function&& function)
{
    bool threw = false;
    try {
        function();
    } catch (const std::domain_error&) {
        threw = true;
    }
    assert(threw);
}

void test_compile_time_surface()
{
    using gmpxx::mpf_class;
    using expr_type = decltype(std::declval<const mpf_class&>() +
                               std::declval<const mpf_class&>());

    static_assert(std::is_same<decltype(gmpxx::e(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::const_e()), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::const_e(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::inv_log_two()), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::inv_log_two(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::log_ten(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::const_log10()), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::const_log10(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::pi_over_two()), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::pi_over_two(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::pi_over_four()), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::pi_over_four(std::declval<mp_bitcnt_t>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::two_pi()), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::two_pi(std::declval<mp_bitcnt_t>())), mpf_class>::value);

    static_assert(std::is_same<decltype(gmpxx::asin(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::acos(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::sinh(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::cosh(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::tanh(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::asinh(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::acosh(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::atanh(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::exp2(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::exp10(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::gamma(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::reciprocal_gamma(std::declval<const mpf_class&>())), mpf_class>::value);

    static_assert(std::is_same<decltype(gmpxx::asin(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::acos(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::sinh(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::cosh(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::tanh(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::asinh(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::acosh(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::atanh(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::exp2(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::exp10(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::gamma(std::declval<expr_type>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::reciprocal_gamma(std::declval<expr_type>())), mpf_class>::value);
}

void test_constants()
{
    const mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class four(4, precision);

    assert(gmpxx::e(precision).get_prec() == gmpxx_detail::effective_mpf_prec(precision));
    assert_close(gmpxx::e(precision), gmpxx::exp(one), 120);
    assert_close(gmpxx::const_e(precision), gmpxx::e(precision), 160);
    assert(gmpxx::const_e().get_prec() == gmpxx_defaults::get_default_prec());

    assert_close(gmpxx::log_ten(precision), gmpxx::log(gmpxx::mpf_class(10, precision)), 120);
    assert_close(gmpxx::const_log10(precision), gmpxx::log_ten(precision), 160);
    assert(gmpxx::const_log10().get_prec() == gmpxx_defaults::get_default_prec());

    assert_close(gmpxx::inv_log_two(precision) * gmpxx::log_two(precision), one, 120);
    assert(gmpxx::inv_log_two().get_prec() == gmpxx_defaults::get_default_prec());

    assert_close(gmpxx::pi_over_two(precision) * two, gmpxx::pi(precision), 120);
    assert_close(gmpxx::pi_over_four(precision) * four, gmpxx::pi(precision), 120);
    assert_close(gmpxx::two_pi(precision), gmpxx::pi(precision) * two, 120);
    assert(gmpxx::pi_over_two().get_prec() == gmpxx_defaults::get_default_prec());
    assert(gmpxx::pi_over_four().get_prec() == gmpxx_defaults::get_default_prec());
    assert(gmpxx::two_pi().get_prec() == gmpxx_defaults::get_default_prec());
}

void test_inverse_trig()
{
    const mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class half("0.5", precision);

    assert(gmpxx::asin(zero) == zero);
    assert(gmpxx::acos(one) == zero);
    assert_close(gmpxx::asin(half) + gmpxx::acos(half), gmpxx::pi_over_two(precision), 110);
    assert_close(gmpxx::sin(gmpxx::asin(half)), half, 110);
    assert_close(gmpxx::cos(gmpxx::acos(half)), half, 110);

    expect_domain_error([&] { (void)gmpxx::asin(gmpxx::mpf_class("1.25", precision)); });
    expect_domain_error([&] { (void)gmpxx::acos(gmpxx::mpf_class("-1.25", precision)); });
}

void test_hyperbolic()
{
    const mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class x("0.75", precision);

    assert(gmpxx::sinh(zero) == zero);
    assert(gmpxx::cosh(zero) == one);
    assert(gmpxx::tanh(zero) == zero);
    assert_close((gmpxx::cosh(x) * gmpxx::cosh(x)) - (gmpxx::sinh(x) * gmpxx::sinh(x)), one, 105);
    assert_close(gmpxx::tanh(x), gmpxx::sinh(x) / gmpxx::cosh(x), 120);
}

void test_inverse_hyperbolic()
{
    const mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class x("0.25", precision);
    const gmpxx::mpf_class y("1.75", precision);

    assert(gmpxx::asinh(zero) == zero);
    assert(gmpxx::acosh(gmpxx::mpf_class(1, precision)) == zero);
    assert(gmpxx::atanh(zero) == zero);
    assert_close(gmpxx::sinh(gmpxx::asinh(x)), x, 105);
    assert_close(gmpxx::cosh(gmpxx::acosh(y)), y, 105);
    assert_close(gmpxx::tanh(gmpxx::atanh(x)), x, 105);

    expect_domain_error([&] { (void)gmpxx::acosh(gmpxx::mpf_class("0.75", precision)); });
    expect_domain_error([&] { (void)gmpxx::atanh(gmpxx::mpf_class(1, precision)); });
    expect_domain_error([&] { (void)gmpxx::atanh(gmpxx::mpf_class(-1, precision)); });
}

void test_exp_base_variants()
{
    const mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class three(3, precision);
    const gmpxx::mpf_class neg_two = -two;

    assert(gmpxx::exp2(zero) == one);
    assert_close(gmpxx::exp2(three), gmpxx::mpf_class(8, precision), 120);
    assert_close(gmpxx::exp2(neg_two), gmpxx::mpf_class("0.25", precision), 120);
    assert(gmpxx::exp10(zero) == one);
    assert_close(gmpxx::exp10(two), gmpxx::mpf_class(100, precision), 120);
    assert_close(gmpxx::log2(gmpxx::exp2(one)), one, 120);
}

void test_gamma_functions()
{
    const mp_bitcnt_t precision = 192;
    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class half("0.5", precision);
    const gmpxx::mpf_class five(5, precision);
    const gmpxx::mpf_class twenty_four(24, precision);
    const gmpxx::mpf_class x("1.25", precision);

    assert_close(gmpxx::gamma(one), one, 110);
    assert_close(gmpxx::gamma(five), twenty_four, 100);
    assert_close(gmpxx::gamma(half), gmpxx::sqrt(gmpxx::pi(precision)), 100);
    assert_close(gmpxx::gamma(x + one), x * gmpxx::gamma(x), 100);
    assert_close(gmpxx::reciprocal_gamma(x) * gmpxx::gamma(x), one, 110);
    assert(gmpxx::reciprocal_gamma(zero) == zero);
    expect_domain_error([&] { (void)gmpxx::gamma(zero); });
    expect_domain_error([&] { (void)gmpxx::gamma(gmpxx::mpf_class(-2, precision)); });
}

void test_expression_overloads()
{
    const mp_bitcnt_t low_precision = 160;
    const mp_bitcnt_t high_precision = 224;
    const gmpxx::mpf_class a("0.25", low_precision);
    const gmpxx::mpf_class b("0.5", high_precision);
    const gmpxx::mpf_class c("1.25", high_precision);
    const gmpxx::mpf_class offset("0.125", high_precision);

    auto small_expr = a + offset;
    auto acosh_expr = c + b;
    const gmpxx::mpf_class small_value(small_expr);
    const gmpxx::mpf_class acosh_value(acosh_expr);

    assert(gmpxx::asin(small_expr).get_prec() == small_value.get_prec());
    assert_close(gmpxx::asin(small_expr), gmpxx::asin(small_value), 135);
    assert_close(gmpxx::acos(small_expr), gmpxx::acos(small_value), 135);
    assert_close(gmpxx::sinh(small_expr), gmpxx::sinh(small_value), 135);
    assert_close(gmpxx::cosh(small_expr), gmpxx::cosh(small_value), 135);
    assert_close(gmpxx::tanh(small_expr), gmpxx::tanh(small_value), 135);
    assert_close(gmpxx::asinh(small_expr), gmpxx::asinh(small_value), 135);
    assert_close(gmpxx::acosh(acosh_expr), gmpxx::acosh(acosh_value), 135);
    assert_close(gmpxx::atanh(small_expr), gmpxx::atanh(small_value), 135);
    assert_close(gmpxx::exp2(small_expr), gmpxx::exp2(small_value), 135);
    assert_close(gmpxx::exp10(small_expr), gmpxx::exp10(small_value), 135);
    assert_close(gmpxx::gamma(acosh_expr), gmpxx::gamma(acosh_value), 110);
    assert_close(gmpxx::reciprocal_gamma(acosh_expr), gmpxx::reciprocal_gamma(acosh_value), 110);
}

void test_precision_policy()
{
    const mp_bitcnt_t precision = 224;
    const gmpxx::mpf_class x("0.25", precision);
    const gmpxx::mpf_class y("1.25", precision);

    assert(gmpxx::asin(x).get_prec() == x.get_prec());
    assert(gmpxx::acos(x).get_prec() == x.get_prec());
    assert(gmpxx::sinh(x).get_prec() == x.get_prec());
    assert(gmpxx::cosh(x).get_prec() == x.get_prec());
    assert(gmpxx::tanh(x).get_prec() == x.get_prec());
    assert(gmpxx::asinh(x).get_prec() == x.get_prec());
    assert(gmpxx::acosh(y).get_prec() == y.get_prec());
    assert(gmpxx::atanh(x).get_prec() == x.get_prec());
    assert(gmpxx::exp2(x).get_prec() == x.get_prec());
    assert(gmpxx::exp10(x).get_prec() == x.get_prec());
    assert(gmpxx::gamma(y).get_prec() == y.get_prec());
    assert(gmpxx::reciprocal_gamma(y).get_prec() == y.get_prec());
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_constants();
    test_inverse_trig();
    test_hyperbolic();
    test_inverse_hyperbolic();
    test_exp_base_variants();
    test_gamma_functions();
    test_expression_overloads();
    test_precision_policy();
    return 0;
}
