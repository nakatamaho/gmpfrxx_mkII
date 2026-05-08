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
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace {

class test_numpunct : public std::numpunct<char> {
public:
    explicit test_numpunct(char decimal_point) : decimal_point_(decimal_point) {}

protected:
    char do_decimal_point() const override { return decimal_point_; }

private:
    char decimal_point_;
};

static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<mpfr_ptr>()),
                             std::istream&>);
static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<mpfr_srcptr>()),
                             std::ostream&>);

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
    mpfrxx::mpfr_class hex_constructor("0x1p+5", 192);
    assert(hex_constructor.to_double() == 32.0);
    mpfrxx::mpfr_class hex_string_constructor(std::string("0x1.8p+4"), 192);
    assert(hex_string_constructor.to_double() == 24.0);

    value = "0x1p+5";
    assert(value.to_double() == 32.0);
    value = std::string("052");
    assert(value.to_double() == 52.0);

    mpfrxx::mpfr_class grouped("0.33333 33333 33333", 192);
    mpfrxx::mpfr_class ungrouped("0.333333333333333", 192);
    assert(mpfr_cmp(grouped.mpfr_data(), ungrouped.mpfr_data()) == 0);

    value.set("1.25 00");
    assert(value.get_str() == "1.25");

    value = "0x1 p+5";
    assert(value.to_double() == 32.0);
    value.set("0x1.8p+4");
    assert(value.to_double() == 24.0);
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

    const mpfrxx::mpfr_class zero("0", 192);
    std::ostringstream showpoint_zero;
    showpoint_zero << std::showpoint << std::setprecision(0) << zero;
    assert(showpoint_zero.str() == "0.00000");

    std::ostringstream hex_internal;
    hex_internal << std::hex << std::showbase << std::internal << std::setw(6)
                 << mpfrxx::mpfr_class("-1", 192);
    assert(hex_internal.str() == "-0x  1");

    std::ostringstream hex_scientific;
    hex_scientific << std::hex << std::scientific << std::showbase << std::setprecision(1)
                   << mpfrxx::mpfr_class("123", 192);
    assert(hex_scientific.str() == "0x7.b@+01");

    std::ostringstream oct_scientific;
    oct_scientific << std::oct << std::scientific << std::showbase << std::setprecision(3)
                   << mpfrxx::mpfr_class("256", 192);
    assert(oct_scientific.str() == "04.000e+02");

    std::ostringstream showpos_internal;
    showpos_internal << std::showpos << std::internal << std::setw(8) << std::setfill('_')
                     << std::fixed << std::setprecision(2) << value;
    assert(showpos_internal.str() == "+___1.25");

    const mpfrxx::mpfr_class a("1.5", 192);
    const mpfrxx::mpfr_class b("2.25", 192);
    std::ostringstream expr_stream;
    expr_stream << std::fixed << std::setprecision(2) << (a + b);
    assert(expr_stream.str() == "3.75");

    std::locale comma_locale(std::locale::classic(), new test_numpunct(','));
    std::ostringstream locale_fixed;
    locale_fixed.imbue(comma_locale);
    locale_fixed << std::fixed << std::setprecision(2) << value;
    assert(locale_fixed.str() == "1,25");

    std::ostringstream locale_expr;
    locale_expr.imbue(comma_locale);
    locale_expr << std::fixed << std::setprecision(2) << (a + b);
    assert(locale_expr.str() == "3,75");
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

    std::istringstream tail_input("1.25tail");
    tail_input >> value;
    assert(!tail_input.fail());
    assert_equal(value, mpfrxx::mpfr_class("1.25", 192));
    assert(tail_input.tellg() == std::streampos(4));

    std::istringstream hex_input("f.f@1");
    hex_input >> std::hex >> value;
    assert_equal(value, mpfrxx::mpfr_class("f.f@1", 192, 16));

    const mpfrxx::mpfr_class before(value);
    std::istringstream invalid_input("not-a-real");
    invalid_input >> value;
    assert(invalid_input.fail());
    assert_equal(value, before);

    std::locale comma_locale(std::locale::classic(), new test_numpunct(','));
    std::istringstream locale_input(" -2,75 rest");
    locale_input.imbue(comma_locale);
    locale_input >> value;
    assert_equal(value, mpfrxx::mpfr_class("-2.75", 192));

    locale_input >> rest;
    assert(rest == "rest");

    value = mpfrxx::mpfr_class(9.0, 192);
    std::istringstream invalid_mantissa_input(".e123");
    invalid_mantissa_input >> value;
    assert(invalid_mantissa_input.fail());
    invalid_mantissa_input.clear();
    assert(invalid_mantissa_input.tellg() == std::streampos(1));
    assert_equal(value, mpfrxx::mpfr_class(9.0, 192));

    value = mpfrxx::mpfr_class(9.0, 192);
    std::istringstream invalid_exponent_input("123e+");
    invalid_exponent_input >> value;
    assert(invalid_exponent_input.fail());
    invalid_exponent_input.clear();
    assert(invalid_exponent_input.tellg() == std::streampos(5));
    assert_equal(value, mpfrxx::mpfr_class(9.0, 192));

    const char decimal_points[] = {'.', ',', 'x'};
    for (char point : decimal_points) {
        std::locale loc(std::locale::classic(), new test_numpunct(point));

        std::string text = std::string("-1") + point + "5e1 rest";
        std::istringstream table_input(text);
        table_input.imbue(loc);
        table_input >> value;
        assert(!table_input.fail());
        assert_equal(value, mpfrxx::mpfr_class("-15", 192));
        table_input >> rest;
        assert(rest == "rest");

        std::ostringstream table_output;
        table_output.imbue(loc);
        table_output << mpfrxx::mpfr_class("1.5", 192);
        assert(table_output.str() == std::string("1") + point + "5");
    }
}

