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

#include <cstdlib>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

void require_equal(const gmpxx::mpf_class& got, const gmpxx::mpf_class& expected)
{
    if (mpf_cmp(got.mpf_data(), expected.mpf_data()) != 0) {
        std::abort();
    }
}

template <typename Function>
void require_domain_error(Function&& function)
{
    try {
        function();
    } catch (const std::domain_error&) {
        return;
    }
    std::abort();
}

template <typename Function>
void require_invalid_argument(Function&& function)
{
    try {
        function();
    } catch (const std::invalid_argument&) {
        return;
    }
    std::abort();
}

void check_public_types()
{
    using mpf = gmpxx::mpf_class;
    using expr_type = decltype(std::declval<const mpf&>() + std::declval<const mpf&>());

    static_assert(std::is_same_v<decltype(gmpxx::frexp(std::declval<const mpf&>(),
                                                       static_cast<mp_exp_t*>(nullptr))),
                                 mpf>);
    static_assert(std::is_same_v<decltype(gmpxx::ilogb(std::declval<const mpf&>())), mp_exp_t>);
    static_assert(std::is_same_v<decltype(gmpxx::logb(std::declval<const mpf&>())), mpf>);
    static_assert(std::is_same_v<decltype(gmpxx::modf(std::declval<const mpf&>(),
                                                      static_cast<mpf*>(nullptr))),
                                 mpf>);

    static_assert(std::is_same_v<decltype(gmpxx::frexp(std::declval<expr_type>(),
                                                       static_cast<mp_exp_t*>(nullptr))),
                                 mpf>);
    static_assert(std::is_same_v<decltype(gmpxx::ilogb(std::declval<expr_type>())), mp_exp_t>);
    static_assert(std::is_same_v<decltype(gmpxx::logb(std::declval<expr_type>())), mpf>);
    static_assert(std::is_same_v<decltype(gmpxx::modf(std::declval<expr_type>(),
                                                      static_cast<mpf*>(nullptr))),
                                 mpf>);
}

void check_frexp_and_logb()
{
    const gmpxx::mpf_class value("13.5", 256);
    mp_exp_t exponent = 0;
    const gmpxx::mpf_class fraction = gmpxx::frexp(value, &exponent);
    if (exponent != 4 || fraction.precision() != value.precision()) {
        std::abort();
    }

    gmpxx::mpf_class expected_fraction(value);
    expected_fraction.div_2exp(4);
    require_equal(fraction, expected_fraction);
    require_equal(gmpxx::ldexp(fraction, static_cast<long>(exponent)), value);

    if (gmpxx::ilogb(value) != 3) {
        std::abort();
    }
    const gmpxx::mpf_class logb_value = gmpxx::logb(value);
    if (logb_value.precision() != value.precision()) {
        std::abort();
    }
    require_equal(logb_value, gmpxx::mpf_class(3, value.precision()));

    const gmpxx::mpf_class small("0.125", 192);
    exponent = 0;
    const gmpxx::mpf_class small_fraction = gmpxx::frexp(small, &exponent);
    if (exponent != -2 || gmpxx::ilogb(small) != -3) {
        std::abort();
    }
    require_equal(small_fraction, gmpxx::mpf_class("0.5", small.precision()));

    const gmpxx::mpf_class negative("-13.5", 256);
    exponent = 0;
    const gmpxx::mpf_class negative_fraction = gmpxx::frexp(negative, &exponent);
    if (exponent != 4) {
        std::abort();
    }
    gmpxx::mpf_class expected_negative_fraction(negative);
    expected_negative_fraction.div_2exp(4);
    require_equal(negative_fraction, expected_negative_fraction);
}

void check_expression_operands()
{
    const gmpxx::mpf_class value("13.5", 256);
    mp_exp_t exponent = 0;
    const gmpxx::mpf_class fraction = gmpxx::frexp(value + value, &exponent);
    if (exponent != 5 || gmpxx::ilogb(value + value) != 4) {
        std::abort();
    }
    require_equal(gmpxx::ldexp(fraction, static_cast<long>(exponent)), gmpxx::mpf_class(value + value));
    require_equal(gmpxx::logb(value + value), gmpxx::mpf_class(4, value.precision()));
}

void check_modf()
{
    const gmpxx::mpf_class value("-13.75", 256);
    gmpxx::mpf_class integer_part = gmpxx::mpf_class::with_precision(128);
    const mp_bitcnt_t integer_precision = integer_part.precision();
    const gmpxx::mpf_class fractional = gmpxx::modf(value, &integer_part);

    if (fractional.precision() != value.precision() || integer_part.precision() != integer_precision) {
        std::abort();
    }
    require_equal(integer_part, gmpxx::mpf_class(-13, integer_precision));
    require_equal(fractional, gmpxx::mpf_class("-0.75", value.precision()));

    gmpxx::mpf_class expr_integer = gmpxx::mpf_class::with_precision(160);
    const gmpxx::mpf_class expr_fractional = gmpxx::modf(value + gmpxx::mpf_class("0.5", 256), &expr_integer);
    require_equal(expr_integer, gmpxx::mpf_class(-13, expr_integer.precision()));
    require_equal(expr_fractional, gmpxx::mpf_class("-0.25", expr_fractional.precision()));
}

void check_zero_and_error_cases()
{
    const gmpxx::mpf_class zero(0, 128);
    mp_exp_t exponent = 123;
    const gmpxx::mpf_class zero_fraction = gmpxx::frexp(zero, &exponent);
    if (exponent != 0 || zero_fraction.precision() != zero.precision() || mpf_sgn(zero_fraction.mpf_data()) != 0) {
        std::abort();
    }

    gmpxx::mpf_class integer_part = gmpxx::mpf_class::with_precision(128);
    const gmpxx::mpf_class fractional = gmpxx::modf(zero, &integer_part);
    if (mpf_sgn(fractional.mpf_data()) != 0 || mpf_sgn(integer_part.mpf_data()) != 0) {
        std::abort();
    }

    require_domain_error([&] { (void)gmpxx::ilogb(zero); });
    require_domain_error([&] { (void)gmpxx::logb(zero); });
    require_invalid_argument([&] { (void)gmpxx::frexp(zero, nullptr); });
    require_invalid_argument([&] { (void)gmpxx::modf(zero, nullptr); });
}

} // namespace

int main()
{
    check_public_types();
    check_frexp_and_logb();
    check_expression_operands();
    check_modf();
    check_zero_and_error_cases();
    return 0;
}
