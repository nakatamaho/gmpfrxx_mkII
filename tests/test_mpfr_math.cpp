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
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>

namespace {

using unary_c_api = int (*)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t);
using unary_round_to_integer_c_api = int (*)(mpfr_ptr, mpfr_srcptr);
using unary_u_c_api = int (*)(mpfr_ptr, mpfr_srcptr, unsigned long, mpfr_rnd_t);
using unary_long_c_api = int (*)(mpfr_ptr, mpfr_srcptr, long, mpfr_rnd_t);
using unary_order_c_api = int (*)(mpfr_ptr, long, mpfr_srcptr, mpfr_rnd_t);
using unary_uintmax_c_api = int (*)(mpfr_ptr, mpfr_srcptr, uintmax_t, mpfr_rnd_t);
using unary_intmax_c_api = int (*)(mpfr_ptr, mpfr_srcptr, intmax_t, mpfr_rnd_t);
using binary_c_api = int (*)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t);
using binary_u_c_api = int (*)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, unsigned long, mpfr_rnd_t);
using ternary_c_api = int (*)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t);
using quaternary_c_api = int (*)(mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t);
using unary_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpfr_class&);
using unary_round_to_integer_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpfr_class&);
using unary_u_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpfr_class&, unsigned long);
using unary_long_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpfr_class&, long);
using unary_order_wrapper = mpfrxx::mpfr_class (*)(long, const mpfrxx::mpfr_class&);
using unary_uintmax_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpfr_class&, uintmax_t);
using unary_intmax_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpfr_class&, intmax_t);
using constant_c_api = int (*)(mpfr_ptr, mpfr_rnd_t);

void assert_same_mpfr_value(const mpfrxx::mpfr_class& got, mpfr_srcptr expected)
{
    assert(mpfr_cmp(got.mpfr_data(), expected) == 0);
}

