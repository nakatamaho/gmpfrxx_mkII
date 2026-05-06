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
#include <mpcxx_mkII.h>

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <utility>

namespace {

using unary_c_api = int (*)(mpc_ptr, mpc_srcptr, mpc_rnd_t);
using binary_c_api = int (*)(mpc_ptr, mpc_srcptr, mpc_srcptr, mpc_rnd_t);
using ternary_c_api = int (*)(mpc_ptr, mpc_srcptr, mpc_srcptr, mpc_srcptr, mpc_rnd_t);
using unary_real_c_api = int (*)(mpfr_ptr, mpc_srcptr, mpfr_rnd_t);
using unary_wrapper = mpfrxx::mpc_class (*)(const mpfrxx::mpc_class&);
using unary_real_wrapper = mpfrxx::mpfr_class (*)(const mpfrxx::mpc_class&);

void assert_same_mpc_value(const mpfrxx::mpc_class& got, const mpc_t expected)
{
    assert(mpfr_cmp(mpc_realref(got.mpc_data()), mpc_realref(expected)) == 0);
    assert(mpfr_cmp(mpc_imagref(got.mpc_data()), mpc_imagref(expected)) == 0);
}

void check_unary(const mpfrxx::mpc_class& value, unary_c_api c_api, unary_wrapper wrapper)
{
    mpfr_prec_t real_precision = value.real_precision();
    mpfr_prec_t imag_precision = value.imag_precision();
    mpc_t expected;
    mpc_init3(expected, real_precision, imag_precision);
    c_api(expected, value.mpc_data(), mpfrxx::mpc_class::default_rounding());

    mpfrxx::mpc_class got = wrapper(value);
    assert(got.real_precision() == real_precision);
    assert(got.imag_precision() == imag_precision);
    assert_same_mpc_value(got, expected);

    mpc_clear(expected);
}

void check_unary_real(const mpfrxx::mpc_class& value,
                      mpfr_prec_t result_precision,
                      unary_real_c_api c_api,
                      unary_real_wrapper wrapper)
{
    mpfr_t expected;
    mpfr_init2(expected, result_precision);
    c_api(expected, value.mpc_data(), mpfrxx::default_rounding_mode());

    mpfrxx::mpfr_class got = wrapper(value);
    assert(got.precision() == result_precision);
    assert(mpfr_cmp(got.mpfr_data(), expected) == 0);

    mpfr_clear(expected);
}

void check_binary(const mpfrxx::mpc_class& lhs,
                  const mpfrxx::mpc_class& rhs,
                  binary_c_api c_api,
                  mpfrxx::mpc_class (*wrapper)(const mpfrxx::mpc_class&, const mpfrxx::mpc_class&))
{
    const mpfr_prec_t real_precision = std::max(lhs.real_precision(), rhs.real_precision());
    const mpfr_prec_t imag_precision = std::max(lhs.imag_precision(), rhs.imag_precision());
    mpc_t expected;
    mpc_init3(expected, real_precision, imag_precision);
    c_api(expected, lhs.mpc_data(), rhs.mpc_data(), mpfrxx::mpc_class::default_rounding());

    mpfrxx::mpc_class got = wrapper(lhs, rhs);
    assert(got.real_precision() == real_precision);
    assert(got.imag_precision() == imag_precision);
    assert_same_mpc_value(got, expected);

    mpc_clear(expected);
}

void check_ternary(const mpfrxx::mpc_class& a,
                   const mpfrxx::mpc_class& b,
                   const mpfrxx::mpc_class& c,
                   ternary_c_api c_api,
                   mpfrxx::mpc_class (*wrapper)(const mpfrxx::mpc_class&,
                                                const mpfrxx::mpc_class&,
                                                const mpfrxx::mpc_class&))
{
    const mpfr_prec_t real_precision = std::max({a.real_precision(),
                                                 b.real_precision(),
                                                 c.real_precision()});
    const mpfr_prec_t imag_precision = std::max({a.imag_precision(),
                                                 b.imag_precision(),
                                                 c.imag_precision()});
    mpc_t expected;
    mpc_init3(expected, real_precision, imag_precision);
    c_api(expected, a.mpc_data(), b.mpc_data(), c.mpc_data(), mpfrxx::mpc_class::default_rounding());

    mpfrxx::mpc_class got = wrapper(a, b, c);
    assert(got.real_precision() == real_precision);
    assert(got.imag_precision() == imag_precision);
    assert_same_mpc_value(got, expected);

    mpc_clear(expected);
}

void test_compile_time_surface()
{
    using mpfrxx::mpc_class;
    using mpfrxx::mpfr_class;
    using expr_type = decltype(std::declval<const mpc_class&>() +
                               std::declval<const mpc_class&>());

    static_assert(std::is_same<decltype(mpfrxx::sqrt(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::log10(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cos(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::tan(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sinh(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::cosh(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::tanh(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::asin(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::acos(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atan(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::asinh(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::acosh(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::atanh(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow(std::declval<const mpc_class&>(),
                                                    std::declval<const mpc_class&>())),
                               mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sqr(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::conj(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::proj(std::declval<const mpc_class&>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::real(std::declval<const mpc_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::imag(std::declval<const mpc_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::abs(std::declval<const mpc_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::norm(std::declval<const mpc_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::arg(std::declval<const mpc_class&>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::agm(std::declval<const mpc_class&>(),
                                                    std::declval<const mpc_class&>())),
                               mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fma(std::declval<const mpc_class&>(),
                                                    std::declval<const mpc_class&>(),
                                                    std::declval<const mpc_class&>())),
                               mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin_cos(std::declval<const mpc_class&>())),
                               std::pair<mpc_class, mpc_class>>::value);
    static_assert(std::is_same<decltype(mpfrxx::rootofunity(8ul, 3ul)), mpc_class>::value);

    static_assert(std::is_same<decltype(mpfrxx::sqrt(std::declval<expr_type>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::exp(std::declval<expr_type>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::sin(std::declval<expr_type>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::pow(std::declval<expr_type>(),
                                                    std::declval<const mpc_class&>())),
                               mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::conj(std::declval<expr_type>())), mpc_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::abs(std::declval<expr_type>())), mpfr_class>::value);
    static_assert(std::is_same<decltype(mpfrxx::fma(std::declval<expr_type>(),
                                                    std::declval<const mpc_class&>(),
                                                    std::declval<const mpc_class&>())),
                               mpc_class>::value);
}

void test_unary_functions_against_mpc()
{
    const mpfrxx::mpc_class values[] = {
        mpfrxx::mpc_class::with_precision(192, 160, 0.75, -0.5),
        mpfrxx::mpc_class::with_precision(224, 192, -1.25, 0.375),
        mpfrxx::mpc_class::with_precision(192, 224, 0.125, 1.5),
    };

    for (const mpfrxx::mpc_class& value : values) {
        check_unary(value, mpc_sqrt, mpfrxx::sqrt);
        check_unary(value, mpc_sqr, mpfrxx::sqr);
        check_unary(value, mpc_conj, mpfrxx::conj);
        check_unary(value, mpc_proj, mpfrxx::proj);
        check_unary(value, mpc_exp, mpfrxx::exp);
        check_unary(value, mpc_log, mpfrxx::log);
        check_unary(value, mpc_log10, mpfrxx::log10);
        check_unary(value, mpc_sin, mpfrxx::sin);
        check_unary(value, mpc_cos, mpfrxx::cos);
        check_unary(value, mpc_tan, mpfrxx::tan);
        check_unary(value, mpc_sinh, mpfrxx::sinh);
        check_unary(value, mpc_cosh, mpfrxx::cosh);
        check_unary(value, mpc_tanh, mpfrxx::tanh);
        check_unary(value, mpc_asin, mpfrxx::asin);
        check_unary(value, mpc_acos, mpfrxx::acos);
        check_unary(value, mpc_atan, mpfrxx::atan);
        check_unary(value, mpc_asinh, mpfrxx::asinh);
        check_unary(value, mpc_acosh, mpfrxx::acosh);
        check_unary(value, mpc_atanh, mpfrxx::atanh);

        check_unary_real(value, value.real_precision(), mpc_real, mpfrxx::real);
        check_unary_real(value, value.imag_precision(), mpc_imag, mpfrxx::imag);
        const mpfr_prec_t real_result_precision = std::max(value.real_precision(), value.imag_precision());
        check_unary_real(value, real_result_precision, mpc_abs, mpfrxx::abs);
        check_unary_real(value, real_result_precision, mpc_norm, mpfrxx::norm);
        check_unary_real(value, real_result_precision, mpc_arg, mpfrxx::arg);
    }
}

void test_binary_functions_against_mpc()
{
    const mpfrxx::mpc_class lhs = mpfrxx::mpc_class::with_precision(192, 160, 0.75, -0.5);
    const mpfrxx::mpc_class rhs = mpfrxx::mpc_class::with_precision(224, 192, -0.25, 0.5);
    check_binary(lhs, rhs, mpc_pow, mpfrxx::pow);
    check_binary(lhs, rhs, mpc_agm, mpfrxx::agm);
}

void test_fma_against_mpc()
{
    const mpfrxx::mpc_class a = mpfrxx::mpc_class::with_precision(192, 160, 0.75, -0.5);
    const mpfrxx::mpc_class b = mpfrxx::mpc_class::with_precision(224, 192, -0.25, 0.5);
    const mpfrxx::mpc_class c = mpfrxx::mpc_class::with_precision(160, 224, 0.125, 0.75);
    check_ternary(a, b, c, mpc_fma, mpfrxx::fma);
}

void test_sin_cos_against_mpc()
{
    const mpfrxx::mpc_class value = mpfrxx::mpc_class::with_precision(192, 160, 0.75, -0.5);
    mpc_t expected_sin;
    mpc_t expected_cos;
    mpc_init3(expected_sin, value.real_precision(), value.imag_precision());
    mpc_init3(expected_cos, value.real_precision(), value.imag_precision());
    mpc_sin_cos(expected_sin,
                expected_cos,
                value.mpc_data(),
                mpfrxx::mpc_class::default_rounding(),
                mpfrxx::mpc_class::default_rounding());

    const auto got = mpfrxx::sin_cos(value);
    assert(got.first.real_precision() == value.real_precision());
    assert(got.first.imag_precision() == value.imag_precision());
    assert(got.second.real_precision() == value.real_precision());
    assert(got.second.imag_precision() == value.imag_precision());
    assert_same_mpc_value(got.first, expected_sin);
    assert_same_mpc_value(got.second, expected_cos);

    mpc_clear(expected_sin);
    mpc_clear(expected_cos);
}

void test_rootofunity_against_mpc()
{
    mpc_t expected;
    mpc_init3(expected, 192, 160);
    mpc_rootofunity(expected, 8ul, 3ul, mpfrxx::mpc_class::default_rounding());

    mpfrxx::mpc_class got = mpfrxx::rootofunity(8ul, 3ul, 192, 160);
    assert(got.real_precision() == 192);
    assert(got.imag_precision() == 160);
    assert_same_mpc_value(got, expected);
    mpc_clear(expected);
}

void test_expression_inputs()
{
    const mpfrxx::mpc_class a = mpfrxx::mpc_class::with_precision(192, 160, 0.75, -0.5);
    const mpfrxx::mpc_class b = mpfrxx::mpc_class::with_precision(224, 192, -0.25, 0.5);
    const auto expr = a + b;
    const mpfrxx::mpc_class materialized(expr);

    mpfrxx::mpc_class got = mpfrxx::sin(expr);
    mpc_t expected;
    mpc_init3(expected, materialized.real_precision(), materialized.imag_precision());
    mpc_sin(expected, materialized.mpc_data(), mpfrxx::mpc_class::default_rounding());
    assert_same_mpc_value(got, expected);
    mpc_clear(expected);

    got = mpfrxx::pow(expr, b);
    const mpfr_prec_t real_precision = std::max(materialized.real_precision(), b.real_precision());
    const mpfr_prec_t imag_precision = std::max(materialized.imag_precision(), b.imag_precision());
    mpc_init3(expected, real_precision, imag_precision);
    mpc_pow(expected, materialized.mpc_data(), b.mpc_data(), mpfrxx::mpc_class::default_rounding());
    assert(got.real_precision() == real_precision);
    assert(got.imag_precision() == imag_precision);
    assert_same_mpc_value(got, expected);
    mpc_clear(expected);

    mpfrxx::mpfr_class got_abs = mpfrxx::abs(expr);
    const mpfr_prec_t real_result_precision = std::max(materialized.real_precision(), materialized.imag_precision());
    mpfr_t expected_real;
    mpfr_init2(expected_real, real_result_precision);
    mpc_abs(expected_real, materialized.mpc_data(), mpfrxx::default_rounding_mode());
    assert(got_abs.precision() == real_result_precision);
    assert(mpfr_cmp(got_abs.mpfr_data(), expected_real) == 0);
    mpfr_clear(expected_real);

    got = mpfrxx::fma(expr, b, a);
    const mpfr_prec_t fma_real_precision = std::max({materialized.real_precision(), b.real_precision(), a.real_precision()});
    const mpfr_prec_t fma_imag_precision = std::max({materialized.imag_precision(), b.imag_precision(), a.imag_precision()});
    mpc_init3(expected, fma_real_precision, fma_imag_precision);
    mpc_fma(expected, materialized.mpc_data(), b.mpc_data(), a.mpc_data(), mpfrxx::mpc_class::default_rounding());
    assert(got.real_precision() == fma_real_precision);
    assert(got.imag_precision() == fma_imag_precision);
    assert_same_mpc_value(got, expected);
    mpc_clear(expected);
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_unary_functions_against_mpc();
    test_binary_functions_against_mpc();
    test_fma_against_mpc();
    test_sin_cos_against_mpc();
    test_rootofunity_against_mpc();
    test_expression_inputs();
    return 0;
}
