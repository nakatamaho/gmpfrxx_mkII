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
#include <array>
#include <cstdlib>

namespace {

void require_same(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& ref, mpfr_prec_t expected_precision)
{
    if (mpfr_cmp(got.get_mpfr_t(), ref.get_mpfr_t()) != 0 || got.precision() != expected_precision) {
        std::abort();
    }
}

mpfrxx::mpfr_class ref_unary(const mpfrxx::mpfr_class& a, mpfr_prec_t precision, bool negative)
{
    auto ref = mpfrxx::mpfr_class::with_precision(precision);
    if (negative) {
        mpfr_neg(ref.get_mpfr_t(), a.get_mpfr_t(), mpfrxx::default_rounding_mode());
    } else {
        mpfr_set(ref.get_mpfr_t(), a.get_mpfr_t(), mpfrxx::default_rounding_mode());
    }
    return ref;
}

mpfrxx::mpfr_class ref_binary(
    const mpfrxx::mpfr_class& a,
    const mpfrxx::mpfr_class& b,
    mpfr_prec_t precision,
    char op)
{
    auto ref = mpfrxx::mpfr_class::with_precision(precision);
    const mpfr_rnd_t rnd = mpfrxx::default_rounding_mode();
    if (op == '+') {
        mpfr_add(ref.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
    } else if (op == '-') {
        mpfr_sub(ref.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
    } else if (op == '*') {
        mpfr_mul(ref.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
    } else {
        mpfr_div(ref.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
    }
    return ref;
}

void check_set(
    const mpfrxx::mpfr_class& a,
    const mpfrxx::mpfr_class& b,
    const mpfrxx::mpfr_class& c,
    const mpfrxx::mpfr_class& d)
{
    const mpfr_prec_t p2 = std::max(a.precision(), b.precision());
    require_same(mpfrxx::mpfr_class(a + b), ref_binary(a, b, p2, '+'), p2);
    require_same(mpfrxx::mpfr_class(a - b), ref_binary(a, b, p2, '-'), p2);
    require_same(mpfrxx::mpfr_class(a * b), ref_binary(a, b, p2, '*'), p2);
    require_same(mpfrxx::mpfr_class(a / b), ref_binary(a, b, p2, '/'), p2);
    require_same(mpfrxx::mpfr_class(-a), ref_unary(a, a.precision(), true), a.precision());
    require_same(mpfrxx::mpfr_class(+a), ref_unary(a, a.precision(), false), a.precision());

    const mpfr_prec_t p3 = std::max({a.precision(), b.precision(), c.precision()});
    {
        auto tmp = mpfrxx::mpfr_class::with_precision(p3);
        auto ref = mpfrxx::mpfr_class::with_precision(p3);
        const mpfr_rnd_t rnd = mpfrxx::default_rounding_mode();
        mpfr_add(tmp.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
        mpfr_mul(ref.get_mpfr_t(), tmp.get_mpfr_t(), c.get_mpfr_t(), rnd);
        require_same(mpfrxx::mpfr_class((a + b) * c), ref, p3);
    }
    {
        auto left = mpfrxx::mpfr_class::with_precision(p3);
        auto ref = mpfrxx::mpfr_class::with_precision(p3);
        const mpfr_rnd_t rnd = mpfrxx::default_rounding_mode();
        mpfr_sub(left.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
        mpfr_div(ref.get_mpfr_t(), left.get_mpfr_t(), c.get_mpfr_t(), rnd);
        require_same(mpfrxx::mpfr_class((a - b) / c), ref, p3);
    }

    const mpfr_prec_t p4 = std::max({a.precision(), b.precision(), c.precision(), d.precision()});
    {
        auto left = mpfrxx::mpfr_class::with_precision(p4);
        auto right = mpfrxx::mpfr_class::with_precision(p4);
        auto ref = mpfrxx::mpfr_class::with_precision(p4);
        const mpfr_rnd_t rnd = mpfrxx::default_rounding_mode();
        mpfr_add(left.get_mpfr_t(), a.get_mpfr_t(), b.get_mpfr_t(), rnd);
        mpfr_sub(right.get_mpfr_t(), c.get_mpfr_t(), d.get_mpfr_t(), rnd);
        mpfr_mul(ref.get_mpfr_t(), left.get_mpfr_t(), right.get_mpfr_t(), rnd);
        require_same(mpfrxx::mpfr_class((a + b) * (c - d)), ref, p4);
    }
}

} // namespace

int main()
{
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    for (mpfr_prec_t precision : std::array<mpfr_prec_t, 3>{{64, 256, 1024}}) {
        const mpfrxx::mpfr_class zero("0", precision);
        const mpfrxx::mpfr_class one("1", precision);
        const mpfrxx::mpfr_class minus_one("-1", precision);
        const mpfrxx::mpfr_class pi("3.1415926535897932384626433832795028841971", precision);
        check_set(pi, one, minus_one, mpfrxx::mpfr_class("2.5", precision));
        check_set(one, pi, mpfrxx::mpfr_class("1e-100", precision), mpfrxx::mpfr_class("7.25", precision));
        check_set(zero, one, pi, mpfrxx::mpfr_class("4", precision));
    }
    return 0;
}
