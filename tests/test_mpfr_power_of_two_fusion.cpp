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
#include <cstdint>
#include <limits>
#include <type_traits>

namespace {

void require_equal(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& ref)
{
    if (mpfr_cmp(got.get_mpfr_t(), ref.get_mpfr_t()) != 0) {
        std::abort();
    }
}

void check_mul_2ui(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& a, unsigned long exponent, bool negative)
{
    auto ref = mpfrxx::mpfr_class::with_precision(got.precision());
    mpfr_mul_2ui(ref.get_mpfr_t(), a.get_mpfr_t(), exponent, mpfrxx::default_rounding_mode());
    if (negative) {
        mpfr_neg(ref.get_mpfr_t(), ref.get_mpfr_t(), mpfrxx::default_rounding_mode());
    }
    require_equal(got, ref);
}

void check_div_2ui(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& a, unsigned long exponent, bool negative)
{
    auto ref = mpfrxx::mpfr_class::with_precision(got.precision());
    mpfr_div_2ui(ref.get_mpfr_t(), a.get_mpfr_t(), exponent, mpfrxx::default_rounding_mode());
    if (negative) {
        mpfr_neg(ref.get_mpfr_t(), ref.get_mpfr_t(), mpfrxx::default_rounding_mode());
    }
    require_equal(got, ref);
}

} // namespace

int main()
{
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    const mpfrxx::mpfr_class a("3.75", 256);

    check_mul_2ui(a * std::uint64_t{1}, a, 0, false);
    check_mul_2ui(a * std::uint64_t{2}, a, 1, false);
    check_mul_2ui(a * std::uint64_t{8}, a, 3, false);
    check_mul_2ui(std::uint64_t{8} * a, a, 3, false);
    check_div_2ui(a / std::uint64_t{8}, a, 3, false);

    check_mul_2ui(a * std::int64_t{-8}, a, 3, true);
    check_mul_2ui(std::int64_t{-8} * a, a, 3, true);
    check_div_2ui(a / std::int64_t{-8}, a, 3, true);

    {
        auto shl_expr = a << 4;
        auto shr_expr = a >> 3;
        static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(shl_expr)>);
        static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(shr_expr)>);
        static_assert(!std::is_same_v<decltype(shl_expr), mpfrxx::mpfr_class>);
        static_assert(!std::is_same_v<decltype(shr_expr), mpfrxx::mpfr_class>);
        check_mul_2ui(shl_expr, a, 4, false);
        check_div_2ui(shr_expr, a, 3, false);
        check_div_2ui((-a) >> 2, a, 2, true);
    }

    check_mul_2ui(a * std::numeric_limits<std::int64_t>::min(), a, 63, true);
    check_div_2ui(a / std::numeric_limits<std::int64_t>::min(), a, 63, true);

    {
        const mpfrxx::mpfr_class got = std::uint64_t{8} / a;
        auto ref = mpfrxx::mpfr_class::with_precision(got.precision());
        mpfr_ui_div(ref.get_mpfr_t(), 8, a.get_mpfr_t(), mpfrxx::default_rounding_mode());
        require_equal(got, ref);
    }
    {
        auto r = mpfrxx::mpfr_class::with_precision(128, 1.5);
        const mpfr_prec_t precision = r.precision();
        auto ref = mpfrxx::mpfr_class::with_precision(precision);

        mpfr_set(ref.get_mpfr_t(), r.get_mpfr_t(), mpfrxx::default_rounding_mode());
        mpfr_mul_2ui(ref.get_mpfr_t(), ref.get_mpfr_t(), 4, mpfrxx::default_rounding_mode());
        r *= std::uint64_t{16};
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);

        mpfr_set(ref.get_mpfr_t(), r.get_mpfr_t(), mpfrxx::default_rounding_mode());
        mpfr_div_2ui(ref.get_mpfr_t(), ref.get_mpfr_t(), 4, mpfrxx::default_rounding_mode());
        r /= std::uint64_t{16};
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);

        mpfr_set(ref.get_mpfr_t(), r.get_mpfr_t(), mpfrxx::default_rounding_mode());
        mpfr_mul_2ui(ref.get_mpfr_t(), ref.get_mpfr_t(), 5, mpfrxx::default_rounding_mode());
        r <<= 5;
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);

        mpfr_set(ref.get_mpfr_t(), r.get_mpfr_t(), mpfrxx::default_rounding_mode());
        mpfr_div_2ui(ref.get_mpfr_t(), ref.get_mpfr_t(), 3, mpfrxx::default_rounding_mode());
        r >>= 3;
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);
    }
    return 0;
}
