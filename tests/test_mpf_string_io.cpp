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
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<mpf_srcptr>()),
                             std::ostream&>);
static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<mpf_ptr>()),
                             std::istream&>);
static_assert(std::is_same_v<decltype(print_mpf(std::declval<std::ostream&>(), std::declval<mpf_srcptr>())),
                             void>);

namespace {

class test_numpunct : public std::numpunct<char> {
public:
    explicit test_numpunct(char decimal_point) : decimal_point_(decimal_point) {}

protected:
    char do_decimal_point() const override { return decimal_point_; }

private:
    char decimal_point_;
};

void assert_equal(const gmpxx::mpf_class& lhs, const gmpxx::mpf_class& rhs)
{
    if (mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    gmpxx::mpf_class lhs("1.5", static_cast<mp_bitcnt_t>(128));
    gmpxx::mpf_class rhs(std::string("2.5"), static_cast<mp_bitcnt_t>(128));

    gmpxx::mpf_class result = lhs + rhs;
    if (result.get_str() != "4") {
        std::abort();
    }

    std::ostringstream out;
    out << result;
    if (out.str() != "4") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.setf(std::ios_base::fixed, std::ios_base::floatfield);
    out.precision(2);
    out << result;
    if (out.str() != "4.00") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::showpos << std::internal << std::setw(8) << std::setfill('_')
        << std::fixed << std::setprecision(2) << result;
    if (out.str() != "+___4.00") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::noshowpos << std::right << std::setfill(' ') << std::fixed << std::setprecision(2)
        << (lhs + rhs);
    if (out.str() != "4.00") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::defaultfloat << std::noshowpos << std::noshowbase << std::nouppercase
        << std::hex << std::showbase << std::setprecision(4) << gmpxx::mpf_class(1.25, 128);
    if (out.str().rfind("0x", 0) != 0) {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::defaultfloat << std::noshowpos << std::noshowbase << std::nouppercase
        << std::hex << std::showbase << std::scientific << std::setprecision(4)
        << gmpxx::mpf_class(1.25, 128);
    if (out.str().rfind("0x", 0) != 0 || out.str().find('@') == std::string::npos) {
        std::abort();
    }

    gmpxx::mpf_class zero("0", 128);
    out.str("");
    out.clear();
    out.precision(0);
    out << std::dec << std::defaultfloat << std::noshowbase << std::nouppercase << std::showpoint << zero;
    if (out.str() != "0.00000") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.precision(0);
    out << std::dec << std::fixed << std::showpoint << zero;
    if (out.str() != "0.") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.precision(0);
    out << std::dec << std::scientific << std::noshowpoint << zero;
    if (out.str() != "0.000000e+00") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.precision(0);
    out << std::defaultfloat << std::noshowbase << std::hex << zero;
    if (out.str() != "0") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.precision(0);
    out << std::showbase << std::hex << zero;
    if (out.str() != "0x0") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.precision(2);
    out << std::showbase << std::fixed << std::oct << gmpxx::mpf_class(".03125", 128);
    if (out.str() != "00.02") {
        std::abort();
    }

    out.str("");
    out.clear();
    out.precision(2);
    out << std::showbase << std::fixed << std::oct << gmpxx::mpf_class(".001953125", 128);
    if (out.str() != "0.00") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::dec << std::defaultfloat << std::noshowbase << std::noshowpos << result.get_mpf_t();
    if (out.str() != "4") {
        std::abort();
    }

    out.str("");
    out.clear();
    print_mpf(out << std::showpos, result.get_mpf_t());
    if (out.str() != "+4") {
        std::abort();
    }

    result.set("-0.25");
    if (result.get_str() != "-0.25") {
        std::abort();
    }

    mp_exp_t exponent = 0;
    if (result.get_str(exponent, 10, 0) != "-25" || exponent != 0) {
        std::abort();
    }

    const std::string before = result.get_str();
    if (result.set_str("not-a-number") == 0) {
        std::abort();
    }
    if (result.get_str() != before) {
        std::abort();
    }

    if (result.set_str("ff", 16) != 0) {
        std::abort();
    }
    if (result.get_str() != "255") {
        std::abort();
    }
    result = "123";
    const std::string decimal_assignment_before = result.get_str();
    bool hex_assignment_threw = false;
    try {
        result = "0x10";
    } catch (const std::invalid_argument&) {
        hex_assignment_threw = true;
    }
    if (!hex_assignment_threw || result.get_str() != decimal_assignment_before) {
        std::abort();
    }
    hex_assignment_threw = false;
    try {
        result = std::string("0x10");
    } catch (const std::invalid_argument&) {
        hex_assignment_threw = true;
    }
    if (!hex_assignment_threw || result.get_str() != decimal_assignment_before) {
        std::abort();
    }
    result = std::string("052");
    if (result.to_double() != 52.0) {
        std::abort();
    }

    std::istringstream decimal_input("  -2.5 rest");
    decimal_input >> result;
    if (result.get_str() != "-2.5" || result.precision() != static_cast<mp_bitcnt_t>(128)) {
        std::abort();
    }
    std::string rest;
    decimal_input >> rest;
    if (rest != "rest") {
        std::abort();
    }

    std::istringstream exponent_input("+1.25e2");
    exponent_input >> result;
    if (result.get_str() != "125") {
        std::abort();
    }

    std::istringstream tail_input("1.25tail");
    tail_input >> result;
    if (tail_input.fail() || result.get_str() != "1.25" ||
        tail_input.tellg() != std::streampos(4)) {
        std::abort();
    }

    std::istringstream hex_input("f.f@1");
    hex_input >> std::hex >> result;
    if (result.get_str() != "255") {
        std::abort();
    }

    const std::string stream_before = result.get_str();
    std::istringstream invalid_input("not-a-number");
    invalid_input >> result;
    if (!invalid_input.fail() || result.get_str() != stream_before) {
        std::abort();
    }

    mpf_t raw_f;
    mpf_init2(raw_f, 192);
    mpf_set_ui(raw_f, 7);
    std::istringstream raw_decimal_input("  -2.5 rest");
    raw_decimal_input >> raw_f;
    if (mpf_cmp(raw_f, gmpxx::mpf_class("-2.5", 192).get_mpf_t()) != 0 ||
        mpf_get_prec(raw_f) != 192) {
        std::abort();
    }
    raw_decimal_input >> rest;
    if (rest != "rest") {
        std::abort();
    }

    std::istringstream raw_hex_input("f.f@1");
    raw_hex_input >> std::hex >> raw_f;
    if (mpf_cmp(raw_f, gmpxx::mpf_class("255", 192).get_mpf_t()) != 0 ||
        mpf_get_prec(raw_f) != 192) {
        std::abort();
    }

    std::istringstream invalid_raw_input("not-a-number");
    invalid_raw_input >> raw_f;
    if (!invalid_raw_input.fail() ||
        mpf_cmp(raw_f, gmpxx::mpf_class("255", 192).get_mpf_t()) != 0 ||
        mpf_get_prec(raw_f) != 192) {
        std::abort();
    }
    mpf_clear(raw_f);

    result = gmpxx::mpf_class(9.0, static_cast<mp_bitcnt_t>(128));
    std::istringstream invalid_mantissa_input(".e123");
    invalid_mantissa_input >> result;
    if (!invalid_mantissa_input.fail()) {
        std::abort();
    }
    invalid_mantissa_input.clear();
    if (invalid_mantissa_input.tellg() != std::streampos(1) ||
        result.get_str() != "9") {
        std::abort();
    }

    result = gmpxx::mpf_class(9.0, static_cast<mp_bitcnt_t>(128));
    std::istringstream invalid_exponent_input("123e+");
    invalid_exponent_input >> result;
    if (!invalid_exponent_input.fail()) {
        std::abort();
    }
    invalid_exponent_input.clear();
    if (invalid_exponent_input.tellg() != std::streampos(5) ||
        result.get_str() != "9") {
        std::abort();
    }

    const char decimal_points[] = {'.', ',', 'x'};
    for (char point : decimal_points) {
        std::locale loc(std::locale::classic(), new test_numpunct(point));

        std::string text = std::string("-1") + point + "5e1 rest";
        std::istringstream locale_input(text);
        locale_input.imbue(loc);
        locale_input >> result;
        if (locale_input.fail()) {
            std::abort();
        }
        assert_equal(result, gmpxx::mpf_class(-15.0, result.precision()));
        locale_input >> rest;
        if (rest != "rest") {
            std::abort();
        }

        std::ostringstream locale_output;
        locale_output.imbue(loc);
        locale_output << gmpxx::mpf_class(1.5, static_cast<mp_bitcnt_t>(128));
        if (locale_output.str() != std::string("1") + point + "5") {
            std::abort();
        }
    }

    bool threw = false;
    try {
        gmpxx::mpf_class invalid("not-a-number", static_cast<mp_bitcnt_t>(128));
        (void)invalid;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    return 0;
}