void require_raw_mpfr_stream_input()
{
    mpfr_t value;
    mpfr_init2(value, 128);
    mpfr_set_ui(value, 9, MPFR_RNDN);

    std::istringstream input("1.25 rest");
    input >> value;
    assert(!input.fail());
    assert(mpfr_get_prec(value) == 128);
    mpfrxx::mpfr_class expected("1.25", 128);
    assert(mpfr_cmp(value, expected.mpfr_data()) == 0);

    std::string rest;
    input >> rest;
    assert(rest == "rest");

    mpfr_set_ui(value, 9, MPFR_RNDN);
    std::istringstream invalid_input("not-a-real");
    invalid_input >> value;
    assert(invalid_input.fail());
    assert(mpfr_get_prec(value) == 128);
    assert(mpfr_cmp_ui(value, 9) == 0);

    std::istringstream rounding_input("1.1");
    mpfrxx::set_default_rounding_mode(MPFR_RNDU);
    rounding_input >> value;
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    assert(!rounding_input.fail());
    mpfr_t rounded_up;
    mpfr_init2(rounded_up, 128);
    mpfr_set_str(rounded_up, "1.1", 10, MPFR_RNDU);
    assert(mpfr_cmp(value, rounded_up) == 0);
    mpfr_clear(rounded_up);

    mpfr_clear(value);
}

void require_raw_mpfr_stream_output()
{
    mpfr_t value;
    mpfr_init2(value, 128);
    mpfr_set_d(value, 1.5, MPFR_RNDN);

    std::ostringstream output;
    output << value;
    assert(output.str() == "1.5");

    std::ostringstream hex_output;
    hex_output << std::hex << std::showbase << std::internal << std::setw(6) << value;
    assert(hex_output.str() == "0x 1.8");

    std::locale comma_locale(std::locale::classic(), new test_numpunct(','));
    std::ostringstream locale_output;
    locale_output.imbue(comma_locale);
    locale_output << value;
    assert(locale_output.str() == "1,5");

    mpfr_clear(value);
}

} // namespace

int main()
{
    require_string_accessors();
    require_stream_output();
    require_stream_input();
    require_raw_mpfr_stream_input();
    require_raw_mpfr_stream_output();
    return 0;
}