void assert_mpfr_close(const mpfrxx::mpfr_class& got,
                       const mpfrxx::mpfr_class& expected,
                       mp_bitcnt_t tolerance_bits)
{
    const mpfr_prec_t precision = std::max(got.precision(), expected.precision());
    mpfr_t diff;
    mpfr_t tolerance;
    mpfr_init2(diff, precision);
    mpfr_init2(tolerance, precision);

    mpfr_sub(diff, got.mpfr_data(), expected.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    mpfr_abs(diff, diff, mpfrxx::mpfr_class::default_rounding());
    mpfr_set_ui(tolerance, 1ul, mpfrxx::mpfr_class::default_rounding());
    mpfr_div_2ui(tolerance, tolerance, static_cast<unsigned long>(tolerance_bits), mpfrxx::mpfr_class::default_rounding());
    assert(mpfr_cmp(diff, tolerance) < 0);

    mpfr_clear(tolerance);
    mpfr_clear(diff);
}

int mpfr_ceil_reference(mpfr_ptr rop, mpfr_srcptr op)
{
    return mpfr_ceil(rop, op);
}

int mpfr_floor_reference(mpfr_ptr rop, mpfr_srcptr op)
{
    return mpfr_floor(rop, op);
}

int mpfr_trunc_reference(mpfr_ptr rop, mpfr_srcptr op)
{
    return mpfr_trunc(rop, op);
}

void check_unary(const mpfrxx::mpfr_class& value, unary_c_api c_api, unary_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, value.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(value);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_unary_round_to_integer(const mpfrxx::mpfr_class& value,
                                  unary_round_to_integer_c_api c_api,
                                  unary_round_to_integer_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, value.mpfr_data());

    const mpfrxx::mpfr_class got = wrapper(value);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_unary_round_to_integer_signed_zero(unary_round_to_integer_c_api c_api,
                                             unary_round_to_integer_wrapper wrapper)
{
    mpfrxx::mpfr_class negative_zero = mpfrxx::mpfr_class::with_precision(128);
    mpfr_set_zero(negative_zero.mpfr_data(), -1);

    mpfr_t expected;
    mpfr_init2(expected, negative_zero.precision());
    c_api(expected, negative_zero.mpfr_data());

    const mpfrxx::mpfr_class got = wrapper(negative_zero);
    assert(got.precision() == negative_zero.precision());
    assert(mpfr_zero_p(got.mpfr_data()) != 0);
    assert(mpfr_signbit(got.mpfr_data()) == mpfr_signbit(expected));

    mpfr_clear(expected);
}

void check_unary_u(const mpfrxx::mpfr_class& value,
                   unsigned long unit,
                   unary_u_c_api c_api,
                   unary_u_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, value.mpfr_data(), unit, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(value, unit);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_unary_long(const mpfrxx::mpfr_class& value,
                      long scalar,
                      unary_long_c_api c_api,
                      unary_long_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, value.mpfr_data(), scalar, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(value, scalar);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_unary_order(long order,
                       const mpfrxx::mpfr_class& value,
                       unary_order_c_api c_api,
                       unary_order_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, order, value.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(order, value);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_unary_uintmax(const mpfrxx::mpfr_class& value,
                         uintmax_t scalar,
                         unary_uintmax_c_api c_api,
                         unary_uintmax_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, value.mpfr_data(), scalar, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(value, scalar);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_unary_intmax(const mpfrxx::mpfr_class& value,
                        intmax_t scalar,
                        unary_intmax_c_api c_api,
                        unary_intmax_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    c_api(expected, value.mpfr_data(), scalar, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(value, scalar);
    assert(got.precision() == value.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_binary(const mpfrxx::mpfr_class& lhs,
                  const mpfrxx::mpfr_class& rhs,
                  binary_c_api c_api,
                  mpfrxx::mpfr_class (*wrapper)(const mpfrxx::mpfr_class&, const mpfrxx::mpfr_class&))
{
    const mpfr_prec_t precision = std::max(lhs.precision(), rhs.precision());
    mpfr_t expected;
    mpfr_init2(expected, precision);
    c_api(expected, lhs.mpfr_data(), rhs.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(lhs, rhs);
    assert(got.precision() == precision);
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_binary_u(const mpfrxx::mpfr_class& lhs,
                    const mpfrxx::mpfr_class& rhs,
                    unsigned long unit,
                    binary_u_c_api c_api,
                    mpfrxx::mpfr_class (*wrapper)(const mpfrxx::mpfr_class&,
                                                  const mpfrxx::mpfr_class&,
                                                  unsigned long))
{
    const mpfr_prec_t precision = std::max(lhs.precision(), rhs.precision());
    mpfr_t expected;
    mpfr_init2(expected, precision);
    c_api(expected, lhs.mpfr_data(), rhs.mpfr_data(), unit, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(lhs, rhs, unit);
    assert(got.precision() == precision);
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_remquo(const mpfrxx::mpfr_class& lhs, const mpfrxx::mpfr_class& rhs)
{
    const mpfr_prec_t precision = std::max(lhs.precision(), rhs.precision());
    mpfr_t expected;
    mpfr_init2(expected, precision);
    long expected_quotient = 0;
    mpfr_remquo(expected,
                &expected_quotient,
                lhs.mpfr_data(),
                rhs.mpfr_data(),
                mpfrxx::mpfr_class::default_rounding());

    const auto got = mpfrxx::remquo(lhs, rhs);
    assert(got.first.precision() == precision);
    assert(got.second == expected_quotient);
    assert_same_mpfr_value(got.first, expected);

    mpfr_clear(expected);
}

void check_ternary(const mpfrxx::mpfr_class& a,
                   const mpfrxx::mpfr_class& b,
                   const mpfrxx::mpfr_class& c,
                   ternary_c_api c_api,
                   mpfrxx::mpfr_class (*wrapper)(const mpfrxx::mpfr_class&,
                                                 const mpfrxx::mpfr_class&,
                                                 const mpfrxx::mpfr_class&))
{
    const mpfr_prec_t precision = std::max({a.precision(), b.precision(), c.precision()});
    mpfr_t expected;
    mpfr_init2(expected, precision);
    c_api(expected, a.mpfr_data(), b.mpfr_data(), c.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(a, b, c);
    assert(got.precision() == precision);
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_quaternary(const mpfrxx::mpfr_class& a,
                      const mpfrxx::mpfr_class& b,
                      const mpfrxx::mpfr_class& c,
                      const mpfrxx::mpfr_class& d,
                      quaternary_c_api c_api,
                      mpfrxx::mpfr_class (*wrapper)(const mpfrxx::mpfr_class&,
                                                    const mpfrxx::mpfr_class&,
                                                    const mpfrxx::mpfr_class&,
                                                    const mpfrxx::mpfr_class&))
{
    const mpfr_prec_t precision = std::max({a.precision(), b.precision(), c.precision(), d.precision()});
    mpfr_t expected;
    mpfr_init2(expected, precision);
    c_api(expected,
          a.mpfr_data(),
          b.mpfr_data(),
          c.mpfr_data(),
          d.mpfr_data(),
          mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(a, b, c, d);
    assert(got.precision() == precision);
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void check_constant(mpfr_prec_t precision,
                    constant_c_api c_api,
                    mpfrxx::mpfr_class (*wrapper)(mpfr_prec_t))
{
    mpfr_t expected;
    mpfr_init2(expected, precision);
    c_api(expected, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = wrapper(precision);
    assert(got.precision() == precision);
    assert_same_mpfr_value(got, expected);

    mpfr_clear(expected);
}

void test_compile_time_surface()
{
    using mpfrxx::mpfr_class;
    using expr_type = decltype(std::declval<const mpfr_class&>() +
                               std::declval<const mpfr_class&>());

    static_assert(std::is_same<decltype(mpfrxx::sqrt(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sqr(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sqrt_ui(9ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sqrt_ui(9ul, static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_pi()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_pi(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_log2()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_log2(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_e()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_e(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_log10()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_log10(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pi_over_two()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pi_over_two(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_euler()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_euler(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_catalan()), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::const_catalan(static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::rec_sqrt(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::ceil(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::floor(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::trunc(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::mul_2ui(std::declval<const mpfr_class&>(), 3ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::div_2ui(std::declval<const mpfr_class&>(), 3ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::mul_2si(std::declval<const mpfr_class&>(), -3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::div_2si(std::declval<const mpfr_class&>(), -3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::agm(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fma(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fms(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fmma(std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fmms(std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin_cos(std::declval<const mpfr_class&>())),
                               std::pair<mpfr_class, mpfr_class>>::value);
    static_assert(std::is_same<decltype(mpfrxx::sinh_cosh(std::declval<const mpfr_class&>())),
                               std::pair<mpfr_class, mpfr_class>>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cos(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::tan(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::asin(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::acos(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atan(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atan2(std::declval<const mpfr_class&>(),
                                                      std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sinh(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cosh(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::tanh(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::asinh(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::acosh(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atanh(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sec(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::csc(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cot(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sech(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::csch(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::coth(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sinu(std::declval<const mpfr_class&>(), 360ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cosu(std::declval<const mpfr_class&>(), 360ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::tanu(std::declval<const mpfr_class&>(), 360ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::asinu(std::declval<const mpfr_class&>(), 360ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::acosu(std::declval<const mpfr_class&>(), 360ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atanu(std::declval<const mpfr_class&>(), 360ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atan2u(std::declval<const mpfr_class&>(),
                                                       std::declval<const mpfr_class&>(),
                                                       360ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sinpi(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cospi(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::tanpi(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::asinpi(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::acospi(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atanpi(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atan2pi(std::declval<const mpfr_class&>(),
                                                        std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cbrt(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::rootn_ui(std::declval<const mpfr_class&>(), 3ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::rootn_si(std::declval<const mpfr_class&>(), 3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::powr(std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::min(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::max(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::dim(std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::hypot(std::declval<const mpfr_class&>(),
                                                      std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::remainder(std::declval<const mpfr_class&>(),
                                                          std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fmod(std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::remquo(std::declval<const mpfr_class&>(),
                                                       std::declval<const mpfr_class&>())),
                               std::pair<mpfr_class, long>>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow_si(std::declval<const mpfr_class&>(), -3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow_ui(std::declval<const mpfr_class&>(), 3ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow_z(std::declval<const mpfr_class&>(),
                                                      std::declval<const mpfrxx::mpz_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow_sj(std::declval<const mpfr_class&>(),
                                                       static_cast<intmax_t>(-3))),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow_uj(std::declval<const mpfr_class&>(),
                                                       static_cast<uintmax_t>(3))),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::ui_pow(3ul, std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::ui_pow_ui(3ul, 5ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::ui_pow_ui(3ul, 5ul, static_cast<mpfr_prec_t>(128))),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::compound_si(std::declval<const mpfr_class&>(), 3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log2(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log10(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log1p(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log2p1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log10p1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log_ui(5ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log_ui(5ul, static_cast<mpfr_prec_t>(128))), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp2(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp10(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::expm1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp2m1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp10m1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::eint(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::li2(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::erf(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::erfc(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::gamma(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::reciprocal_gamma(std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::gamma_inc(std::declval<const mpfr_class&>(),
                                                          std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::beta(std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::lngamma(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::lgamma(std::declval<const mpfr_class&>())),
                               std::pair<mpfr_class, int>>::value);
    static_assert(std::is_same<decltype(mpfrxx::digamma(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::zeta(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::zeta_ui(5ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::zeta_ui(5ul, static_cast<mpfr_prec_t>(128))),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fac_ui(5ul)), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fac_ui(5ul, static_cast<mpfr_prec_t>(128))),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::j0(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::j1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::jn(2l, std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::y0(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::y1(std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::yn(2l, std::declval<const mpfr_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::ai(std::declval<const mpfr_class&>())), mpfr_class>::value);

    static_assert(std::is_same<decltype(mpfrxx::sqrt(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sqr(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::rec_sqrt(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::ceil(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::floor(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::trunc(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::mul_2ui(std::declval<expr_type>(), 3ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::div_2ui(std::declval<expr_type>(), 3ul)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::mul_2si(std::declval<expr_type>(), -3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::div_2si(std::declval<expr_type>(), -3l)),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::remainder(std::declval<expr_type>(),
                                                          std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fmod(std::declval<expr_type>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::remquo(std::declval<expr_type>(),
                                                       std::declval<const mpfr_class&>())),
                               std::pair<mpfr_class, long>>::value);
    static_assert(std::is_same<decltype(mpfrxx::agm(std::declval<expr_type>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fma(std::declval<expr_type>(),
                                                    std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin_cos(std::declval<expr_type>())),
                               std::pair<mpfr_class, mpfr_class>>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atan2(std::declval<expr_type>(),
                                                      std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow(std::declval<expr_type>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::hypot(std::declval<expr_type>(),
                                                      std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fms(std::declval<expr_type>(),
                                                    std::declval<const mpfr_class&>(),
                                                    std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fmma(std::declval<expr_type>(),
                                                     std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>(),
                                                     std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::erf(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::gamma_inc(std::declval<expr_type>(),
                                                          std::declval<const mpfr_class&>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::reciprocal_gamma(std::declval<expr_type>())),
                               mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::lgamma(std::declval<expr_type>())),
                               std::pair<mpfr_class, int>>::value);
}

void test_unary_functions_against_mpfr()
{
    const mpfrxx::mpfr_class values[] = {
        mpfrxx::mpfr_class("0.25", 192),
        mpfrxx::mpfr_class("1.5", 224),
        mpfrxx::mpfr_class("4.0", 160),
    };

    for (const mpfrxx::mpfr_class& value : values) {
        check_unary(value, mpfr_sqrt, mpfrxx::sqrt);
        check_unary(value, mpfr_sqr, mpfrxx::sqr);
        check_unary(value, mpfr_rec_sqrt, mpfrxx::rec_sqrt);
        check_unary(value, mpfr_sin, mpfrxx::sin);
        check_unary(value, mpfr_cos, mpfrxx::cos);
        check_unary(value, mpfr_tan, mpfrxx::tan);
        check_unary(value, mpfr_asin, mpfrxx::asin);
        check_unary(value, mpfr_acos, mpfrxx::acos);
        check_unary(value, mpfr_atan, mpfrxx::atan);
        check_unary(value, mpfr_sinh, mpfrxx::sinh);
        check_unary(value, mpfr_cosh, mpfrxx::cosh);
        check_unary(value, mpfr_tanh, mpfrxx::tanh);
        check_unary(value, mpfr_asinh, mpfrxx::asinh);
        check_unary(value, mpfr_atanh, mpfrxx::atanh);
        check_unary(value, mpfr_sec, mpfrxx::sec);
        check_unary(value, mpfr_csc, mpfrxx::csc);
        check_unary(value, mpfr_cot, mpfrxx::cot);
        check_unary(value, mpfr_sech, mpfrxx::sech);
        check_unary(value, mpfr_csch, mpfrxx::csch);
        check_unary(value, mpfr_coth, mpfrxx::coth);
        check_unary(value, mpfr_sinpi, mpfrxx::sinpi);
        check_unary(value, mpfr_cospi, mpfrxx::cospi);
        check_unary(value, mpfr_tanpi, mpfrxx::tanpi);
        check_unary(value, mpfr_asinpi, mpfrxx::asinpi);
        check_unary(value, mpfr_acospi, mpfrxx::acospi);
        check_unary(value, mpfr_atanpi, mpfrxx::atanpi);
        check_unary_u(value, 360ul, mpfr_sinu, mpfrxx::sinu);
        check_unary_u(value, 360ul, mpfr_cosu, mpfrxx::cosu);
        check_unary_u(value, 360ul, mpfr_tanu, mpfrxx::tanu);
        check_unary_u(value, 360ul, mpfr_asinu, mpfrxx::asinu);
        check_unary_u(value, 360ul, mpfr_acosu, mpfrxx::acosu);
        check_unary_u(value, 360ul, mpfr_atanu, mpfrxx::atanu);
        check_unary(value, mpfr_cbrt, mpfrxx::cbrt);
        check_unary_u(value, 3ul, mpfr_rootn_ui, mpfrxx::rootn_ui);
        check_unary_long(value, 3l, mpfr_rootn_si, mpfrxx::rootn_si);
        check_unary(value, mpfr_log, mpfrxx::log);
        check_unary(value, mpfr_log2, mpfrxx::log2);
        check_unary(value, mpfr_log10, mpfrxx::log10);
        check_unary(value, mpfr_log1p, mpfrxx::log1p);
        check_unary(value, mpfr_log2p1, mpfrxx::log2p1);
        check_unary(value, mpfr_log10p1, mpfrxx::log10p1);
        check_unary(value, mpfr_exp, mpfrxx::exp);
        check_unary(value, mpfr_exp2, mpfrxx::exp2);
        check_unary(value, mpfr_exp10, mpfrxx::exp10);
        check_unary(value, mpfr_expm1, mpfrxx::expm1);
        check_unary(value, mpfr_exp2m1, mpfrxx::exp2m1);
        check_unary(value, mpfr_exp10m1, mpfrxx::exp10m1);
        check_unary(value, mpfr_eint, mpfrxx::eint);
        check_unary(value, mpfr_erf, mpfrxx::erf);
        check_unary(value, mpfr_erfc, mpfrxx::erfc);
        check_unary(value, mpfr_gamma, mpfrxx::gamma);
        check_unary(value, mpfr_lngamma, mpfrxx::lngamma);
        check_unary(value, mpfr_digamma, mpfrxx::digamma);
        check_unary(value, mpfr_zeta, mpfrxx::zeta);
        check_unary(value, mpfr_j0, mpfrxx::j0);
        check_unary(value, mpfr_j1, mpfrxx::j1);
        check_unary_order(2l, value, mpfr_jn, mpfrxx::jn);
        check_unary(value, mpfr_y0, mpfrxx::y0);
        check_unary(value, mpfr_y1, mpfrxx::y1);
        check_unary_order(2l, value, mpfr_yn, mpfrxx::yn);
        check_unary(value, mpfr_ai, mpfrxx::ai);
    }

    const mpfrxx::mpfr_class acosh_value("1.5", 224);
    check_unary(acosh_value, mpfr_acosh, mpfrxx::acosh);
    const mpfrxx::mpfr_class li2_value("0.25", 192);
    check_unary(li2_value, mpfr_li2, mpfrxx::li2);
}

void test_round_to_integer_functions_against_mpfr()
{
    const mpfrxx::mpfr_class values[] = {
        mpfrxx::mpfr_class("-2.75", 192),
        mpfrxx::mpfr_class("-0.25", 224),
        mpfrxx::mpfr_class("0.25", 160),
        mpfrxx::mpfr_class("1.5", 192),
        mpfrxx::mpfr_class("4.0", 224),
    };

    for (const mpfrxx::mpfr_class& value : values) {
        check_unary_round_to_integer(value, mpfr_ceil_reference, mpfrxx::ceil);
        check_unary_round_to_integer(value, mpfr_floor_reference, mpfrxx::floor);
        check_unary_round_to_integer(value, mpfr_trunc_reference, mpfrxx::trunc);
    }

    check_unary_round_to_integer_signed_zero(mpfr_ceil_reference, mpfrxx::ceil);
    check_unary_round_to_integer_signed_zero(mpfr_floor_reference, mpfrxx::floor);
    check_unary_round_to_integer_signed_zero(mpfr_trunc_reference, mpfrxx::trunc);
}

void test_scaling_functions_against_mpfr()
{
    const mpfrxx::mpfr_class values[] = {
        mpfrxx::mpfr_class("-2.75", 192),
        mpfrxx::mpfr_class("0.25", 160),
        mpfrxx::mpfr_class("1.5", 224),
    };

    for (const mpfrxx::mpfr_class& value : values) {
        check_unary_u(value, 5ul, mpfr_mul_2ui, mpfrxx::mul_2ui);
        check_unary_u(value, 3ul, mpfr_div_2ui, mpfrxx::div_2ui);
        check_unary_long(value, -4l, mpfr_mul_2si, mpfrxx::mul_2si);
        check_unary_long(value, -2l, mpfr_div_2si, mpfrxx::div_2si);
    }
}

void test_lgamma_against_mpfr()
{
    const mpfrxx::mpfr_class value("1.75", 192);
    mpfr_t expected;
    mpfr_init2(expected, value.precision());
    int expected_sign = 0;
    mpfr_lgamma(expected, &expected_sign, value.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const auto got = mpfrxx::lgamma(value);
    assert(got.first.precision() == value.precision());
    assert(got.second == expected_sign);
    assert_same_mpfr_value(got.first, expected);

    mpfr_clear(expected);
}

void test_sqrt_ui_against_mpfr()
{
    mpfr_t expected;
    mpfr_init2(expected, 192);
    mpfr_sqrt_ui(expected, 9ul, mpfrxx::mpfr_class::default_rounding());

    const mpfrxx::mpfr_class got = mpfrxx::sqrt_ui(9ul, 192);
    assert(got.precision() == 192);
    assert_same_mpfr_value(got, expected);
    mpfr_clear(expected);
}

void test_constants_against_mpfr()
{
    check_constant(160, mpfr_const_pi, mpfrxx::const_pi);
    check_constant(192, mpfr_const_log2, mpfrxx::const_log2);
    check_constant(224, mpfr_const_euler, mpfrxx::const_euler);
    check_constant(256, mpfr_const_catalan, mpfrxx::const_catalan);

    const mpfrxx::mpfr_class default_pi = mpfrxx::const_pi();
    assert(default_pi.precision() == mpfrxx::mpfr_class::default_precision());

    mpfr_t expected;
    mpfr_init2(expected, 192);
    mpfr_set_ui(expected, 1ul, mpfrxx::mpfr_class::default_rounding());
    mpfr_exp(expected, expected, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_e = mpfrxx::const_e(192);
    assert(got_e.precision() == 192);
    assert_same_mpfr_value(got_e, expected);

    mpfr_set_prec(expected, 224);
    mpfr_log_ui(expected, 10ul, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_log10 = mpfrxx::const_log10(224);
    assert(got_log10.precision() == 224);
    assert_same_mpfr_value(got_log10, expected);

    mpfr_set_prec(expected, 256);
    mpfr_const_pi(expected, mpfrxx::mpfr_class::default_rounding());
    mpfr_div_2ui(expected, expected, 1ul, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_pi_over_two = mpfrxx::pi_over_two(256);
    assert(got_pi_over_two.precision() == 256);
    assert_same_mpfr_value(got_pi_over_two, expected);
    assert(mpfrxx::const_e().precision() == mpfrxx::mpfr_class::default_precision());
    assert(mpfrxx::const_log10().precision() == mpfrxx::mpfr_class::default_precision());
    assert(mpfrxx::pi_over_two().precision() == mpfrxx::mpfr_class::default_precision());
    mpfr_clear(expected);
}

void test_extended_transcendent_identities()
{
    const mpfr_prec_t precision = 224;
    const mpfrxx::mpfr_class zero("0", precision);
    const mpfrxx::mpfr_class one("1", precision);
    const mpfrxx::mpfr_class two("2", precision);
    const mpfrxx::mpfr_class four("4", precision);
    const mpfrxx::mpfr_class half("0.5", precision);
    const mpfrxx::mpfr_class quarter("0.25", precision);
    const mpfrxx::mpfr_class x("0.75", precision);
    const mpfrxx::mpfr_class y("1.75", precision);
    const mpfrxx::mpfr_class three("3", precision);

    assert_mpfr_close(mpfrxx::const_e(precision), mpfrxx::exp(one), 180);
    assert_mpfr_close(mpfrxx::const_log10(precision), mpfrxx::log(mpfrxx::mpfr_class(10, precision)), 180);
    assert_mpfr_close(mpfrxx::pi_over_two(precision) * two, mpfrxx::const_pi(precision), 180);
    assert_mpfr_close(mpfrxx::log(mpfrxx::exp(one)), one, 170);
    assert_mpfr_close(mpfrxx::log(mpfrxx::mpfr_class(10, precision)), mpfrxx::log(mpfrxx::exp10(one)), 170);
    assert_mpfr_close(mpfrxx::const_pi(precision) / two + mpfrxx::const_pi(precision) / four,
                      three * mpfrxx::const_pi(precision) / four,
                      180);
    assert_mpfr_close(two * mpfrxx::const_pi(precision), mpfrxx::const_pi(precision) * two, 200);

    assert(mpfrxx::asin(zero) == zero);
    assert(mpfrxx::acos(one) == zero);
    assert_mpfr_close(mpfrxx::asin(half) + mpfrxx::acos(half),
                      mpfrxx::const_pi(precision) / two,
                      160);
    assert_mpfr_close(mpfrxx::sin(mpfrxx::asin(half)), half, 165);
    assert_mpfr_close(mpfrxx::cos(mpfrxx::acos(half)), half, 165);

    assert(mpfrxx::sinh(zero) == zero);
    assert(mpfrxx::cosh(zero) == one);
    assert(mpfrxx::tanh(zero) == zero);
    assert_mpfr_close(mpfrxx::cosh(x) * mpfrxx::cosh(x) - mpfrxx::sinh(x) * mpfrxx::sinh(x),
                      one,
                      155);
    assert_mpfr_close(mpfrxx::tanh(x), mpfrxx::sinh(x) / mpfrxx::cosh(x), 175);

    assert(mpfrxx::asinh(zero) == zero);
    assert(mpfrxx::acosh(one) == zero);
    assert(mpfrxx::atanh(zero) == zero);
    assert_mpfr_close(mpfrxx::sinh(mpfrxx::asinh(quarter)), quarter, 160);
    assert_mpfr_close(mpfrxx::cosh(mpfrxx::acosh(y)), y, 160);
    assert_mpfr_close(mpfrxx::tanh(mpfrxx::atanh(quarter)), quarter, 160);

    assert(mpfrxx::exp2(zero) == one);
    assert_mpfr_close(mpfrxx::exp2(three), mpfrxx::mpfr_class(8, precision), 180);
    assert_mpfr_close(mpfrxx::exp2(-two), mpfrxx::mpfr_class("0.25", precision), 180);
    assert(mpfrxx::exp10(zero) == one);
    assert_mpfr_close(mpfrxx::exp10(two), mpfrxx::mpfr_class(100, precision), 175);
    assert_mpfr_close(mpfrxx::log2(mpfrxx::exp2(one)), one, 170);

    assert_mpfr_close(mpfrxx::gamma(one), one, 170);
    assert_mpfr_close(mpfrxx::gamma(mpfrxx::mpfr_class(5, precision)), mpfrxx::mpfr_class(24, precision), 150);
    assert_mpfr_close(mpfrxx::gamma(half), mpfrxx::sqrt(mpfrxx::const_pi(precision)), 150);
    assert_mpfr_close(mpfrxx::gamma(x + one), x * mpfrxx::gamma(x), 150);
    assert_mpfr_close(mpfrxx::exp(mpfrxx::lngamma(y)), mpfrxx::gamma(y), 145);
    assert_mpfr_close(mpfrxx::reciprocal_gamma(y) * mpfrxx::gamma(y), one, 160);
    assert(mpfrxx::reciprocal_gamma(zero) == zero);
}

void test_binary_and_ternary_against_mpfr()
{
    const mpfrxx::mpfr_class lhs("0.25", 192);
    const mpfrxx::mpfr_class rhs("1.5", 224);
    const mpfrxx::mpfr_class addend("0.125", 160);
    const mpfrxx::mpfr_class fourth("0.75", 256);

    check_binary(lhs, rhs, mpfr_agm, mpfrxx::agm);
    check_binary(lhs, rhs, mpfr_atan2, mpfrxx::atan2);
    check_binary(lhs, rhs, mpfr_atan2pi, mpfrxx::atan2pi);
    check_binary_u(lhs, rhs, 360ul, mpfr_atan2u, mpfrxx::atan2u);
    check_binary(lhs, rhs, mpfr_pow, mpfrxx::pow);
    check_binary(lhs, rhs, mpfr_powr, mpfrxx::powr);
    check_binary(lhs, rhs, mpfr_min, mpfrxx::min);
    check_binary(lhs, rhs, mpfr_max, mpfrxx::max);
    check_binary(rhs, lhs, mpfr_dim, mpfrxx::dim);
    check_binary(lhs, rhs, mpfr_hypot, mpfrxx::hypot);
    check_binary(lhs, rhs, mpfr_remainder, mpfrxx::remainder);
    check_binary(lhs, rhs, mpfr_fmod, mpfrxx::fmod);
    check_remquo(lhs, rhs);
    check_binary(mpfrxx::mpfr_class("-5.75", 224),
                 mpfrxx::mpfr_class("2.0", 160),
                 mpfr_remainder,
                 mpfrxx::remainder);
    check_binary(mpfrxx::mpfr_class("-5.75", 224),
                 mpfrxx::mpfr_class("2.0", 160),
                 mpfr_fmod,
                 mpfrxx::fmod);
    check_remquo(mpfrxx::mpfr_class("-5.75", 224), mpfrxx::mpfr_class("2.0", 160));
    check_binary(rhs, lhs, mpfr_gamma_inc, mpfrxx::gamma_inc);
    check_binary(lhs, rhs, mpfr_beta, mpfrxx::beta);
    check_ternary(lhs, rhs, addend, mpfr_fma, mpfrxx::fma);
    check_ternary(lhs, rhs, addend, mpfr_fms, mpfrxx::fms);
    check_quaternary(lhs, rhs, addend, fourth, mpfr_fmma, mpfrxx::fmma);
    check_quaternary(lhs, rhs, addend, fourth, mpfr_fmms, mpfrxx::fmms);
}

void test_power_scalar_functions_against_mpfr()
{
    const mpfrxx::mpfr_class base("1.25", 192);
    const mpfrxx::mpfr_class exponent("2.5", 224);

    check_unary_long(base, -3l, mpfr_pow_si, mpfrxx::pow_si);
    check_unary_u(base, 5ul, mpfr_pow_ui, mpfrxx::pow_ui);
    check_unary_intmax(base, static_cast<intmax_t>(-3), mpfr_pow_sj, mpfrxx::pow_sj);
    check_unary_uintmax(base, static_cast<uintmax_t>(5), mpfr_pow_uj, mpfrxx::pow_uj);
    check_unary_long(base, 4l, mpfr_compound_si, mpfrxx::compound_si);

    mpfr_t expected;
    mpfr_init2(expected, base.precision());

    const mpfrxx::mpz_class exponent_z(5);
    mpfr_pow_z(expected, base.mpfr_data(), exponent_z.mpz_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_pow_z = mpfrxx::pow_z(base, exponent_z);
    assert(got_pow_z.precision() == base.precision());
    assert_same_mpfr_value(got_pow_z, expected);

    mpfr_set_prec(expected, exponent.precision());
    mpfr_ui_pow(expected, 3ul, exponent.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_ui_pow = mpfrxx::ui_pow(3ul, exponent);
    assert(got_ui_pow.precision() == exponent.precision());
    assert_same_mpfr_value(got_ui_pow, expected);

    mpfr_set_prec(expected, 160);
    mpfr_ui_pow_ui(expected, 3ul, 5ul, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_ui_pow_ui = mpfrxx::ui_pow_ui(3ul, 5ul, 160);
    assert(got_ui_pow_ui.precision() == 160);
    assert_same_mpfr_value(got_ui_pow_ui, expected);

    mpfr_set_prec(expected, 176);
    mpfr_log_ui(expected, 17ul, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_log_ui = mpfrxx::log_ui(17ul, 176);
    assert(got_log_ui.precision() == 176);
    assert_same_mpfr_value(got_log_ui, expected);

    mpfr_set_prec(expected, 192);
    mpfr_zeta_ui(expected, 5ul, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_zeta_ui = mpfrxx::zeta_ui(5ul, 192);
    assert(got_zeta_ui.precision() == 192);
    assert_same_mpfr_value(got_zeta_ui, expected);

    mpfr_set_prec(expected, 224);
    mpfr_fac_ui(expected, 12ul, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_fac_ui = mpfrxx::fac_ui(12ul, 224);
    assert(got_fac_ui.precision() == 224);
    assert_same_mpfr_value(got_fac_ui, expected);

    mpfr_clear(expected);
}

void test_paired_functions_against_mpfr()
{
    const mpfrxx::mpfr_class value("0.125", 192);
    mpfr_t expected_sin;
    mpfr_t expected_cos;
    mpfr_init2(expected_sin, value.precision());
    mpfr_init2(expected_cos, value.precision());
    mpfr_sin_cos(expected_sin, expected_cos, value.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const auto trig = mpfrxx::sin_cos(value);
    assert(trig.first.precision() == value.precision());
    assert(trig.second.precision() == value.precision());
    assert_same_mpfr_value(trig.first, expected_sin);
    assert_same_mpfr_value(trig.second, expected_cos);
    mpfr_clear(expected_sin);
    mpfr_clear(expected_cos);

    mpfr_t expected_sinh;
    mpfr_t expected_cosh;
    mpfr_init2(expected_sinh, value.precision());
    mpfr_init2(expected_cosh, value.precision());
    mpfr_sinh_cosh(expected_sinh, expected_cosh, value.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

    const auto hyperbolic = mpfrxx::sinh_cosh(value);
    assert(hyperbolic.first.precision() == value.precision());
    assert(hyperbolic.second.precision() == value.precision());
    assert_same_mpfr_value(hyperbolic.first, expected_sinh);
    assert_same_mpfr_value(hyperbolic.second, expected_cosh);
    mpfr_clear(expected_sinh);
    mpfr_clear(expected_cosh);
}

void test_expression_inputs()
{
    const mpfrxx::mpfr_class a("0.25", 192);
    const mpfrxx::mpfr_class b("0.5", 224);
    const mpfrxx::mpfr_class c("1.25", 224);
    const auto expr = a + b;
    const auto positive_expr = c + b;
    const mpfrxx::mpfr_class materialized(expr);
    const mpfrxx::mpfr_class positive_materialized(positive_expr);

    mpfr_t expected;
    mpfr_init2(expected, materialized.precision());
    mpfr_sqr(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got = mpfrxx::sqr(expr);
    assert(got.precision() == materialized.precision());
    assert_same_mpfr_value(got, expected);

    mpfr_mul_2si(expected, materialized.mpfr_data(), -3l, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_scaled = mpfrxx::mul_2si(expr, -3l);
    assert(got_scaled.precision() == materialized.precision());
    assert_same_mpfr_value(got_scaled, expected);

    mpfr_remainder(expected, materialized.mpfr_data(), b.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_remainder = mpfrxx::remainder(expr, b);
    assert(got_remainder.precision() == materialized.precision());
    assert_same_mpfr_value(got_remainder, expected);

    mpfr_asin(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_asin = mpfrxx::asin(expr);
    assert(got_asin.precision() == materialized.precision());
    assert_same_mpfr_value(got_asin, expected);

    mpfr_acos(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_acos = mpfrxx::acos(expr);
    assert(got_acos.precision() == materialized.precision());
    assert_same_mpfr_value(got_acos, expected);

    mpfr_sinh(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_sinh = mpfrxx::sinh(expr);
    assert(got_sinh.precision() == materialized.precision());
    assert_same_mpfr_value(got_sinh, expected);

    mpfr_cosh(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_cosh = mpfrxx::cosh(expr);
    assert(got_cosh.precision() == materialized.precision());
    assert_same_mpfr_value(got_cosh, expected);

    mpfr_tanh(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_tanh = mpfrxx::tanh(expr);
    assert(got_tanh.precision() == materialized.precision());
    assert_same_mpfr_value(got_tanh, expected);

    mpfr_asinh(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_asinh = mpfrxx::asinh(expr);
    assert(got_asinh.precision() == materialized.precision());
    assert_same_mpfr_value(got_asinh, expected);

    mpfr_atanh(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_atanh = mpfrxx::atanh(expr);
    assert(got_atanh.precision() == materialized.precision());
    assert_same_mpfr_value(got_atanh, expected);

    mpfr_exp2(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_exp2 = mpfrxx::exp2(expr);
    assert(got_exp2.precision() == materialized.precision());
    assert_same_mpfr_value(got_exp2, expected);

    mpfr_exp10(expected, materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_exp10 = mpfrxx::exp10(expr);
    assert(got_exp10.precision() == materialized.precision());
    assert_same_mpfr_value(got_exp10, expected);

    mpfr_set_prec(expected, positive_materialized.precision());
    mpfr_acosh(expected, positive_materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_acosh = mpfrxx::acosh(positive_expr);
    assert(got_acosh.precision() == positive_materialized.precision());
    assert_same_mpfr_value(got_acosh, expected);

    mpfr_gamma(expected, positive_materialized.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_gamma = mpfrxx::gamma(positive_expr);
    assert(got_gamma.precision() == positive_materialized.precision());
    assert_same_mpfr_value(got_gamma, expected);

    mpfr_ui_div(expected, 1ul, expected, mpfrxx::mpfr_class::default_rounding());
    const mpfrxx::mpfr_class got_reciprocal_gamma = mpfrxx::reciprocal_gamma(positive_expr);
    assert(got_reciprocal_gamma.precision() == positive_materialized.precision());
    assert_same_mpfr_value(got_reciprocal_gamma, expected);

    mpfr_clear(expected);
}

void test_extended_transcendent_precision_policy()
{
    const mpfrxx::mpfr_class x("0.25", 224);
    const mpfrxx::mpfr_class y("1.75", 256);

    assert(mpfrxx::asin(x).precision() == x.precision());
    assert(mpfrxx::acos(x).precision() == x.precision());
    assert(mpfrxx::sinh(x).precision() == x.precision());
    assert(mpfrxx::cosh(x).precision() == x.precision());
    assert(mpfrxx::tanh(x).precision() == x.precision());
    assert(mpfrxx::asinh(x).precision() == x.precision());
    assert(mpfrxx::acosh(y).precision() == y.precision());
    assert(mpfrxx::atanh(x).precision() == x.precision());
    assert(mpfrxx::exp2(x).precision() == x.precision());
    assert(mpfrxx::exp10(x).precision() == x.precision());
    assert(mpfrxx::gamma(y).precision() == y.precision());
    assert(mpfrxx::reciprocal_gamma(y).precision() == y.precision());
    assert(mpfrxx::lngamma(y).precision() == y.precision());

    const auto lgamma_result = mpfrxx::lgamma(y);
    assert(lgamma_result.first.precision() == y.precision());
}

void test_destination_precision_preservation_for_helpers()
{
    const mpfrxx::mpfr_class value("5.75", 192);
    const mpfrxx::mpfr_class divisor("2.0", 224);
    mpfrxx::mpfr_class destination = mpfrxx::mpfr_class::with_precision(96);

    mpfr_t expected;
    mpfr_init2(expected, destination.precision());

    mpfr_mul_2ui(expected, value.mpfr_data(), 5ul, mpfrxx::mpfr_class::default_rounding());
    destination = mpfrxx::mul_2ui(value, 5ul);
    assert(destination.precision() == 96);
    assert_same_mpfr_value(destination, expected);

    mpfr_remainder(expected, value.mpfr_data(), divisor.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    destination = mpfrxx::remainder(value, divisor);
    assert(destination.precision() == 96);
    assert_same_mpfr_value(destination, expected);

    mpfr_clear(expected);
}

void assert_double_close(const mpfrxx::mpfr_class& got, double expected)
{
    const double actual = got.to_double();
    const double scale = std::max(1.0, std::abs(expected));
    const double tolerance = 256.0 * std::numeric_limits<double>::epsilon() * scale;
    assert(std::abs(actual - expected) <= tolerance);
}

void assert_double_close_1e15(const mpfrxx::mpfr_class& got, double expected)
{
    const double actual = got.to_double();
    const double scale = std::max(1.0, std::abs(expected));
    assert(std::abs(actual - expected) <= 1e-15 * scale);
}

void test_double_trig_hyperbolic_random_smoke()
{
    std::mt19937_64 generator(0x6d7066725f73696eULL);
    std::uniform_real_distribution<double> magnitude_distribution(0.125, 0.75);
    std::uniform_int_distribution<int> sign_distribution(0, 1);
    std::uniform_real_distribution<double> positive_distribution(1.0, 2.0);
    constexpr double pi = 3.141592653589793238462643383279502884;

    for (int i = 0; i < 64; ++i) {
        const double sign = sign_distribution(generator) == 0 ? -1.0 : 1.0;
        const double input = sign * magnitude_distribution(generator);
        const double x = magnitude_distribution(generator);
        const double y = (sign_distribution(generator) == 0 ? -1.0 : 1.0) * magnitude_distribution(generator);
        const double positive_input = positive_distribution(generator);

        const mpfrxx::mpfr_class value(input, 192);
        const mpfrxx::mpfr_class x_value(x, 192);
        const mpfrxx::mpfr_class y_value(y, 224);
        const mpfrxx::mpfr_class positive_value(positive_input, 192);

        assert_double_close(mpfrxx::sin(value), std::sin(input));
        assert_double_close(mpfrxx::cos(value), std::cos(input));
        assert_double_close(mpfrxx::tan(value), std::tan(input));
        assert_double_close(mpfrxx::asin(value), std::asin(input));
        assert_double_close(mpfrxx::acos(value), std::acos(input));
        assert_double_close(mpfrxx::atan(value), std::atan(input));
        assert_double_close(mpfrxx::atan2(y_value, x_value), std::atan2(y, x));

        assert_double_close(mpfrxx::sinh(value), std::sinh(input));
        assert_double_close(mpfrxx::cosh(value), std::cosh(input));
        assert_double_close(mpfrxx::tanh(value), std::tanh(input));
        assert_double_close(mpfrxx::asinh(value), std::asinh(input));
        assert_double_close(mpfrxx::acosh(positive_value), std::acosh(positive_input));
        assert_double_close(mpfrxx::atanh(value), std::atanh(input));

        assert_double_close(mpfrxx::sec(value), 1.0 / std::cos(input));
        assert_double_close(mpfrxx::csc(value), 1.0 / std::sin(input));
        assert_double_close(mpfrxx::cot(value), 1.0 / std::tan(input));
        assert_double_close(mpfrxx::sech(value), 1.0 / std::cosh(input));
        assert_double_close(mpfrxx::csch(value), 1.0 / std::sinh(input));
        assert_double_close(mpfrxx::coth(value), 1.0 / std::tanh(input));

        assert_double_close(mpfrxx::sinu(value, 360ul), std::sin(input * 2.0 * pi / 360.0));
        assert_double_close(mpfrxx::cosu(value, 360ul), std::cos(input * 2.0 * pi / 360.0));
        assert_double_close(mpfrxx::tanu(value, 360ul), std::tan(input * 2.0 * pi / 360.0));
        assert_double_close(mpfrxx::asinu(value, 360ul), std::asin(input) * 360.0 / (2.0 * pi));
        assert_double_close(mpfrxx::acosu(value, 360ul), std::acos(input) * 360.0 / (2.0 * pi));
        assert_double_close(mpfrxx::atanu(value, 360ul), std::atan(input) * 360.0 / (2.0 * pi));
        assert_double_close(mpfrxx::atan2u(y_value, x_value, 360ul), std::atan2(y, x) * 360.0 / (2.0 * pi));

        assert_double_close(mpfrxx::sinpi(value), std::sin(pi * input));
        assert_double_close(mpfrxx::cospi(value), std::cos(pi * input));
        assert_double_close(mpfrxx::tanpi(value), std::tan(pi * input));
        assert_double_close(mpfrxx::asinpi(value), std::asin(input) / pi);
        assert_double_close(mpfrxx::acospi(value), std::acos(input) / pi);
        assert_double_close(mpfrxx::atanpi(value), std::atan(input) / pi);
        assert_double_close(mpfrxx::atan2pi(y_value, x_value), std::atan2(y, x) / pi);
    }
}

void test_double_transcendent_random_smoke_1e15()
{
    std::mt19937_64 generator(0x6578745f6d706672ULL);
    std::uniform_real_distribution<double> small_distribution(-0.6, 0.6);
    std::uniform_real_distribution<double> positive_distribution(0.25, 2.5);
    std::uniform_real_distribution<double> acosh_distribution(1.0, 2.5);

    for (int i = 0; i < 64; ++i) {
        const double input = small_distribution(generator);
        const double positive_input = positive_distribution(generator);
        const double acosh_input = acosh_distribution(generator);

        const mpfrxx::mpfr_class value(input, 256);
        const mpfrxx::mpfr_class positive_value(positive_input, 256);
        const mpfrxx::mpfr_class acosh_value(acosh_input, 256);

        assert_double_close_1e15(mpfrxx::asin(value), std::asin(input));
        assert_double_close_1e15(mpfrxx::acos(value), std::acos(input));
        assert_double_close_1e15(mpfrxx::atan(value), std::atan(input));
        assert_double_close_1e15(mpfrxx::sinh(value), std::sinh(input));
        assert_double_close_1e15(mpfrxx::cosh(value), std::cosh(input));
        assert_double_close_1e15(mpfrxx::tanh(value), std::tanh(input));
        assert_double_close_1e15(mpfrxx::asinh(value), std::asinh(input));
        assert_double_close_1e15(mpfrxx::acosh(acosh_value), std::acosh(acosh_input));
        assert_double_close_1e15(mpfrxx::atanh(value), std::atanh(input));
        assert_double_close_1e15(mpfrxx::exp(value), std::exp(input));
        assert_double_close_1e15(mpfrxx::exp2(value), std::exp2(input));
        assert_double_close_1e15(mpfrxx::exp10(value), std::pow(10.0, input));
        assert_double_close_1e15(mpfrxx::expm1(value), std::expm1(input));
        assert_double_close_1e15(mpfrxx::log(positive_value), std::log(positive_input));
        assert_double_close_1e15(mpfrxx::log2(positive_value), std::log2(positive_input));
        assert_double_close_1e15(mpfrxx::log10(positive_value), std::log10(positive_input));
        assert_double_close_1e15(mpfrxx::log1p(value), std::log1p(input));
        assert_double_close_1e15(mpfrxx::gamma(positive_value), std::tgamma(positive_input));
        assert_double_close_1e15(mpfrxx::reciprocal_gamma(positive_value), 1.0 / std::tgamma(positive_input));
    }
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_unary_functions_against_mpfr();
    test_round_to_integer_functions_against_mpfr();
    test_scaling_functions_against_mpfr();
    test_lgamma_against_mpfr();
    test_sqrt_ui_against_mpfr();
    test_constants_against_mpfr();
    test_binary_and_ternary_against_mpfr();
    test_power_scalar_functions_against_mpfr();
    test_paired_functions_against_mpfr();
    test_expression_inputs();
    test_extended_transcendent_precision_policy();
    test_destination_precision_preservation_for_helpers();
    test_double_trig_hyperbolic_random_smoke();
    test_double_transcendent_random_smoke_1e15();
    return 0;
}
