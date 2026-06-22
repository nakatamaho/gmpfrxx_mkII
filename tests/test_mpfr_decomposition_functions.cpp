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

#include <cstdlib>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

void require_equal(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& expected)
{
    if (mpfr_cmp(got.mpfr_data(), expected.mpfr_data()) != 0) {
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
    using mpfr = mpfrxx::mpfr_class;
    using expr_type = decltype(std::declval<const mpfr&>() + std::declval<const mpfr&>());

    static_assert(std::is_same_v<decltype(mpfrxx::frexp(std::declval<const mpfr&>(),
                                                        static_cast<mpfr_exp_t*>(nullptr))),
                                 mpfr>);
    static_assert(std::is_same_v<decltype(mpfrxx::ilogb(std::declval<const mpfr&>())), mpfr_exp_t>);
    static_assert(std::is_same_v<decltype(mpfrxx::logb(std::declval<const mpfr&>())), mpfr>);
    static_assert(std::is_same_v<decltype(mpfrxx::modf(std::declval<const mpfr&>(),
                                                       static_cast<mpfr*>(nullptr))),
                                 mpfr>);

    static_assert(std::is_same_v<decltype(mpfrxx::frexp(std::declval<expr_type>(),
                                                        static_cast<mpfr_exp_t*>(nullptr))),
                                 mpfr>);
    static_assert(std::is_same_v<decltype(mpfrxx::ilogb(std::declval<expr_type>())), mpfr_exp_t>);
    static_assert(std::is_same_v<decltype(mpfrxx::logb(std::declval<expr_type>())), mpfr>);
    static_assert(std::is_same_v<decltype(mpfrxx::modf(std::declval<expr_type>(),
                                                       static_cast<mpfr*>(nullptr))),
                                 mpfr>);
}

void check_frexp_and_logb()
{
    const mpfrxx::mpfr_class value("13.5", 256);
    mpfr_exp_t exponent = 0;
    const mpfrxx::mpfr_class fraction = mpfrxx::frexp(value, &exponent);
    if (exponent != 4 || fraction.precision() != value.precision()) {
        std::abort();
    }

    auto expected_fraction = mpfrxx::mpfr_class::with_precision(value.precision());
    mpfr_exp_t expected_exponent = 0;
    mpfr_frexp(&expected_exponent,
               expected_fraction.mpfr_data(),
               value.mpfr_data(),
               mpfrxx::mpfr_class::default_rounding());
    if (expected_exponent != exponent) {
        std::abort();
    }
    require_equal(fraction, expected_fraction);
    require_equal(mpfrxx::ldexp(fraction, static_cast<long>(exponent)), value);

    if (mpfrxx::ilogb(value) != 3) {
        std::abort();
    }
    const mpfrxx::mpfr_class logb_value = mpfrxx::logb(value);
    if (logb_value.precision() != value.precision()) {
        std::abort();
    }
    require_equal(logb_value, mpfrxx::mpfr_class(3, value.precision()));

    const mpfrxx::mpfr_class small("0.125", 192);
    exponent = 0;
    const mpfrxx::mpfr_class small_fraction = mpfrxx::frexp(small, &exponent);
    if (exponent != -2 || mpfrxx::ilogb(small) != -3) {
        std::abort();
    }
    require_equal(small_fraction, mpfrxx::mpfr_class("0.5", small.precision()));

    const mpfrxx::mpfr_class negative("-13.5", 256);
    exponent = 0;
    const mpfrxx::mpfr_class negative_fraction = mpfrxx::frexp(negative, &exponent);
    if (exponent != 4) {
        std::abort();
    }
    auto expected_negative_fraction = mpfrxx::mpfr_class::with_precision(negative.precision());
    mpfr_frexp(&expected_exponent,
               expected_negative_fraction.mpfr_data(),
               negative.mpfr_data(),
               mpfrxx::mpfr_class::default_rounding());
    require_equal(negative_fraction, expected_negative_fraction);
}

void check_expression_operands()
{
    const mpfrxx::mpfr_class value("13.5", 256);
    mpfr_exp_t exponent = 0;
    const mpfrxx::mpfr_class fraction = mpfrxx::frexp(value + value, &exponent);
    if (exponent != 5 || mpfrxx::ilogb(value + value) != 4) {
        std::abort();
    }
    require_equal(mpfrxx::ldexp(fraction, static_cast<long>(exponent)), mpfrxx::mpfr_class(value + value));
    require_equal(mpfrxx::logb(value + value), mpfrxx::mpfr_class(4, value.precision()));
}

void check_modf()
{
    const mpfrxx::mpfr_class value("-13.75", 256);
    mpfrxx::mpfr_class integer_part = mpfrxx::mpfr_class::with_precision(128);
    const mpfr_prec_t integer_precision = integer_part.precision();
    const mpfrxx::mpfr_class fractional = mpfrxx::modf(value, &integer_part);

    if (fractional.precision() != value.precision() || integer_part.precision() != integer_precision) {
        std::abort();
    }

    auto expected_integer = mpfrxx::mpfr_class::with_precision(integer_precision);
    auto expected_fractional = mpfrxx::mpfr_class::with_precision(value.precision());
    mpfr_modf(expected_integer.mpfr_data(),
              expected_fractional.mpfr_data(),
              value.mpfr_data(),
              mpfrxx::mpfr_class::default_rounding());
    require_equal(integer_part, expected_integer);
    require_equal(fractional, expected_fractional);

    mpfrxx::mpfr_class expr_integer = mpfrxx::mpfr_class::with_precision(160);
    const mpfrxx::mpfr_class expr_fractional = mpfrxx::modf(value + mpfrxx::mpfr_class("0.5", 256), &expr_integer);
    require_equal(expr_integer, mpfrxx::mpfr_class(-13, expr_integer.precision()));
    require_equal(expr_fractional, mpfrxx::mpfr_class("-0.25", expr_fractional.precision()));
}

void check_zero_and_error_cases()
{
    const mpfrxx::mpfr_class zero(0, 128);
    mpfr_exp_t exponent = 123;
    const mpfrxx::mpfr_class zero_fraction = mpfrxx::frexp(zero, &exponent);
    if (exponent != 0 || zero_fraction.precision() != zero.precision() || mpfr_zero_p(zero_fraction.mpfr_data()) == 0) {
        std::abort();
    }

    mpfrxx::mpfr_class integer_part = mpfrxx::mpfr_class::with_precision(128);
    const mpfrxx::mpfr_class fractional = mpfrxx::modf(zero, &integer_part);
    if (mpfr_zero_p(fractional.mpfr_data()) == 0 || mpfr_zero_p(integer_part.mpfr_data()) == 0) {
        std::abort();
    }

    require_domain_error([&] { (void)mpfrxx::ilogb(zero); });
    require_domain_error([&] { (void)mpfrxx::logb(zero); });
    require_invalid_argument([&] { (void)mpfrxx::frexp(zero, nullptr); });
    require_invalid_argument([&] { (void)mpfrxx::modf(zero, nullptr); });

    mpfrxx::mpfr_class nan_value = mpfrxx::mpfr_class::with_precision(128);
    mpfr_set_nan(nan_value.mpfr_data());
    require_domain_error([&] { (void)mpfrxx::ilogb(nan_value); });
    require_domain_error([&] { (void)mpfrxx::logb(nan_value); });
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
