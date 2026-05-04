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

#include <cassert>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

void assert_equal(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& expected)
{
    assert(got.precision() == expected.precision());
    assert(mpfr_cmp(got.mpfr_data(), expected.mpfr_data()) == 0);
}

void require_string_accessors()
{
    mpfrxx::mpfr_class value("1.25", 192);
    assert(value.get_str() == "1.25");

    mpfr_exp_t exponent = 0;
    assert(value.get_str(exponent, 10, 3) == "125");
    assert(exponent == 1);

    value.set("-0.25");
    assert(value.get_str() == "-0.25");
    assert(value.get_str(exponent, 10, 2) == "-25");
    assert(exponent == 0);

    const std::string before = value.get_str();
    assert(value.set_str("not-a-real") != 0);
    assert(value.get_str() == before);

    bool threw = false;
    try {
        value.set("not-a-real");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    assert(value.get_str() == before);

    mpfrxx::mpfr_class hex_value("ff", 192, 16);
    assert(hex_value.get_str() == "255");
}

void require_stream_output()
{
    const mpfrxx::mpfr_class value("1.25", 192);

    std::ostringstream fixed;
    fixed << std::fixed << std::setprecision(2) << value;
    assert(fixed.str() == "1.25");

    std::ostringstream scientific;
    scientific << std::scientific << std::setprecision(3) << value;
    assert(scientific.str() == "1.250e+00");

    std::ostringstream showpos_internal;
    showpos_internal << std::showpos << std::internal << std::setw(8) << std::setfill('_')
                     << std::fixed << std::setprecision(2) << value;
    assert(showpos_internal.str() == "+___1.25");

    const mpfrxx::mpfr_class a("1.5", 192);
    const mpfrxx::mpfr_class b("2.25", 192);
    std::ostringstream expr_stream;
    expr_stream << std::fixed << std::setprecision(2) << (a + b);
    assert(expr_stream.str() == "3.75");
}

void require_stream_input()
{
    mpfrxx::mpfr_class value("1.25", 192);
    std::istringstream decimal_input("  -2.5 rest");
    decimal_input >> value;
    assert_equal(value, mpfrxx::mpfr_class("-2.5", 192));

    std::string rest;
    decimal_input >> rest;
    assert(rest == "rest");

    std::istringstream exponent_input("+1.25e2");
    exponent_input >> value;
    assert_equal(value, mpfrxx::mpfr_class("125", 192));

    std::istringstream hex_input("f.f@1");
    hex_input >> std::hex >> value;
    assert_equal(value, mpfrxx::mpfr_class("f.f@1", 192, 16));

    const mpfrxx::mpfr_class before(value);
    std::istringstream invalid_input("not-a-real");
    invalid_input >> value;
    assert(invalid_input.fail());
    assert_equal(value, before);
}

} // namespace

int main()
{
    require_string_accessors();
    require_stream_output();
    require_stream_input();
    return 0;
}
