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

namespace {

class test_numpunct : public std::numpunct<char> {
public:
    explicit test_numpunct(char decimal_point) : decimal_point_(decimal_point) {}

protected:
    char do_decimal_point() const override { return decimal_point_; }

private:
    char decimal_point_;
};

void assert_value(const mpfrxx::mpc_class& value, double real, double imag)
{
    assert(mpfr_cmp_d(mpc_realref(value.mpc_data()), real) == 0);
    assert(mpfr_cmp_d(mpc_imagref(value.mpc_data()), imag) == 0);
}

void test_output()
{
    mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(192, 160, 1.25, -2.5);

    std::ostringstream plain;
    plain << z;
    assert(plain.str() == "(1.25,-2.5)");

    std::ostringstream scientific;
    scientific << std::scientific << std::setprecision(3) << z;
    assert(scientific.str() == "(1.250e+00,-2.500e+00)");

    std::ostringstream flags;
    flags << std::fixed << std::showpos << std::setprecision(2)
          << std::setw(20) << std::setfill('*')
          << mpfrxx::mpc_class::with_precision(192, 160, 1.25, 2.5);
    assert(flags.str() == "(+1.25,+2.50)");
    assert(flags.width() == 0);
}

void test_expression_output()
{
    mpfrxx::mpc_class lhs = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    mpfrxx::mpc_class rhs = mpfrxx::mpc_class::with_precision(160, 3.0, -4.0);

    std::ostringstream out;
    out << lhs + rhs;
    assert(out.str() == "(4,-2)");
}

void test_input()
{
    mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(96, 128);
    const mpfr_prec_t real_prec = z.real_precision();
    const mpfr_prec_t imag_prec = z.imag_precision();

    std::istringstream in(" ( 1.25 , -2.5 ) ");
    in >> z;
    assert(in);
    assert_value(z, 1.25, -2.5);
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
        mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(96, 128, 3.0, 4.0);
        const mpfr_prec_t real_prec = z.real_precision();
        const mpfr_prec_t imag_prec = z.imag_precision();
        std::istringstream in(text);
        in >> z;
        assert(!in);
        assert_value(z, 3.0, 4.0);
        assert(z.real_precision() == real_prec);
        assert(z.imag_precision() == imag_prec);
    }
}

void test_locale_decimal_point()
{
    std::locale comma_locale(std::locale::classic(), new test_numpunct(','));
    mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(192, 160, 1.25, -2.5);

    std::ostringstream out;
    out.imbue(comma_locale);
    out << z;
    assert(out.str() == "(1,25,-2,5)");

    mpfrxx::mpc_class parsed = mpfrxx::mpc_class::with_precision(96, 128);
    std::istringstream in(out.str());
    in.imbue(comma_locale);
    in >> parsed;
    assert(in);
    assert_value(parsed, 1.25, -2.5);
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
