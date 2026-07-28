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
 */

#include <gmpfrxx_mkII.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <utility>

namespace {

template <typename T>
void require(bool condition, const T& message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::abort();
    }
}

void verify_same_family_spike_expressions()
{
    using mpfr_expr = decltype(
        std::declval<mpfrxx::mpfr_class&>() *
            std::declval<mpfrxx::mpfr_class&>() +
        std::declval<mpfrxx::mpfr_class&>());
    using mpc_expr = decltype(
        std::declval<mpfrxx::mpc_class&>() *
            std::declval<mpfrxx::mpc_class&>() +
        std::declval<mpfrxx::mpc_class&>());
    using mpf_expr = decltype(
        std::declval<gmpxx::mpf_class&>() *
            std::declval<gmpxx::mpf_class&>() +
        std::declval<gmpxx::mpf_class&>());
    using mpfc_expr = decltype(
        std::declval<gmpxx::mpfc_class&>() *
            std::declval<gmpxx::mpfc_class&>() +
        std::declval<gmpxx::mpfc_class&>());
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<mpfr_expr>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<mpc_expr>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<mpf_expr>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<mpfc_expr>);

    mpfrxx::mpfr_class a = 2.0;
    mpfrxx::mpfr_class b = 3.0;
    const mpfrxx::mpfr_class real_result = a * b + sqrt(a);
    mpfrxx::mpc_class z(a, b);
    const mpfrxx::mpc_class complex_result = z * z + z;

    gmpxx::mpf_class f = 2.0;
    gmpxx::mpf_class g = 3.0;
    const gmpxx::mpf_class gmp_result = f * g + f;
    gmpxx::mpfc_class c(f, g);
    const gmpxx::mpfc_class gmp_complex_result = c * c + c;

    require(real_result > a, "same-family MPFR arithmetic failed");
    require(mpfrxx::real(complex_result) != a, "same-family MPC arithmetic failed");
    require(gmp_result > f, "same-family MPF arithmetic failed");
    require(gmpxx::real(gmp_complex_result) != f, "same-family MPFC arithmetic failed");
}

void verify_generic_utility_primitives()
{
    mpfrxx::mpfr_class value = 2.0;
    const mpfrxx::mpfr_class root = sqrt(value);
    const mpfrxx::mpfr_class square = value * value;
    const mpfrxx::mpfr_class signed_value = -mpfrxx::abs(value);
    const mpfrxx::mpfr_class nearest = mpfrxx::floor(value + 0.5);
    const mpfrxx::mpfr_class circle = mpfrxx::const_pi();
    const mpfrxx::mpc_class complex_value(value, root);
    const mpfrxx::mpfr_class magnitude = abs(complex_value);

    std::ostringstream decimal;
    std::ostringstream hexadecimal;
    decimal << std::setprecision(40) << magnitude;
    hexadecimal << std::hexfloat << magnitude;
    require(square > value && signed_value < 0 && nearest == 2 && circle > value,
            "MPFR generic utility primitives failed");
    require(!decimal.str().empty() && !hexadecimal.str().empty(),
            "MPFR stream output failed");

    gmpxx::mpf_class gmp_value = 2.0;
    const gmpxx::mpf_class gmp_square = gmp_value * gmp_value;
    const gmpxx::mpf_class gmp_signed_value = -gmpxx::abs(gmp_value);
    const gmpxx::mpf_class gmp_nearest = gmpxx::floor(gmp_value + 0.5);
    const gmpxx::mpf_class gmp_circle = gmpxx::const_pi();
    std::ostringstream gmp_decimal;
    gmp_decimal << std::setprecision(40) << gmp_circle;
    require(gmp_square > gmp_value && gmp_signed_value < 0 &&
                gmp_nearest == 2 && gmp_circle > gmp_value,
            "GMP generic utility primitives failed");
    require(!gmp_decimal.str().empty(), "GMP stream output failed");
}

void verify_temporary_expression_lifetime()
{
    const mpfrxx::mpfr_class result =
        (mpfrxx::mpfr_class(1.0) + mpfrxx::mpfr_class(2.0)) *
        (mpfrxx::mpfr_class(3.0) + mpfrxx::mpfr_class(4.0));
    const mpfrxx::mpc_class complex_result =
        (mpfrxx::mpc_class(1.0, 2.0) + mpfrxx::mpc_class(3.0, 4.0)) *
        mpfrxx::mpc_class(2.0, 0.0);
    require(result == 21.0, "temporary MPFR expression materialization failed");
    require(mpfrxx::real(complex_result) == 8.0 &&
                mpfrxx::imag(complex_result) == 12.0,
            "temporary MPC expression materialization failed");
}

void verify_gmp_pow2_no_binary64_fallback()
{
    gmpxx::mpf_class low = 1;
    low.div_2exp(200);
    const gmpxx::mpf_class value = gmpxx::mpf_class(1) + low;

    const gmpxx::mpf_class square = value * value;
    const gmpxx::mpf_class residual = square - 1;
    gmpxx::mpf_class upper_bound = low;
    upper_bound.mul_2exp(2);
    require(residual > low && residual < upper_bound,
            "GMP pow2 residual was not retained below binary64");

    // Deliberately emulate the historical binary64 fallback as a negative reference.
    const double binary64_value = mpf_get_d(value.mpf_data());
    const double binary64_residual = binary64_value * binary64_value - 1.0;
    require(binary64_residual == 0.0,
            "binary64 negative reference unexpectedly retained the residual");

    std::cout << "P2C GMP pow2 residual: 2^-200 < residual < 2^-198; "
                 "emulated binary64 residual: 0\n";
}

} // namespace

int main()
{
    verify_same_family_spike_expressions();
    verify_generic_utility_primitives();
    verify_temporary_expression_lifetime();
    verify_gmp_pow2_no_binary64_fallback();
    return 0;
}
