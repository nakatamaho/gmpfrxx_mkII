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
#include <array>
#include <cstdlib>

namespace {

void require_same(const gmpxx::mpf_class& got, const gmpxx::mpf_class& ref, mp_bitcnt_t expected_precision)
{
    if (mpf_cmp(got.get_mpf_t(), ref.get_mpf_t()) != 0 || got.precision() != expected_precision) {
        std::abort();
    }
}

gmpxx::mpf_class ref_unary(const gmpxx::mpf_class& a, mp_bitcnt_t precision, bool negative)
{
    auto ref = gmpxx::mpf_class::with_precision(precision);
    if (negative) {
        mpf_neg(ref.get_mpf_t(), a.get_mpf_t());
    } else {
        mpf_set(ref.get_mpf_t(), a.get_mpf_t());
    }
    return ref;
}

gmpxx::mpf_class ref_binary(
    const gmpxx::mpf_class& a,
    const gmpxx::mpf_class& b,
    mp_bitcnt_t precision,
    char op)
{
    auto ref = gmpxx::mpf_class::with_precision(precision);
    if (op == '+') {
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    } else if (op == '-') {
        mpf_sub(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    } else if (op == '*') {
        mpf_mul(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    } else {
        mpf_div(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
    }
    return ref;
}

void check_set(
    const gmpxx::mpf_class& a,
    const gmpxx::mpf_class& b,
    const gmpxx::mpf_class& c,
    const gmpxx::mpf_class& d)
{
    const mp_bitcnt_t p2 = std::max(a.precision(), b.precision());
    require_same(gmpxx::mpf_class(a + b), ref_binary(a, b, p2, '+'), p2);
    require_same(gmpxx::mpf_class(a - b), ref_binary(a, b, p2, '-'), p2);
    require_same(gmpxx::mpf_class(a * b), ref_binary(a, b, p2, '*'), p2);
    require_same(gmpxx::mpf_class(a / b), ref_binary(a, b, p2, '/'), p2);
    require_same(gmpxx::mpf_class(-a), ref_unary(a, a.precision(), true), a.precision());
    require_same(gmpxx::mpf_class(+a), ref_unary(a, a.precision(), false), a.precision());

    const mp_bitcnt_t p3 = std::max({a.precision(), b.precision(), c.precision()});
    {
        auto tmp = gmpxx::mpf_class::with_precision(p3);
        auto ref = gmpxx::mpf_class::with_precision(p3);
        mpf_add(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), tmp.get_mpf_t(), c.get_mpf_t());
        require_same(gmpxx::mpf_class((a + b) * c), ref, p3);
    }
    {
        auto tmp = gmpxx::mpf_class::with_precision(p3);
        auto ref = gmpxx::mpf_class::with_precision(p3);
        mpf_sub(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_div(ref.get_mpf_t(), tmp.get_mpf_t(), c.get_mpf_t());
        require_same(gmpxx::mpf_class((a - b) / c), ref, p3);
    }
    {
        auto tmp = gmpxx::mpf_class::with_precision(p3);
        auto ref = gmpxx::mpf_class::with_precision(p3);
        mpf_sub(tmp.get_mpf_t(), b.get_mpf_t(), c.get_mpf_t());
        mpf_add(ref.get_mpf_t(), a.get_mpf_t(), tmp.get_mpf_t());
        require_same(gmpxx::mpf_class(a + (b - c)), ref, p3);
    }

    const mp_bitcnt_t p4 = std::max({a.precision(), b.precision(), c.precision(), d.precision()});
    {
        auto left = gmpxx::mpf_class::with_precision(p4);
        auto right = gmpxx::mpf_class::with_precision(p4);
        auto ref = gmpxx::mpf_class::with_precision(p4);
        mpf_add(left.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_sub(right.get_mpf_t(), c.get_mpf_t(), d.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), left.get_mpf_t(), right.get_mpf_t());
        require_same(gmpxx::mpf_class((a + b) * (c - d)), ref, p4);
    }
}

void check_assignment_preserves_destination_precision(
    const gmpxx::mpf_class& a,
    const gmpxx::mpf_class& b,
    const gmpxx::mpf_class& c)
{
    constexpr mp_bitcnt_t destination_precision = 128;
    {
        auto dst = gmpxx::mpf_class::with_precision(destination_precision);
        auto ref = ref_binary(a, b, destination_precision, '+');
        dst = a + b;
        require_same(dst, ref, destination_precision);
    }
    {
        auto dst = gmpxx::mpf_class::with_precision(destination_precision);
        auto tmp = gmpxx::mpf_class::with_precision(destination_precision);
        auto ref = gmpxx::mpf_class::with_precision(destination_precision);
        mpf_add(tmp.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());
        mpf_mul(ref.get_mpf_t(), tmp.get_mpf_t(), c.get_mpf_t());
        dst = (a + b) * c;
        require_same(dst, ref, destination_precision);
    }
}

} // namespace

int main()
{
    for (mp_bitcnt_t precision : std::array<mp_bitcnt_t, 3>{{64, 256, 1024}}) {
        const gmpxx::mpf_class zero("0", precision);
        const gmpxx::mpf_class one("1", precision);
        const gmpxx::mpf_class minus_one("-1", precision);
        const gmpxx::mpf_class pi("3.1415926535897932384626433832795028841971", precision);
        check_set(pi, one, minus_one, gmpxx::mpf_class("2.5", precision));
        check_set(one, pi, gmpxx::mpf_class("1e-100", precision), gmpxx::mpf_class("7.25", precision));
        check_set(gmpxx::mpf_class(1.25, precision),
                  gmpxx::mpf_class("-2.75", precision),
                  gmpxx::mpf_class("1e+100", precision),
                  gmpxx::mpf_class("9.5", precision));
        check_set(zero, one, pi, gmpxx::mpf_class("4", precision));
    }

    check_set(gmpxx::mpf_class("3.5", 64),
              gmpxx::mpf_class("-7.25", 256),
              gmpxx::mpf_class("1e-50", 1024),
              gmpxx::mpf_class("11.125", 256));
    check_assignment_preserves_destination_precision(
        gmpxx::mpf_class("3.5", 64),
        gmpxx::mpf_class("-7.25", 256),
        gmpxx::mpf_class("1e-50", 1024));
    return 0;
}
