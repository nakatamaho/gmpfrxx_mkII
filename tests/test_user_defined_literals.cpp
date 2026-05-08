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

#include <cassert>
#include <cstdint>

namespace {

void assert_mpf_equal(const gmpxx::mpf_class& lhs, const gmpxx::mpf_class& rhs)
{
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

void test_mpz_literals()
{
    using namespace gmpxx::literals;

    gmpxx::mpz_class a = 42_mpz;
    gmpxx::mpz_class b = "123456789012345678901234567890"_mpz;
    gmpxx::mpz_class c = 0x123456789abcdef0123456789abcdef0123_mpz;

    assert(a == gmpxx::mpz_class(std::int64_t{42}));
    assert(b == gmpxx::mpz_class("123456789012345678901234567890", 10));
    assert(c == gmpxx::mpz_class("123456789abcdef0123456789abcdef0123", 16));
}

void test_mpq_literals()
{
    using namespace gmpxx::literals;

    gmpxx::mpq_class a = 42_mpq;
    gmpxx::mpq_class b = "2/4"_mpq;
    gmpxx::mpq_class c = 0x10_mpq;

    assert(a == gmpxx::mpq_class(std::int64_t{42}));
    assert(b == gmpxx::mpq_class("1/2", 10));
    assert(c == gmpxx::mpq_class(std::int64_t{16}));
}

void test_mpf_literals()
{
    using namespace gmpxx::literals;

    gmpxx::mpf_class a = 1.25_mpf;
    gmpxx::mpf_class b = "1.25"_mpf;
    gmpxx::mpf_class c = 0.5_mpf;
    gmpxx::mpf_class d = "3.75"_mpf;
    gmpxx::mpf_class e = 112.5e-1_mpf;

    assert_mpf_equal(a, b);
    assert_mpf_equal(c, gmpxx::mpf_class(0.5, c.get_prec()));
    assert_mpf_equal(d, gmpxx::mpf_class(3.75, d.get_prec()));
    assert_mpf_equal(e, gmpxx::mpf_class(11.25, e.get_prec()));
}

void test_mpfc_imaginary_literals()
{
    using namespace gmpxx::literals;

    gmpxx::mpfc_class imag = 2.5_mpfc_i;
    assert(imag.real() == 0);
    assert_mpf_equal(imag.imag(), gmpxx::mpf_class(2.5, imag.imag().get_prec()));
    assert(imag.real_precision() == gmpxx::default_mpf_precision_bits());
    assert(imag.imag_precision() == gmpxx::default_mpf_precision_bits());

    gmpxx::mpfc_class string_imag = "1.75"_mpfc_i;
    assert(string_imag.real() == 0);
    assert_mpf_equal(string_imag.imag(),
                     gmpxx::mpf_class("1.75", string_imag.imag().get_prec(), 10));

    gmpxx::mpfc_class hex_imag = "0xff"_mpfc_i;
    assert(hex_imag.real() == 0);
    assert_mpf_equal(hex_imag.imag(),
                     gmpxx::mpf_class("255", hex_imag.imag().get_prec(), 10));

    gmpxx::mpfc_class hex_fraction_imag = "0x1.8"_mpfc_i;
    assert(hex_fraction_imag.real() == 0);
    assert_mpf_equal(hex_fraction_imag.imag(),
                     gmpxx::mpf_class("1.8", hex_fraction_imag.imag().get_prec(), 16));

    gmpxx::mpfc_class z = gmpxx::mpf_class("1.25", 192) + "2.5"_mpfc_i;
    assert_mpf_equal(z.real(), gmpxx::mpf_class("1.25", z.real().get_prec()));
    assert_mpf_equal(z.imag(), gmpxx::mpf_class("2.5", z.imag().get_prec()));

    gmpxx::mpfc_class negative_imag = -3.25_mpfc_i;
    assert(negative_imag.real() == 0);
    assert_mpf_equal(negative_imag.imag(),
                     gmpxx::mpf_class("-3.25", negative_imag.imag().get_prec()));

    gmpxx::mpfc_class tiny_imag = -9.765625e-4_mpfc_i;
    assert(tiny_imag.real() == 0);
    assert_mpf_equal(tiny_imag.imag(),
                     gmpxx::mpf_class("-0.0009765625", tiny_imag.imag().get_prec()));

    gmpxx::mpfc_class large_imag = 5.36870912e+8_mpfc_i;
    assert(large_imag.real() == 0);
    assert_mpf_equal(large_imag.imag(),
                     gmpxx::mpf_class("536870912", large_imag.imag().get_prec()));

    gmpxx::mpfc_class string_tiny = "-9.765625e-4"_mpfc_i;
    assert(string_tiny.real() == 0);
    assert_mpf_equal(string_tiny.imag(),
                     gmpxx::mpf_class("-0.0009765625", string_tiny.imag().get_prec()));

    gmpxx::mpfc_class string_large = "5.36870912e+8"_mpfc_i;
    assert(string_large.real() == 0);
    assert_mpf_equal(string_large.imag(),
                     gmpxx::mpf_class("536870912", string_large.imag().get_prec()));

    gmpxx::mpfc_class numeric_decimal = 0.1_mpfc_i;
    gmpxx::mpfc_class string_decimal = "0.1"_mpfc_i;
    assert_mpf_equal(numeric_decimal.imag(),
                     gmpxx::mpf_class(static_cast<double>(0.1L), numeric_decimal.imag().get_prec()));
    assert_mpf_equal(string_decimal.imag(),
                     gmpxx::mpf_class("0.1", string_decimal.imag().get_prec(), 10));
    assert(numeric_decimal.imag() != string_decimal.imag());
}

void test_string_literals_use_auto_base_for_exact_types()
{
    using namespace gmpxx::literals;

    gmpxx::mpz_class z = "0xff"_mpz;
    assert(z == gmpxx::mpz_class(std::int64_t{255}));

    gmpxx::mpq_class q = "0x10/0x20"_mpq;
    assert(q == gmpxx::mpq_class("1/2", 10));

    gmpxx::mpf_class f = "1.5"_mpf;
    assert(f == gmpxx::mpf_class("1.5", f.get_prec(), 10));

    gmpxx::mpz_class raw = 123_mpz;
    assert(raw == gmpxx::mpz_class(std::int64_t{123}));
}

void test_gmpxx_namespace_exposes_literals()
{
    using namespace gmpxx;

    mpz_class z = 123_mpz;
    mpq_class q = -11_mpq;
    mpf_class f = 112.5e-1_mpf;

    assert(z == mpz_class(std::int64_t{123}));
    assert(q == mpq_class(std::int64_t{-11}));
    assert_mpf_equal(f, mpf_class("11.25", f.get_prec()));
}

} // namespace

int main()
{
    test_mpz_literals();
    test_mpq_literals();
    test_mpf_literals();
    test_mpfc_imaginary_literals();
    test_string_literals_use_auto_base_for_exact_types();
    test_gmpxx_namespace_exposes_literals();

    return 0;
}
