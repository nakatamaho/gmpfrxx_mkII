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
#include <iomanip>
#include <locale>
#include <sstream>

namespace {

class test_numpunct : public std::numpunct<char> {
public:
    explicit test_numpunct(char decimal_point) : decimal_point_(decimal_point) {}

protected:
    char do_decimal_point() const override { return decimal_point_; }

private:
    char decimal_point_;
};

void test_output()
{
    gmpxx::mpfc_class z(gmpxx::mpf_class("1.25", 192), gmpxx::mpf_class("-2.5", 160));

    std::ostringstream plain;
    plain << z;
    assert(plain.str() == "(1.25,-2.5)");

    std::ostringstream scientific;
    scientific << std::scientific << std::setprecision(3) << z;
    assert(scientific.str() == "(1.250e+00,-2.500e+00)");

    std::ostringstream flags;
    flags << std::fixed << std::showpos << std::setprecision(2)
          << std::setw(20) << std::setfill('*') << gmpxx::mpfc_class(gmpxx::mpf_class("1.25", 192),
                                                                       gmpxx::mpf_class("2.5", 160));
    assert(flags.str() == "(+1.25,+2.50)");
    assert(flags.width() == 0);
}

void test_expression_output()
{
    gmpxx::mpfc_class lhs(gmpxx::mpf_class(1, 160), gmpxx::mpf_class(2, 160));
    gmpxx::mpfc_class rhs(gmpxx::mpf_class(3, 160), gmpxx::mpf_class(-4, 160));

    std::ostringstream out;
    out << lhs + rhs;
    assert(out.str() == "(4,-2)");
}

void test_input()
{
    gmpxx::mpfc_class z(gmpxx::mpf_class(0, 96), gmpxx::mpf_class(0, 128));
    const mp_bitcnt_t real_prec = z.real_precision();
    const mp_bitcnt_t imag_prec = z.imag_precision();

    std::istringstream in(" ( 1.25 , -2.5 ) ");
    in >> z;
    assert(in);
    assert(z.real() == gmpxx::mpf_class("1.25", real_prec));
    assert(z.imag() == gmpxx::mpf_class("-2.5", imag_prec));
    assert(z.real_precision() == real_prec);
    assert(z.imag_precision() == imag_prec);
}

void test_failed_input_preserves_value()
{
    const char* inputs[] = {
        "1.25",
        "(1.25)",
        "(1.25; -2.5)",
        "(1.25,not-a-number)",
        "(1.25,-2.5]",
        "(1.25,-2.5",
    };

    for (const char* text : inputs) {
        gmpxx::mpfc_class z(gmpxx::mpf_class(3, 96), gmpxx::mpf_class(4, 128));
        const mp_bitcnt_t real_prec = z.real_precision();
        const mp_bitcnt_t imag_prec = z.imag_precision();
        std::istringstream in(text);
        in >> z;
        assert(!in);
        assert(z.real() == 3);
        assert(z.imag() == 4);
        assert(z.real_precision() == real_prec);
        assert(z.imag_precision() == imag_prec);
    }
}

void test_locale_decimal_point()
{
    std::locale comma_locale(std::locale::classic(), new test_numpunct(','));
    gmpxx::mpfc_class z(gmpxx::mpf_class("1.25", 192), gmpxx::mpf_class("-2.5", 160));

    std::ostringstream out;
    out.imbue(comma_locale);
    out << z;
    assert(out.str() == "(1,25,-2,5)");

    gmpxx::mpfc_class parsed(gmpxx::mpf_class(0, 96), gmpxx::mpf_class(0, 128));
    std::istringstream in(out.str());
    in.imbue(comma_locale);
    in >> parsed;
    assert(in);
    assert(parsed.real() == gmpxx::mpf_class("1.25", parsed.real_precision()));
    assert(parsed.imag() == gmpxx::mpf_class("-2.5", parsed.imag_precision()));
}

} // namespace

int main()
{
    test_output();
    test_expression_output();
    test_input();
    test_failed_input_preserves_value();
    test_locale_decimal_point();
    return 0;
}
