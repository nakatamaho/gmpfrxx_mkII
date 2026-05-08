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
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

void assert_mpf_equal(const gmpxx::mpf_class& got,
                      const gmpxx::mpf_class& expected)
{
    assert(mpf_cmp(got.get_mpf_t(), expected.get_mpf_t()) == 0);
}

void assert_mpf_close(const gmpxx::mpf_class& got,
                      const gmpxx::mpf_class& expected,
                      mp_bitcnt_t tolerance_bits)
{
    const mp_bitcnt_t precision = std::max(got.get_prec(), expected.get_prec());
    gmpxx::mpf_class diff(0, precision);
    mpf_sub(diff.get_mpf_t(), got.get_mpf_t(), expected.get_mpf_t());
    mpf_abs(diff.get_mpf_t(), diff.get_mpf_t());

    gmpxx::mpf_class tolerance(1, precision);
    mpf_div_2exp(tolerance.get_mpf_t(), tolerance.get_mpf_t(), tolerance_bits);
    assert(diff < tolerance);
}

void test_compile_time_surface()
{
    using gmpxx::mpf_class;
    using gmpxx::mpq_class;
    using gmpxx::mpz_class;

    static_assert(std::is_same<decltype(gmpxx::sqrt(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::abs(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::neg(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::ceil(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::floor(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::trunc(std::declval<const mpf_class&>())), mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::mpf_remainder(std::declval<const mpf_class&>(),
                                                            std::declval<const mpf_class&>())),
                               mpf_class>::value);
    static_assert(std::is_same<decltype(std::declval<mpf_class&>().set_epsilon()), void>::value);
    static_assert(std::is_same<decltype(gmpxx::hypot(std::declval<const mpf_class&>(),
                                                     std::declval<const mpf_class&>())),
                               mpf_class>::value);
    static_assert(std::is_same<decltype(gmpxx::sgn(std::declval<const mpf_class&>())), int>::value);
    static_assert(std::is_same<decltype(gmpxx::abs(std::declval<const mpz_class&>())), mpz_class>::value);
    static_assert(std::is_same<decltype(gmpxx::abs(std::declval<const mpq_class&>())), mpq_class>::value);
    static_assert(std::is_same<decltype(gmpxx::sqrt(std::declval<const mpz_class&>())), mpz_class>::value);
    static_assert(std::is_same<decltype(gmpxx::gcd(std::declval<const mpz_class&>(),
                                                   std::declval<const mpz_class&>())),
                               mpz_class>::value);
    static_assert(std::is_same<decltype(gmpxx::lcm(std::declval<const mpz_class&>(),
                                                   std::declval<const mpz_class&>())),
                               mpz_class>::value);
}

void test_sqrt()
{
    gmpxx::mpf_class four("4.0", static_cast<mp_bitcnt_t>(256));
    gmpxx::mpf_class result = gmpxx::sqrt(four);

    gmpxx::mpf_class expected = gmpxx::mpf_class::with_precision(four.get_prec());
    mpf_sqrt(expected.get_mpf_t(), four.get_mpf_t());

    assert(result.get_prec() == four.get_prec());
    assert_mpf_equal(result, expected);

    gmpxx::mpf_class zero("0.0", static_cast<mp_bitcnt_t>(192));
    gmpxx::mpf_class zero_result = gmpxx::sqrt(zero);

    gmpxx::mpf_class zero_expected = gmpxx::mpf_class::with_precision(zero.get_prec());
    mpf_sqrt(zero_expected.get_mpf_t(), zero.get_mpf_t());

    assert(zero_result.get_prec() == zero.get_prec());
    assert_mpf_equal(zero_result, zero_expected);
}

void test_abs()
{
    gmpxx::mpf_class negative("-3.5", static_cast<mp_bitcnt_t>(320));
    gmpxx::mpf_class result = gmpxx::abs(negative);

    gmpxx::mpf_class expected = gmpxx::mpf_class::with_precision(negative.get_prec());
    mpf_abs(expected.get_mpf_t(), negative.get_mpf_t());

    assert(result.get_prec() == negative.get_prec());
    assert_mpf_equal(result, expected);

    gmpxx::mpf_class positive("2.25", static_cast<mp_bitcnt_t>(128));
    gmpxx::mpf_class positive_result = gmpxx::abs(positive);

    gmpxx::mpf_class positive_expected = gmpxx::mpf_class::with_precision(positive.get_prec());
    mpf_abs(positive_expected.get_mpf_t(), positive.get_mpf_t());

    assert(positive_result.get_prec() == positive.get_prec());
    assert_mpf_equal(positive_result, positive_expected);
}

void test_neg()
{
    gmpxx::mpf_class negative("-3.5", static_cast<mp_bitcnt_t>(384));
    gmpxx::mpf_class result = gmpxx::neg(negative);

    gmpxx::mpf_class expected = gmpxx::mpf_class::with_precision(negative.get_prec());
    mpf_neg(expected.get_mpf_t(), negative.get_mpf_t());

    assert(result.get_prec() == negative.get_prec());
    assert_mpf_equal(result, expected);

    gmpxx::mpf_class positive("2.25", static_cast<mp_bitcnt_t>(160));
    gmpxx::mpf_class positive_result = gmpxx::neg(positive);

    gmpxx::mpf_class positive_expected = gmpxx::mpf_class::with_precision(positive.get_prec());
    mpf_neg(positive_expected.get_mpf_t(), positive.get_mpf_t());

    assert(positive_result.get_prec() == positive.get_prec());
    assert_mpf_equal(positive_result, positive_expected);
}

void test_rounding_functions()
{
    gmpxx::mpf_class positive("123.456", static_cast<mp_bitcnt_t>(256));
    gmpxx::mpf_class negative("-123.456", static_cast<mp_bitcnt_t>(256));

    assert_mpf_equal(gmpxx::ceil(positive), gmpxx::mpf_class("124", positive.get_prec()));
    assert_mpf_equal(gmpxx::ceil(negative), gmpxx::mpf_class("-123", negative.get_prec()));
    assert_mpf_equal(gmpxx::floor(positive), gmpxx::mpf_class("123", positive.get_prec()));
    assert_mpf_equal(gmpxx::floor(negative), gmpxx::mpf_class("-124", negative.get_prec()));
    assert_mpf_equal(gmpxx::trunc(positive), gmpxx::mpf_class("123", positive.get_prec()));
    assert_mpf_equal(gmpxx::trunc(negative), gmpxx::mpf_class("-123", negative.get_prec()));

    assert(gmpxx::ceil(positive).get_prec() == positive.get_prec());
    assert(gmpxx::floor(negative).get_prec() == negative.get_prec());
    assert(gmpxx::trunc(negative).get_prec() == negative.get_prec());

    const char* values[] = {"1.9", "4.3", "-7.1", "-2.8", "-1.5", "2.5"};
    for (const char* value : values) {
        gmpxx::mpf_class x(value, static_cast<mp_bitcnt_t>(256));
        assert_mpf_equal(gmpxx::trunc(x), gmpxx::mpf_class(std::trunc(x.get_d()), x.get_prec()));
        assert_mpf_equal(gmpxx::floor(x), gmpxx::mpf_class(std::floor(x.get_d()), x.get_prec()));
        assert_mpf_equal(gmpxx::ceil(x), gmpxx::mpf_class(std::ceil(x.get_d()), x.get_prec()));
    }
}

void test_hypot_and_scaling()
{
    gmpxx::mpf_class x("3.0", static_cast<mp_bitcnt_t>(192));
    gmpxx::mpf_class y("4.0", static_cast<mp_bitcnt_t>(384));
    gmpxx::mpf_class result = gmpxx::hypot(x, y);

    assert(result.get_prec() == y.get_prec());
    assert_mpf_equal(result, gmpxx::mpf_class("5.0", result.get_prec()));

    gmpxx::mpf_class value("2.0", static_cast<mp_bitcnt_t>(256));
    value.div_2exp(1);
    assert_mpf_equal(value, gmpxx::mpf_class("1.0", value.get_prec()));
    value.mul_2exp(1);
    assert_mpf_equal(value, gmpxx::mpf_class("2.0", value.get_prec()));

    assert(gmpxx::hypot(gmpxx::mpf_class(-3), 4.0) > gmpxx::mpf_class("4.9"));
    assert(gmpxx::hypot(-3.0, gmpxx::mpf_class(4)) < gmpxx::mpf_class("5.1"));
    assert(gmpxx::hypot(gmpxx::mpf_class(-3), 4L) > gmpxx::mpf_class("4.9"));
    assert(gmpxx::hypot(-3L, gmpxx::mpf_class(4)) < gmpxx::mpf_class("5.1"));
    assert(gmpxx::hypot(gmpxx::mpf_class(-3), 4UL) > gmpxx::mpf_class("4.9"));
    assert(gmpxx::hypot(3UL, gmpxx::mpf_class(4)) < gmpxx::mpf_class("5.1"));
}

void test_epsilon_and_remainder()
{
    gmpxx::mpf_class epsilon(0, static_cast<mp_bitcnt_t>(128));
    epsilon.set_epsilon();

    gmpxx::mpf_class expected_epsilon(1, epsilon.get_prec());
    mpf_div_2exp(expected_epsilon.get_mpf_t(),
                 expected_epsilon.get_mpf_t(),
                 epsilon.get_prec() - 1);
    assert_mpf_equal(epsilon, expected_epsilon);

    struct case_data {
        const char* x;
        const char* y;
        const char* quotient;
        const char* remainder;
    };

    const case_data cases[] = {
        {"10.5", "3.2", "3", "0.9"},
        {"23.7", "4.5", "5", "1.2"},
        {"5.3", "2.1", "2", "1.1"},
        {"-15.8", "6.1", "-3", "2.5"},
        {"-7.6", "2.3", "-4", "1.6"},
    };

    for (const case_data& c : cases) {
        gmpxx::mpf_class x(c.x, static_cast<mp_bitcnt_t>(256));
        gmpxx::mpf_class y(c.y, static_cast<mp_bitcnt_t>(256));
        gmpxx::mpz_class quotient;
        gmpxx::mpf_class remainder = gmpxx::mpf_remainder(x, y, &quotient);

        assert(quotient == gmpxx::mpz_class(c.quotient));
        assert_mpf_close(remainder, gmpxx::mpf_class(c.remainder, remainder.get_prec()), 220);

        gmpxx::mpf_class reconstructed = quotient * y + remainder;
        assert_mpf_close(reconstructed, x, 220);
    }

    bool threw = false;
    try {
        (void)gmpxx::mpf_remainder(gmpxx::mpf_class("1"), gmpxx::mpf_class("0"));
    } catch (const std::domain_error&) {
        threw = true;
    }
    assert(threw);
}

void test_sign_and_exact_math_functions()
{
    assert(gmpxx::sgn(gmpxx::mpf_class("123.456")) > 0);
    assert(gmpxx::sgn(gmpxx::mpf_class("-123.456")) < 0);
    assert(gmpxx::sgn(gmpxx::mpf_class("0")) == 0);

    assert(gmpxx::abs(gmpxx::mpz_class("-456")) == gmpxx::mpz_class("456"));
    assert(gmpxx::abs(gmpxx::mpq_class("-1/3")) == gmpxx::mpq_class("1/3"));
    assert(gmpxx::sqrt(gmpxx::mpz_class("24")) == gmpxx::mpz_class("4"));
    assert(gmpxx::gcd(gmpxx::mpz_class("24"), gmpxx::mpz_class("36")) == gmpxx::mpz_class("12"));
    assert(gmpxx::lcm(gmpxx::mpz_class("24"), gmpxx::mpz_class("36")) == gmpxx::mpz_class("72"));
    assert(gmpxx::factorial(gmpxx::mpz_class(std::int64_t{5})) == gmpxx::mpz_class("120"));
    assert(gmpxx::primorial(gmpxx::mpz_class(std::int64_t{5})) == gmpxx::mpz_class("30"));
    assert(gmpxx::fibonacci(gmpxx::mpz_class(std::int64_t{7})) == gmpxx::mpz_class("13"));
}

void test_taylor_denominator_uint64_helpers()
{
    const mp_bitcnt_t precision = 192;
    const std::uint64_t k = UINT64_C(32768);
    const std::uint64_t sin_den1 = gmpxx::mpf_math_detail::checked_taylor_counter_product(UINT64_C(2), k);
    const std::uint64_t sin_den2 = gmpxx::mpf_math_detail::checked_taylor_counter_add(sin_den1, UINT64_C(1));
    const std::uint64_t sin_den = gmpxx::mpf_math_detail::checked_taylor_counter_product(sin_den1, sin_den2);
    const std::uint64_t cos_den = gmpxx::mpf_math_detail::checked_taylor_counter_product(sin_den1 - UINT64_C(1), sin_den1);
    const std::uint64_t odd_den = gmpxx::mpf_math_detail::checked_taylor_odd_denominator(k);

    static_assert(UINT64_C(4295032832) > std::numeric_limits<std::uint32_t>::max(), "");
    assert(sin_den == UINT64_C(4295032832));
    assert(cos_den == UINT64_C(4294901760));
    assert(odd_den == UINT64_C(65537));
    assert_mpf_equal(gmpxx::mpf_math_detail::make_u64(sin_den, precision),
                     gmpxx::mpf_class("4295032832", precision));
    assert_mpf_equal(gmpxx::mpf_math_detail::make_u64(cos_den, precision),
                     gmpxx::mpf_class("4294901760", precision));
    assert_mpf_equal(gmpxx::mpf_math_detail::make_u64(odd_den, precision),
                     gmpxx::mpf_class("65537", precision));
    assert(gmpxx::mpf_math_detail::checked_taylor_shift_count(17, odd_den) ==
           static_cast<mp_bitcnt_t>(UINT64_C(1114129)));

    bool product_threw = false;
    try {
        (void)gmpxx::mpf_math_detail::checked_taylor_counter_product(
            std::numeric_limits<std::uint64_t>::max(), UINT64_C(2));
    } catch (const std::overflow_error&) {
        product_threw = true;
    }
    assert(product_threw);

    bool add_threw = false;
    try {
        (void)gmpxx::mpf_math_detail::checked_taylor_counter_add(
            std::numeric_limits<std::uint64_t>::max(), UINT64_C(1));
    } catch (const std::overflow_error&) {
        add_threw = true;
    }
    assert(add_threw);

    bool odd_threw = false;
    try {
        (void)gmpxx::mpf_math_detail::checked_taylor_odd_denominator(
            (std::numeric_limits<std::uint64_t>::max() / UINT64_C(2)) + UINT64_C(1));
    } catch (const std::overflow_error&) {
        odd_threw = true;
    }
    assert(odd_threw);

    bool shift_threw = false;
    try {
        (void)gmpxx::mpf_math_detail::checked_taylor_shift_count(
            std::numeric_limits<mp_bitcnt_t>::max(), UINT64_C(2));
    } catch (const std::overflow_error&) {
        shift_threw = true;
    }
    assert(shift_threw);

    bool counter_threw = false;
    std::uint64_t counter = std::numeric_limits<std::uint64_t>::max();
    try {
        gmpxx::mpf_math_detail::increment_taylor_counter(counter);
    } catch (const std::overflow_error&) {
        counter_threw = true;
    }
    assert(counter_threw);
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_sqrt();
    test_abs();
    test_neg();
    test_rounding_functions();
    test_hypot_and_scaling();
    test_epsilon_and_remainder();
    test_sign_and_exact_math_functions();
    test_taylor_denominator_uint64_helpers();
    return 0;
}
