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
#include <cstdint>
#include <limits>

namespace {

void require_equal(const gmpxx::mpf_class& got, mpf_t ref)
{
    if (mpf_cmp(got.get_mpf_t(), ref) != 0) {
        std::abort();
    }
}

void check_mul_2exp(const gmpxx::mpf_class& got, const gmpxx::mpf_class& a, mp_bitcnt_t exponent, bool negative)
{
    mpf_t ref;
    mpf_init2(ref, got.precision());
    mpf_mul_2exp(ref, a.get_mpf_t(), exponent);
    if (negative) {
        mpf_neg(ref, ref);
    }
    require_equal(got, ref);
    mpf_clear(ref);
}

void check_div_2exp(const gmpxx::mpf_class& got, const gmpxx::mpf_class& a, mp_bitcnt_t exponent, bool negative)
{
    mpf_t ref;
    mpf_init2(ref, got.precision());
    mpf_div_2exp(ref, a.get_mpf_t(), exponent);
    if (negative) {
        mpf_neg(ref, ref);
    }
    require_equal(got, ref);
    mpf_clear(ref);
}

} // namespace

int main()
{
    const gmpxx::mpf_class a("3.75", 256);

    check_mul_2exp(a * std::uint64_t{1}, a, 0, false);
    check_mul_2exp(a * std::uint64_t{2}, a, 1, false);
    check_mul_2exp(a * std::uint64_t{8}, a, 3, false);
    check_mul_2exp(std::uint64_t{8} * a, a, 3, false);
    check_div_2exp(a / std::uint64_t{8}, a, 3, false);

    check_mul_2exp(a * std::int64_t{-8}, a, 3, true);
    check_mul_2exp(std::int64_t{-8} * a, a, 3, true);
    check_div_2exp(a / std::int64_t{-8}, a, 3, true);

    check_mul_2exp(a * std::numeric_limits<std::int64_t>::min(), a, 63, true);
    check_div_2exp(a / std::numeric_limits<std::int64_t>::min(), a, 63, true);

    {
        const gmpxx::mpf_class got = a * std::uint64_t{3};
        mpf_t ref;
        mpf_init2(ref, got.precision());
        mpf_mul_ui(ref, a.get_mpf_t(), 3);
        require_equal(got, ref);
        mpf_clear(ref);
    }
    {
        const gmpxx::mpf_class got = std::uint64_t{8} / a;
        mpf_t ref;
        mpf_init2(ref, got.precision());
        mpf_ui_div(ref, 8, a.get_mpf_t());
        require_equal(got, ref);
        mpf_clear(ref);
    }
    {
        auto r = gmpxx::mpf_class::with_precision(128, 1.5);
        const mp_bitcnt_t precision = r.precision();
        mpf_t ref;
        mpf_init2(ref, precision);

        mpf_set(ref, r.get_mpf_t());
        mpf_mul_2exp(ref, ref, 4);
        r *= std::uint64_t{16};
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);

        mpf_set(ref, r.get_mpf_t());
        mpf_div_2exp(ref, ref, 4);
        r /= std::uint64_t{16};
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);

        mpf_set(ref, r.get_mpf_t());
        mpf_mul_2exp(ref, ref, 5);
        r <<= 5;
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);

        mpf_set(ref, r.get_mpf_t());
        mpf_div_2exp(ref, ref, 3);
        r >>= 3;
        if (r.precision() != precision) {
            std::abort();
        }
        require_equal(r, ref);
        mpf_clear(ref);
    }
    return 0;
}
