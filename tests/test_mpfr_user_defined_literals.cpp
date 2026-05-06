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

#include <cassert>

namespace {

void assert_mpfr_equal(const mpfrxx::mpfr_class& lhs, const mpfrxx::mpfr_class& rhs)
{
    assert(mpfr_equal_p(lhs.mpfr_data(), rhs.mpfr_data()) != 0);
}

void assert_mpc_equal(const mpfrxx::mpc_class& value,
                      const mpfrxx::mpfr_class& expected_real,
                      const mpfrxx::mpfr_class& expected_imag)
{
    assert(mpfr_equal_p(mpc_realref(value.mpc_data()), expected_real.mpfr_data()) != 0);
    assert(mpfr_equal_p(mpc_imagref(value.mpc_data()), expected_imag.mpfr_data()) != 0);
}

void test_mpfr_literals()
{
    using namespace mpfrxx::literals;

    mpfrxx::mpfr_class a = 1.25_mpfr;
    mpfrxx::mpfr_class b = "1.25"_mpfr;
    mpfrxx::mpfr_class c = 0.5_mpfr;
    mpfrxx::mpfr_class d = "3.75"_mpfr;
    mpfrxx::mpfr_class e = 112.5e-1_mpfr;

    assert_mpfr_equal(a, b);
    assert_mpfr_equal(c, mpfrxx::mpfr_class(0.5, c.precision()));
    assert_mpfr_equal(d, mpfrxx::mpfr_class("3.75", d.precision(), 10));
    assert_mpfr_equal(e, mpfrxx::mpfr_class(11.25, e.precision()));
}

void test_mpc_imaginary_literals()
{
    using namespace mpfrxx::literals;

    mpfrxx::mpc_class imag = 2.5_mpc_i;
    assert_mpc_equal(imag,
                     mpfrxx::mpfr_class("0", imag.real_precision()),
                     mpfrxx::mpfr_class(2.5, imag.imag_precision()));
    assert(imag.real_precision() == mpfrxx::default_mpc_real_precision_bits());
    assert(imag.imag_precision() == mpfrxx::default_mpc_imag_precision_bits());

    mpfrxx::mpc_class string_imag = "0x1.8"_mpc_i;
    assert_mpc_equal(string_imag,
                     mpfrxx::mpfr_class("0", string_imag.real_precision()),
                     mpfrxx::mpfr_class("1.8", string_imag.imag_precision(), 16));

    mpfrxx::mpc_class z = "1.25"_mpfr + "2.5"_mpc_i;
    assert_mpc_equal(z,
                     mpfrxx::mpfr_class("1.25", z.real_precision()),
                     mpfrxx::mpfr_class("2.5", z.imag_precision()));

    mpfrxx::mpc_class negative_imag = -3.25_mpc_i;
    assert_mpc_equal(negative_imag,
                     mpfrxx::mpfr_class("0", negative_imag.real_precision()),
                     mpfrxx::mpfr_class("-3.25", negative_imag.imag_precision()));

    mpfrxx::mpc_class tiny_imag = -9.765625e-4_mpc_i;
    assert_mpc_equal(tiny_imag,
                     mpfrxx::mpfr_class("0", tiny_imag.real_precision()),
                     mpfrxx::mpfr_class("-0.0009765625", tiny_imag.imag_precision()));

    mpfrxx::mpc_class large_imag = 5.36870912e+8_mpc_i;
    assert_mpc_equal(large_imag,
                     mpfrxx::mpfr_class("0", large_imag.real_precision()),
                     mpfrxx::mpfr_class("536870912", large_imag.imag_precision()));

    mpfrxx::mpc_class string_tiny = "-9.765625e-4"_mpc_i;
    assert_mpc_equal(string_tiny,
                     mpfrxx::mpfr_class("0", string_tiny.real_precision()),
                     mpfrxx::mpfr_class("-0.0009765625", string_tiny.imag_precision()));

    mpfrxx::mpc_class string_large = "5.36870912e+8"_mpc_i;
    assert_mpc_equal(string_large,
                     mpfrxx::mpfr_class("0", string_large.real_precision()),
                     mpfrxx::mpfr_class("536870912", string_large.imag_precision()));
}

void test_string_literals_use_auto_base()
{
    using namespace mpfrxx::literals;

    mpfrxx::mpfr_class hex = "0x10"_mpfr;
    mpfrxx::mpfr_class binary = "0b101.1"_mpfr;
    mpfrxx::mpfr_class decimal = "1.5"_mpfr;

    assert_mpfr_equal(hex, mpfrxx::mpfr_class("10", hex.precision(), 16));
    assert_mpfr_equal(binary, mpfrxx::mpfr_class("101.1", binary.precision(), 2));
    assert_mpfr_equal(decimal, mpfrxx::mpfr_class("1.5", decimal.precision(), 10));
}

void test_literal_precision_uses_wrapper_default()
{
    using namespace mpfrxx::literals;

    mpfrxx::mpfr_class decimal = "2.5"_mpfr;
    mpfrxx::mpfr_class floating = 2.5_mpfr;

    assert(decimal.precision() == mpfrxx::mpfr_class::default_precision());
    assert(floating.precision() == mpfrxx::mpfr_class::default_precision());
}

void test_mpfrxx_namespace_exposes_literals()
{
    using namespace mpfrxx;

    mpfr_class real = 112.5e-1_mpfr;
    mpfr_class string_real = "0x1.8"_mpfr;
    mpc_class imag = -9.765625e-4_mpc_i;

    assert_mpfr_equal(real, mpfr_class(11.25, real.precision()));
    assert_mpfr_equal(string_real, mpfr_class("1.8", string_real.precision(), 16));
    assert_mpc_equal(imag,
                     mpfr_class("0", imag.real_precision()),
                     mpfr_class("-0.0009765625", imag.imag_precision()));
}

} // namespace

int main()
{
    test_mpfr_literals();
    test_mpc_imaginary_literals();
    test_string_literals_use_auto_base();
    test_literal_precision_uses_wrapper_default();
    test_mpfrxx_namespace_exposes_literals();

    return 0;
}
