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

#include <gmpfrxx_mkII.h>

#include <cassert>

namespace {

void assert_mpf_equal(const gmpxx::mpf_class& got, const gmpxx::mpf_class& expected, mp_bitcnt_t expected_precision)
{
    assert(mpf_cmp(got.mpf_data(), expected.mpf_data()) == 0);
    assert(got.precision() == expected_precision);
}

void assert_mpfr_equal(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& expected, mpfr_prec_t expected_precision)
{
    assert(mpfr_cmp(got.mpfr_data(), expected.mpfr_data()) == 0);
    assert(got.precision() == expected_precision);
}

void test_mpf_alias_safety()
{
    {
        gmpxx::mpf_class a("1.5", 256);
        gmpxx::mpf_class b("2.5", 256);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class expected(0.0, precision);
        mpf_add(expected.mpf_data(), a.mpf_data(), b.mpf_data());

        a = a + b;

        assert_mpf_equal(a, expected, precision);
    }
    {
        gmpxx::mpf_class a("3.75", 256);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class expected(0.0, precision);
        mpf_sub(expected.mpf_data(), a.mpf_data(), a.mpf_data());

        a = a - a;

        assert_mpf_equal(a, expected, precision);
    }
    {
        gmpxx::mpf_class a("3.75", 256);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class expected(0.0, precision);
        mpf_mul(expected.mpf_data(), a.mpf_data(), a.mpf_data());

        a = a * a;

        assert_mpf_equal(a, expected, precision);
    }
    {
        gmpxx::mpf_class a("3.75", 256);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class expected(0.0, precision);
        mpf_div(expected.mpf_data(), a.mpf_data(), a.mpf_data());

        a = a / a;

        assert_mpf_equal(a, expected, precision);
    }
    {
        gmpxx::mpf_class a("1.5", 256);
        gmpxx::mpf_class b("2.5", 256);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class tmp(0.0, precision);
        gmpxx::mpf_class expected(0.0, precision);
        mpf_add(tmp.mpf_data(), a.mpf_data(), b.mpf_data());
        mpf_mul(expected.mpf_data(), tmp.mpf_data(), a.mpf_data());

        a = (a + b) * a;

        assert_mpf_equal(a, expected, precision);
    }
    {
        gmpxx::mpf_class a("7.5", 256);
        gmpxx::mpf_class b("2.5", 256);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class lhs(0.0, precision);
        gmpxx::mpf_class rhs(0.0, precision);
        gmpxx::mpf_class expected(0.0, precision);
        mpf_sub(lhs.mpf_data(), a.mpf_data(), b.mpf_data());
        mpf_add(rhs.mpf_data(), a.mpf_data(), b.mpf_data());
        mpf_div(expected.mpf_data(), lhs.mpf_data(), rhs.mpf_data());

        a = (a - b) / (a + b);

        assert_mpf_equal(a, expected, precision);
    }
    {
        gmpxx::mpf_class a("1.5", 64);
        gmpxx::mpf_class b("2.5", 1024);
        const mp_bitcnt_t precision = a.precision();
        gmpxx::mpf_class tmp(0.0, precision);
        gmpxx::mpf_class expected(0.0, precision);
        mpf_add(tmp.mpf_data(), a.mpf_data(), b.mpf_data());
        mpf_mul(expected.mpf_data(), tmp.mpf_data(), a.mpf_data());

        a = (a + b) * a;

        assert_mpf_equal(a, expected, precision);
    }
}

void test_mpfr_alias_safety()
{
    const mpfr_rnd_t rounding = mpfrxx::default_rounding_mode();

    {
        mpfrxx::mpfr_class a("1.5", 256);
        mpfrxx::mpfr_class b("2.5", 256);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_add(expected.mpfr_data(), a.mpfr_data(), b.mpfr_data(), rounding);

        a = a + b;

        assert_mpfr_equal(a, expected, precision);
    }
    {
        mpfrxx::mpfr_class a("3.75", 256);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_sub(expected.mpfr_data(), a.mpfr_data(), a.mpfr_data(), rounding);

        a = a - a;

        assert_mpfr_equal(a, expected, precision);
    }
    {
        mpfrxx::mpfr_class a("3.75", 256);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_mul(expected.mpfr_data(), a.mpfr_data(), a.mpfr_data(), rounding);

        a = a * a;

        assert_mpfr_equal(a, expected, precision);
    }
    {
        mpfrxx::mpfr_class a("3.75", 256);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_div(expected.mpfr_data(), a.mpfr_data(), a.mpfr_data(), rounding);

        a = a / a;

        assert_mpfr_equal(a, expected, precision);
    }
    {
        mpfrxx::mpfr_class a("1.5", 256);
        mpfrxx::mpfr_class b("2.5", 256);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class tmp(0.0, precision);
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_add(tmp.mpfr_data(), a.mpfr_data(), b.mpfr_data(), rounding);
        mpfr_mul(expected.mpfr_data(), tmp.mpfr_data(), a.mpfr_data(), rounding);

        a = (a + b) * a;

        assert_mpfr_equal(a, expected, precision);
    }
    {
        mpfrxx::mpfr_class a("7.5", 256);
        mpfrxx::mpfr_class b("2.5", 256);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class lhs(0.0, precision);
        mpfrxx::mpfr_class rhs(0.0, precision);
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_sub(lhs.mpfr_data(), a.mpfr_data(), b.mpfr_data(), rounding);
        mpfr_add(rhs.mpfr_data(), a.mpfr_data(), b.mpfr_data(), rounding);
        mpfr_div(expected.mpfr_data(), lhs.mpfr_data(), rhs.mpfr_data(), rounding);

        a = (a - b) / (a + b);

        assert_mpfr_equal(a, expected, precision);
    }
    {
        mpfrxx::mpfr_class a("1.5", 64);
        mpfrxx::mpfr_class b("2.5", 1024);
        const mpfr_prec_t precision = a.precision();
        mpfrxx::mpfr_class tmp(0.0, precision);
        mpfrxx::mpfr_class expected(0.0, precision);
        mpfr_add(tmp.mpfr_data(), a.mpfr_data(), b.mpfr_data(), rounding);
        mpfr_mul(expected.mpfr_data(), tmp.mpfr_data(), a.mpfr_data(), rounding);

        a = (a + b) * a;

        assert_mpfr_equal(a, expected, precision);
    }
}

} // namespace

int main()
{
    test_mpf_alias_safety();
    test_mpfr_alias_safety();
    return 0;
}
