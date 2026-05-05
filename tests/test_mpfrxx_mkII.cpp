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

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace {

#if defined(__SIZEOF_INT128__)
namespace helper {

std::string uint128_to_string(__uint128_t value)
{
    if (value == 0) {
        return "0";
    }

    std::string result;
    while (value != 0) {
        const unsigned digit = static_cast<unsigned>(value % 10);
        result.push_back(static_cast<char>('0' + digit));
        value /= 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::string int128_to_string(__int128_t value)
{
    if (value < 0) {
        const __uint128_t magnitude = static_cast<__uint128_t>(-(value + 1)) + 1u;
        return "-" + uint128_to_string(magnitude);
    }
    return uint128_to_string(static_cast<__uint128_t>(value));
}

} // namespace helper
#endif

void require_string_equal(const std::string& actual, const std::string& expected)
{
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Actual  : " << actual << std::endl;
    assert(actual == expected && "string comparison failed");
}

std::string normalize_decimal_string(std::string text)
{
    const auto dot = text.find('.');
    if (dot == std::string::npos) {
        return text;
    }
    while (text.size() > dot + 1 && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    if (text == "-0") {
        return "0";
    }
    return text;
}

std::size_t decimal_digit_count(const std::string& text)
{
    return static_cast<std::size_t>(std::count_if(text.begin(), text.end(), [](unsigned char ch) {
        return std::isdigit(ch) != 0;
    }));
}

void require_mpfr_close(const mpfrxx::mpfr_class& actual, const std::string& expected)
{
    const std::string actual_text =
        expected.find('.') == std::string::npos ? actual.get_str() : actual.to_string(decimal_digit_count(expected));
    require_string_equal(normalize_decimal_string(actual_text), normalize_decimal_string(expected));
}

void require_mpfr_same_string(const mpfrxx::mpfr_class& actual, const mpfrxx::mpfr_class& expected,
                              std::size_t digits = 40)
{
    require_string_equal(actual.to_string(digits), expected.to_string(digits));
}

void require_mpc_close(const mpfrxx::mpc_class& actual, const std::string& real, const std::string& imag)
{
    const mpfrxx::mpfr_class actual_real(mpc_realref(actual.mpc_data()), actual.real_precision());
    const mpfrxx::mpfr_class actual_imag(mpc_imagref(actual.mpc_data()), actual.imag_precision());
    std::ostringstream actual_text;
    actual_text << "(" << actual_real.get_str() << "," << actual_imag.get_str() << ")";
    require_string_equal(normalize_decimal_string(actual_text.str()), "(" + normalize_decimal_string(real) + "," +
                                                               normalize_decimal_string(imag) + ")");
}

void test_exact_aliases()
{
    static_assert(std::is_same_v<mpfrxx::mpz_class, gmpxx::mpz_class>);
    static_assert(std::is_same_v<mpfrxx::mpq_class, gmpxx::mpq_class>);

    mpfrxx::mpz_class z(42);
    mpfrxx::mpq_class q(mpfrxx::mpz_class(3), mpfrxx::mpz_class(4));
    assert(z.get_str() == "42");
    assert(q.get_str() == "3/4");
    std::cout << "test_exact_aliases passed." << std::endl;
}

void test_mpfr_construction_assignment()
{
    mpfrxx::set_default_precision_bits(192);
    mpfrxx::mpfr_class zero;
    assert(zero.precision() == 192);
    require_mpfr_close(zero, "0");

    mpfrxx::mpfr_class a("1.25", 160);
    mpfrxx::mpfr_class b(a);
    assert(b.precision() == 160);
    require_mpfr_close(b, "1.25");

    mpfrxx::mpfr_class c(std::move(b));
    require_mpfr_close(c, "1.25");

    c = "-2.5";
    require_mpfr_close(c, "-2.5");
    c = std::string("3.75");
    require_mpfr_close(c, "3.75");
    std::cout << "test_mpfr_construction_assignment passed." << std::endl;
}

void test_mpfr_arithmetic_comparison()
{
    auto a = mpfrxx::mpfr_class::with_precision(160, 1.5);
    auto b = mpfrxx::mpfr_class::with_precision(224, 2.5);
    auto c = mpfrxx::mpfr_class::with_precision(192, 4.0);

    auto expr = (a + b) * c - 3;
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(expr)>);
    static_assert(std::is_same_v<typename decltype(expr)::result_type, mpfrxx::mpfr_class>);

    mpfrxx::mpfr_class materialized(expr);
    assert(materialized.precision() == 224);
    require_mpfr_close(materialized, "13");

    auto destination = mpfrxx::mpfr_class::with_precision(96);
    destination = expr;
    assert(destination.precision() == 96);
    require_mpfr_close(destination, "13");

    destination += 2;
    require_mpfr_close(destination, "15");
    destination -= mpfrxx::mpfr_class("5", 160);
    require_mpfr_close(destination, "10");
    destination *= 3;
    require_mpfr_close(destination, "30");
    destination /= 6;
    require_mpfr_close(destination, "5");

    assert(destination == mpfrxx::mpfr_class("5", 96));
    assert(destination != a);
    assert(a < b);
    assert(c >= b);
    std::cout << "test_mpfr_arithmetic_comparison passed." << std::endl;
}

void test_mpfr_strings_streams_math()
{
    mpfrxx::mpfr_class value("1.25", 192);
    assert(value.get_str() == "1.25");

    mpfr_exp_t exponent = 0;
    assert(value.get_str(exponent, 10, 3) == "125");
    assert(exponent == 1);

    std::ostringstream out;
    out.setf(std::ios::fixed, std::ios::floatfield);
    out.precision(2);
    out << value;
    assert(out.str() == "1.25");

    std::istringstream in("-2.5 tail");
    in >> value;
    require_mpfr_close(value, "-2.5");
    std::string tail;
    in >> tail;
    assert(tail == "tail");

    mpfrxx::mpfr_class sqrt_value = mpfrxx::mpfr_class::with_precision(192);
    mpfr_sqrt(sqrt_value.mpfr_data(), mpfrxx::mpfr_class("4", 192).mpfr_data(), MPFR_RNDN);
    require_mpfr_close(sqrt_value, "2");

    mpfrxx::mpfr_class exp_value = mpfrxx::mpfr_class::with_precision(192);
    mpfr_exp(exp_value.mpfr_data(), mpfrxx::mpfr_class("1", 192).mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class log_value = mpfrxx::mpfr_class::with_precision(192);
    mpfr_log(log_value.mpfr_data(), exp_value.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(log_value, "1");
    std::cout << "test_mpfr_strings_streams_math passed." << std::endl;
}

void test_mpc_construction_arithmetic()
{
    mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    mpfrxx::mpc_class w = mpfrxx::mpc_class::with_precision(192, 3.0, 4.0);
    mpfrxx::mpfr_class real = mpfrxx::mpfr_class::with_precision(224, 5.0);
    mpfrxx::mpq_class half(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    auto expr = z + w * real - half;
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(expr)>);
    static_assert(std::is_same_v<typename decltype(expr)::result_type, mpfrxx::mpc_class>);

    mpfrxx::mpc_class result(expr);
    assert(result.real_precision() >= 192);
    assert(result.imag_precision() >= 192);
    require_mpc_close(result, "15.5", "22");

    result = -z;
    require_mpc_close(result, "-1", "-2");

    result = result + mpfrxx::mpz_class(3);
    require_mpc_close(result, "2", "-2");
    result = result * mpfrxx::mpc_class::with_precision(160, 0.0, 1.0);
    require_mpc_close(result, "2", "2");
    std::cout << "test_mpc_construction_arithmetic passed." << std::endl;
}

void test_mpc_streams()
{
    mpfrxx::mpc_class value = mpfrxx::mpc_class::with_precision(160, 1.25, -2.5);
    std::ostringstream out;
    out.setf(std::ios::fixed, std::ios::floatfield);
    out.precision(2);
    out << value;
    assert(out.str() == "(1.25,-2.50)");

    std::istringstream in("(3.5,4.25)");
    in >> value;
    assert(!in.fail());
    require_mpc_close(value, "3.5", "4.25");
    std::cout << "test_mpc_streams passed." << std::endl;
}

void testDefaultPrecision()
{
    mpfrxx::set_default_precision_bits(192);
    mpfrxx::mpfr_class value;
    assert(value.get_prec() == 192);
    require_mpfr_close(value, "0");
    std::cout << "testDefaultPrecision passed." << std::endl;
}

void testDefaultConstructor()
{
    mpfrxx::mpfr_class value;
    require_mpfr_close(value, "0");
    std::cout << "testDefaultConstructor passed." << std::endl;
}

void testCopyConstructor()
{
    mpfrxx::mpfr_class value("1.25", 160);
    mpfrxx::mpfr_class copy(value);
    assert(copy.get_prec() == 160);
    require_mpfr_close(copy, "1.25");
    std::cout << "testCopyConstructor passed." << std::endl;
}

void testAssignmentOperator()
{
    mpfrxx::mpfr_class value("1.25", 160);
    mpfrxx::mpfr_class assigned;
    assigned = value;
    assert(assigned.get_prec() == mpfrxx::mpfr_class::default_precision());
    require_mpfr_close(assigned, "1.25");
    std::cout << "testAssignmentOperator passed." << std::endl;
}

void testAssignmentOperator_the_rule_of_five()
{
    mpfrxx::mpfr_class a("123", 160);

    std::cout << "##testing the rule 1 of 5: copy constructor\n";
    mpfrxx::mpfr_class b(a);
    assert(b == a && " test failed");
    std::cout << "##testing the rule 1 of 5: copy constructor test passed.\n" << std::endl;

    std::cout << "##testing the rule 4 of 5: move constructor\n";
    mpfrxx::mpfr_class c(std::move(a));
    assert(c == b && " test failed");
    std::cout << "##testing the rule 4 of 5: move constructor test passed.\n" << std::endl;

    std::cout << "##testing the rule 2 of 5: copy assignment\n";
    mpfrxx::mpfr_class d;
    d = b;
    assert(d == b && " test failed");
    std::cout << "##testing the rule 2 of 5: copy assignment test passed.\n" << std::endl;

    std::cout << "##testing the rule 5 of 5: copy assignment\n";
    mpfrxx::mpfr_class e;
    e = std::move(c);
    assert(e == b);
    std::cout << "##testing the rule 5 of 5: copy assignment test passed.\n" << std::endl;

    require_mpfr_close(b, "123");
    require_mpfr_close(d, "123");
    require_mpfr_close(e, "123");
    std::cout << "testAssignmentOperator_the_rule_of_five passed." << std::endl;
}

void testInitializationAndAssignmentDouble()
{
    mpfrxx::mpfr_class a(3.5, 160);
    mpfrxx::mpfr_class b;
    b = 3.5;
    require_mpfr_close(a, "3.5");
    require_mpfr_close(b, "3.5");
    std::cout << "testInitializationAndAssignmentDouble passed." << std::endl;
}

void testInitializationAndAssignmentString()
{
    mpfrxx::mpfr_class a("1.4142135624", 192);
    mpfrxx::mpfr_class b;
    b = "1.4142135624";
    mpfrxx::mpfr_class hex("ff", 192, 16);
    require_mpfr_close(a, "1.4142135624");
    require_mpfr_close(b, "1.4142135624");
    require_mpfr_close(hex, "255");
    std::cout << "testInitializationAndAssignmentString passed." << std::endl;
}

void testAddition()
{
    mpfrxx::mpfr_class a(1.5, 160);
    mpfrxx::mpfr_class b(2.5, 192);
    mpfrxx::mpfr_class c = a + b;
    assert(c.get_prec() == 192);
    require_mpfr_close(c, "4");
    a += b;
    require_mpfr_close(a, "4");
    std::cout << "testAddition passed." << std::endl;
}

void testMultplication()
{
    mpfrxx::mpfr_class a(2.0, 160);
    mpfrxx::mpfr_class b(3.0, 192);
    mpfrxx::mpfr_class c = a * b;
    require_mpfr_close(c, "6");
    a *= b;
    require_mpfr_close(a, "6");
    std::cout << "testMultplication passed." << std::endl;
}

void testDivision()
{
    mpfrxx::mpfr_class a(6.0, 160);
    mpfrxx::mpfr_class b(2.0, 192);
    mpfrxx::mpfr_class c = a / b;
    require_mpfr_close(c, "3");
    a /= b;
    require_mpfr_close(a, "3");
    std::cout << "testDivision passed." << std::endl;
}

void testSubtraction()
{
    mpfrxx::mpfr_class a(5.0, 160);
    mpfrxx::mpfr_class b(2.0, 192);
    mpfrxx::mpfr_class c = a - b;
    require_mpfr_close(c, "3");
    a -= b;
    require_mpfr_close(a, "3");
    std::cout << "testSubtraction passed." << std::endl;
}

void testComparisonOperators()
{
    mpfrxx::mpfr_class a("1.23", 160);
    mpfrxx::mpfr_class b("4.56", 160);
    mpfrxx::mpfr_class c("1.23", 160);
    assert(a == c);
    assert(a != b);
    assert(a < b);
    assert(b > a);
    assert(a <= c);
    assert(b >= a);
    std::cout << "testComparisonOperators passed." << std::endl;
}

void testSqrt()
{
    mpfrxx::mpfr_class input("4", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(input.get_prec());
    mpfr_sqrt(result.mpfr_data(), input.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "2");
    std::cout << "testSqrt passed." << std::endl;
}

void testNeg()
{
    mpfrxx::mpfr_class a(-3.5, 160);
    mpfrxx::mpfr_class result = -a;
    require_mpfr_close(result, "3.5");
    std::cout << "testNeg passed." << std::endl;
}

void testAbs()
{
    mpfrxx::mpfr_class a(-3.5, 160);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(a.get_prec());
    mpfr_abs(result.mpfr_data(), a.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "3.5");
    std::cout << "testAbs passed." << std::endl;
}

void test_mpfr_class_double_addition()
{
    mpfrxx::mpfr_class a(1.25, 160);
    mpfrxx::mpfr_class b = a + 2.5;
    mpfrxx::mpfr_class c = 2.5 + a;
    require_mpfr_close(b, "3.75");
    require_mpfr_close(c, "3.75");
    std::cout << "test_mpfr_class_double_addition passed." << std::endl;
}

void test_mpfr_class_double_subtraction()
{
    mpfrxx::mpfr_class a(5.25, 160);
    mpfrxx::mpfr_class b = a - 2.5;
    mpfrxx::mpfr_class c = 10.0 - a;
    require_mpfr_close(b, "2.75");
    require_mpfr_close(c, "4.75");
    std::cout << "test_mpfr_class_double_subtraction passed." << std::endl;
}

void test_mpfr_class_double_multiplication()
{
    mpfrxx::mpfr_class a(1.5, 160);
    mpfrxx::mpfr_class b = a * 4.0;
    mpfrxx::mpfr_class c = 4.0 * a;
    require_mpfr_close(b, "6");
    require_mpfr_close(c, "6");
    std::cout << "test_mpfr_class_double_multiplication passed." << std::endl;
}

void test_mpfr_class_double_division()
{
    mpfrxx::mpfr_class a(9.0, 160);
    mpfrxx::mpfr_class b = a / 3.0;
    mpfrxx::mpfr_class c = 18.0 / a;
    require_mpfr_close(b, "3");
    require_mpfr_close(c, "2");
    std::cout << "test_mpfr_class_double_division passed." << std::endl;
}

void testOutputOperator()
{
    std::ostringstream out;
    out << std::fixed;
    out.precision(2);
    out << mpfrxx::mpfr_class("1.25", 160);
    assert(out.str() == "1.25");
    std::cout << "testOutputOperator passed." << std::endl;
}

void testCeilFunction()
{
    mpfrxx::mpfr_class a("3.25", 160);
    mpfrxx::mpfr_class b("-3.25", 160);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(160);
    mpfr_ceil(result.mpfr_data(), a.mpfr_data());
    require_mpfr_close(result, "4");
    mpfr_ceil(result.mpfr_data(), b.mpfr_data());
    require_mpfr_close(result, "-3");
    std::cout << "testCeilFunction passed." << std::endl;
}

void testFloor()
{
    mpfrxx::mpfr_class a("3.75", 160);
    mpfrxx::mpfr_class b("-3.75", 160);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(160);
    mpfr_floor(result.mpfr_data(), a.mpfr_data());
    require_mpfr_close(result, "3");
    mpfr_floor(result.mpfr_data(), b.mpfr_data());
    require_mpfr_close(result, "-4");
    std::cout << "testFloor passed." << std::endl;
}

void testHypot()
{
    mpfrxx::mpfr_class a("3", 192);
    mpfrxx::mpfr_class b("4", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_hypot(result.mpfr_data(), a.mpfr_data(), b.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "5");
    std::cout << "testHypot passed." << std::endl;
}

void testSgn()
{
    assert(mpfr_sgn(mpfrxx::mpfr_class("3", 160).mpfr_data()) > 0);
    assert(mpfr_sgn(mpfrxx::mpfr_class("-3", 160).mpfr_data()) < 0);
    assert(mpfr_sgn(mpfrxx::mpfr_class("0", 160).mpfr_data()) == 0);
    std::cout << "testSgn passed." << std::endl;
}

void test_get_d()
{
    mpfrxx::mpfr_class value("123.5", 160);
    require_string_equal(value.get_str(), "123.5");
    std::cout << "test_get_d passed." << std::endl;
}

void test_get_ui()
{
    mpfrxx::mpfr_class value("123", 160);
    assert(mpfr_get_ui(value.mpfr_data(), MPFR_RNDN) == 123UL);
    std::cout << "test_get_ui passed." << std::endl;
}

void test_get_si()
{
    mpfrxx::mpfr_class positive("123", 160);
    mpfrxx::mpfr_class negative("-123", 160);
    assert(mpfr_get_si(positive.mpfr_data(), MPFR_RNDN) == 123L);
    assert(mpfr_get_si(negative.mpfr_data(), MPFR_RNDN) == -123L);
    std::cout << "test_get_si passed." << std::endl;
}

void test_mpfr_class_constructor_precision()
{
    mpfrxx::mpfr_class x(-1.5, 64);
    mpfrxx::mpfr_class y(3.5, 256);
    mpfrxx::mpfr_class result = x + y;
    assert(result.get_prec() == 256);
    x += y;
    assert(x.get_prec() == 64);

    mpfrxx::mpfr_class copied(y);
    assert(copied.get_prec() == 256);
    mpfrxx::mpfr_class assigned;
    assigned = y;
    assert(assigned.get_prec() == mpfrxx::default_precision_bits());
    std::cout << "test_mpfr_class_constructor_precision passed." << std::endl;
}

void test_mpfr_class_constructor_with_mpfr()
{
    mpfr_t raw;
    mpfr_init2(raw, 224);
    mpfr_set_str(raw, "1.125", 10, MPFR_RNDN);
    mpfrxx::mpfr_class value(raw);
    assert(value.get_prec() == 224);
    require_mpfr_close(value, "1.125");
    mpfr_clear(raw);
    std::cout << "test_mpfr_class_constructor_with_mpfr passed." << std::endl;
}

void test_mpfr_class_constructor_with_mpf()
{
    mpfr_t raw;
    mpfr_init2(raw, 224);
    mpfr_set_str(raw, "-2.25", 10, MPFR_RNDN);
    mpfrxx::mpfr_class value(raw);
    assert(value.get_prec() == 224);
    require_mpfr_close(value, "-2.25");
    mpfr_clear(raw);
    std::cout << "test_mpfr_class_constructor_with_mpf passed." << std::endl;
}

void test_mpfr_class_literal()
{
    mpfrxx::mpfr_class decimal("1.25", 160);
    mpfrxx::mpfr_class hex("10", 160, 16);
    require_mpfr_close(decimal, "1.25");
    require_mpfr_close(hex, "16");
    std::cout << "test_mpfr_class_literal passed." << std::endl;
}

void test_mpfr_class_swap()
{
    mpfrxx::mpfr_class a("1.25", 160);
    mpfrxx::mpfr_class b("2.5", 192);
    a.swap(b);
    assert(a.get_prec() == 192);
    assert(b.get_prec() == 160);
    require_mpfr_close(a, "2.5");
    require_mpfr_close(b, "1.25");
    swap(a, b);
    require_mpfr_close(a, "1.25");
    require_mpfr_close(b, "2.5");
    std::cout << "test_mpfr_class_swap passed." << std::endl;
}

void test_template_cmp()
{
    mpfrxx::mpfr_class value(3.14, 160);
    assert(cmp(value, 3.14) == 0);
    assert(cmp(value, 2.0) > 0);
    assert(cmp(2.0, value) < 0);
    std::cout << "test_template_cmp passed." << std::endl;
}

void test_set_str()
{
    mpfrxx::mpfr_class value;
    assert(value.set_str("123.456", 10) == 0);
    require_mpfr_close(value, "123.456");
    assert(value.set_str("not-a-number", 10) != 0);
    std::cout << "test_set_str passed." << std::endl;
}

void test_mpfr_class_get_str()
{
    mpfrxx::mpfr_class value("1234.56789", 192);
    mpfr_exp_t exponent = 0;
    std::string digits = value.get_str(exponent, 10, 5);
    assert(!digits.empty());
    assert(exponent == 4);
    std::cout << "test_mpfr_class_get_str passed." << std::endl;
}

void test_trunc_function()
{
    mpfrxx::mpfr_class a("3.75", 160);
    mpfrxx::mpfr_class b("-3.75", 160);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(160);
    mpfr_trunc(result.mpfr_data(), a.mpfr_data());
    require_mpfr_close(result, "3");
    mpfr_trunc(result.mpfr_data(), b.mpfr_data());
    require_mpfr_close(result, "-3");
    std::cout << "test_trunc_function passed." << std::endl;
}

void test_fits_sint_p()
{
    mpfrxx::mpfr_class small("123", 160);
    mpfrxx::mpfr_class large(static_cast<double>(INT_MAX), 160);
    large += 1;
    assert(mpfr_fits_sint_p(small.mpfr_data(), MPFR_RNDN) != 0);
    assert(mpfr_fits_sint_p(large.mpfr_data(), MPFR_RNDN) == 0);
    std::cout << "test_fits_sint_p passed." << std::endl;
}

void test_fits_slong_p()
{
    mpfrxx::mpfr_class value("123", 160);
    assert(mpfr_fits_slong_p(value.mpfr_data(), MPFR_RNDN) != 0);
    std::cout << "test_fits_slong_p passed." << std::endl;
}

void test_fits_sshort_p()
{
    mpfrxx::mpfr_class value("123", 160);
    assert(mpfr_fits_sshort_p(value.mpfr_data(), MPFR_RNDN) != 0);
    std::cout << "test_fits_sshort_p passed." << std::endl;
}

void test_fits_uint_p()
{
    mpfrxx::mpfr_class positive("123", 160);
    mpfrxx::mpfr_class negative("-1", 160);
    assert(mpfr_fits_uint_p(positive.mpfr_data(), MPFR_RNDN) != 0);
    assert(mpfr_fits_uint_p(negative.mpfr_data(), MPFR_RNDN) == 0);
    std::cout << "test_fits_uint_p passed." << std::endl;
}

void test_fits_ulong_p()
{
    mpfrxx::mpfr_class value("123", 160);
    assert(mpfr_fits_ulong_p(value.mpfr_data(), MPFR_RNDN) != 0);
    std::cout << "test_fits_ulong_p passed." << std::endl;
}

void test_fits_ushort_p()
{
    mpfrxx::mpfr_class value("123", 160);
    assert(mpfr_fits_ushort_p(value.mpfr_data(), MPFR_RNDN) != 0);
    std::cout << "test_fits_ushort_p passed." << std::endl;
}

void testDefaultConstructor_mpz_class()
{
    mpfrxx::mpz_class value;
    assert(value == 0);
    std::cout << "testDefaultConstructor_mpz_class passed." << std::endl;
}

void testCopyConstructor_mpz_class()
{
    mpfrxx::mpz_class value(123);
    mpfrxx::mpz_class copy(value);
    assert(copy == value);
    std::cout << "testCopyConstructor_mpz_class passed." << std::endl;
}

void testAssignmentOperator_mpz_class()
{
    mpfrxx::mpz_class value(123);
    mpfrxx::mpz_class assigned;
    assigned = value;
    assert(assigned == value);
    std::cout << "testAssignmentOperator_mpz_class passed." << std::endl;
}

void testAssignmentOperator_the_rule_of_five_mpz_class()
{
    mpfrxx::mpz_class a(123);

    std::cout << "##testing the rule 1 of 5: copy constructor\n";
    mpfrxx::mpz_class b(a);
    assert(b == a && " test failed");
    std::cout << "##testing the rule 1 of 5: copy constructor test passed.\n" << std::endl;

    std::cout << "##testing the rule 4 of 5: move constructor\n";
    mpfrxx::mpz_class c(std::move(a));
    assert(c == b && " test failed");
    std::cout << "##testing the rule 4 of 5: move constructor test passed.\n" << std::endl;

    std::cout << "##testing the rule 2 of 5: copy assignment\n";
    mpfrxx::mpz_class d;
    d = b;
    assert(d == b && " test failed");
    std::cout << "##testing the rule 2 of 5: copy assignment test passed.\n" << std::endl;

    std::cout << "##testing the rule 5 of 5: copy assignment\n";
    mpfrxx::mpz_class e;
    e = std::move(c);
    assert(e == b);
    std::cout << "##testing the rule 5 of 5: copy assignment test passed.\n" << std::endl;

    assert(b == 123);
    assert(d == 123);
    assert(e == 123);
    std::cout << "testAssignmentOperator_the_rule_of_five_mpz_class passed." << std::endl;
}

void testInitializationAndAssignmentDouble_mpz_class()
{
    mpfrxx::mpz_class a(123.75);
    mpfrxx::mpz_class b;
    b = 123.75;
    assert(a == 123);
    assert(b == 123);
    std::cout << "testInitializationAndAssignmentDouble_mpz_class passed." << std::endl;
}

void testInitializationAndAssignment_int64_t_uint64_t_mpz_class()
{
    std::int64_t signed_value = std::numeric_limits<std::int64_t>::min();
    std::uint64_t unsigned_value = std::numeric_limits<std::uint64_t>::max();
    mpfrxx::mpz_class a(signed_value);
    mpfrxx::mpz_class b(unsigned_value);
    assert(a.get_str() == "-9223372036854775808");
    assert(b.get_str() == "18446744073709551615");
    std::cout << "testInitializationAndAssignment_int64_t_uint64_t_mpz_class passed." << std::endl;
}

void testInitializationAndAssignment_mpz_class_mpfr_class()
{
    mpfrxx::mpfr_class source("123.75", 160);
    mpfrxx::mpz_class value;
    mpfr_get_z(value.get_mpz_t(), source.mpfr_data(), MPFR_RNDZ);
    mpfrxx::mpz_class assigned;
    mpfr_get_z(assigned.get_mpz_t(), source.mpfr_data(), MPFR_RNDZ);
    assert(value == 123);
    assert(assigned == 123);
    std::cout << "testInitializationAndAssignment_mpz_class_mpfr_class passed." << std::endl;
}

void testInitializationAndAssignment_mpfr_class_mpz_class()
{
    mpfrxx::mpz_class source(123);
    mpfrxx::mpfr_class value = mpfrxx::mpfr_class::with_precision(160);
    mpfr_set_z(value.mpfr_data(), source.get_mpz_t(), MPFR_RNDN);
    mpfrxx::mpfr_class assigned;
    mpfr_set_z(assigned.mpfr_data(), source.get_mpz_t(), MPFR_RNDN);
    require_mpfr_close(value, "123");
    require_mpfr_close(assigned, "123");
    std::cout << "testInitializationAndAssignment_mpfr_class_mpz_class passed." << std::endl;
}

void testInitializationAndAssignmentString_mpz_class()
{
    mpfrxx::mpz_class decimal("123456");
    mpfrxx::mpz_class hex("ff", 16);
    assert(decimal.get_str() == "123456");
    assert(hex.get_str() == "255");
    std::cout << "testInitializationAndAssignmentString_mpz_class passed." << std::endl;
}

void test_template_cmp_mpz_class()
{
    mpfrxx::mpz_class value(123);
    assert(cmp(value, 123) == 0);
    assert(cmp(value, 122) > 0);
    assert(cmp(124, value) > 0);
    std::cout << "test_template_cmp_mpz_class passed." << std::endl;
}

void test_mpz_class_literal()
{
    auto value = gmpxx::literals::operator""_mpz("123456", 6);
    assert(value == 123456);
    std::cout << "test_mpz_class_literal passed." << std::endl;
}

void test_arithmetic_operators_mpz_class()
{
    mpfrxx::mpz_class a(20);
    mpfrxx::mpz_class b(6);
    assert(mpfrxx::mpz_class(a + b) == 26);
    assert(mpfrxx::mpz_class(a - b) == 14);
    assert(mpfrxx::mpz_class(a * b) == 120);
    assert(mpfrxx::mpz_class(a / b) == 3);
    assert(mpfrxx::mpz_class(a % b) == 2);
    a += b;
    assert(a == 26);
    std::cout << "test_arithmetic_operators_mpz_class passed." << std::endl;
}

void testFitsFunctions_mpz_class()
{
    mpfrxx::mpz_class value(123);
    assert(value.fits_sint_p());
    assert(value.fits_uint_p());
    assert(value.fits_slong_p());
    assert(value.fits_ulong_p());
    std::cout << "testFitsFunctions_mpz_class passed." << std::endl;
}

void testAbsFunction_mpz_class()
{
    mpfrxx::mpz_class value(-123);
    assert(abs(value) == 123);
    std::cout << "testAbsFunction_mpz_class passed." << std::endl;
}

void testConversionFunctions_mpz_class()
{
    mpfrxx::mpz_class value(123);
    assert(value.get_si() == 123);
    assert(value.get_ui() == 123U);
    assert(value.get_d() == 123.0);
    std::cout << "testConversionFunctions_mpz_class passed." << std::endl;
}

void testMathFunctions_mpz_class()
{
    assert(sqrt(mpfrxx::mpz_class(144)) == 12);
    assert(gcd(mpfrxx::mpz_class(36), mpfrxx::mpz_class(48)) == 12);
    assert(lcm(mpfrxx::mpz_class(12), mpfrxx::mpz_class(18)) == 36);
    std::cout << "testMathFunctions_mpz_class passed." << std::endl;
}

template <class A, class B>
auto test_func(const A& a, const B& b)
{
    using expression_type = std::decay_t<decltype(a * b)>;
    using result_type = typename expression_type::result_type;
    return result_type(a * b);
}

void test_mpz_class_extention()
{
    mpfrxx::mpz_class f(2), g(1), h(3);
    mpfrxx::mpz_class result;
    result = test_func(f * h, g);
    std::cout << "The result of test_func(f * h, g) is: " << result << std::endl;
    assert(result == 6);

    assert(gmpxx::factorial(mpfrxx::mpz_class(5)) == 120);
    assert(gmpxx::primorial(mpfrxx::mpz_class(5)) == 30);
    assert(gmpxx::fibonacci(mpfrxx::mpz_class(10)) == 55);
    std::cout << "test_mpz_class_extention passed." << std::endl;
}

void test_set_str_mpz_class()
{
    mpfrxx::mpz_class value;
    assert(value.set_str("123456", 10) == 0);
    assert(value == 123456);
    assert(value.set_str("ff", 16) == 0);
    assert(value == 255);
    std::cout << "test_set_str_mpz_class tests passed." << std::endl;
}

void test_factorial_mpz_class()
{
    assert(gmpxx::factorial(mpfrxx::mpz_class(10)) == 3628800);
    std::cout << "test_factorial_mpz_class passed." << std::endl;
}

void test_primorial_mpz_class()
{
    assert(gmpxx::primorial(mpfrxx::mpz_class(7)) == 210);
    std::cout << "test_primorial_mpz_class passed." << std::endl;
}

void test_fibonacci_mpz_class()
{
    assert(gmpxx::fibonacci(mpfrxx::mpz_class(12)) == 144);
    std::cout << "test_fibonacci_mpz_class passed." << std::endl;
}

void test_mpz_class_swap()
{
    mpfrxx::mpz_class a(1);
    mpfrxx::mpz_class b(2);
    a.swap(b);
    assert(a == 2);
    assert(b == 1);
    swap(a, b);
    assert(a == 1);
    assert(b == 2);
    std::cout << "test_mpz_class_swap passed." << std::endl;
}

void testOutputOperator_mpz_class()
{
    std::ostringstream out;
    out << mpfrxx::mpz_class(123456);
    assert(out.str() == "123456");
    std::cout << "testOutputOperator_mpz_class passed." << std::endl;
}

void test_mpz_class_addition()
{
    mpfrxx::mpz_class a(3);
    mpfrxx::mpz_class b(4);
    assert(mpfrxx::mpz_class(a + b) == 7);
    double d = 2.0;
    mpfrxx::mpz_class c(-1);
    assert(mpfrxx::mpz_class(c + d) == 1);
    assert(mpfrxx::mpz_class(d + c) == 1);
    c += d;
    assert(c == 1);
    std::cout << "test_mpz_class_addition passed." << std::endl;
}

void test_mpz_class_subtraction()
{
    mpfrxx::mpz_class a(10);
    mpfrxx::mpz_class b(4);
    assert(mpfrxx::mpz_class(a - b) == 6);
    double d = 2.0;
    mpfrxx::mpz_class c(-1);
    assert(mpfrxx::mpz_class(c - d) == -3);
    assert(mpfrxx::mpz_class(d - c) == 3);
    c -= d;
    assert(c == -3);
    std::cout << "test_mpz_class_subtraction passed." << std::endl;
}

void test_mpz_class_multiplication()
{
    mpfrxx::mpz_class a(10);
    mpfrxx::mpz_class b(4);
    assert(mpfrxx::mpz_class(a * b) == 40);
    double d = -2.0;
    mpfrxx::mpz_class c(3);
    assert(mpfrxx::mpz_class(c * d) == -6);
    assert(mpfrxx::mpz_class(d * c) == -6);
    c *= d;
    assert(c == -6);
    std::cout << "test_mpz_class_multiplication passed." << std::endl;
}

void test_mpz_class_division()
{
    mpfrxx::mpz_class a(10);
    mpfrxx::mpz_class b(4);
    assert(mpfrxx::mpz_class(a / b) == 2);
    double d = -2.0;
    mpfrxx::mpz_class c(7);
    assert(mpfrxx::mpz_class(c / d) == -3);
    assert(mpfrxx::mpz_class(-18.0 / c) == -2);
    c /= d;
    assert(c == -3);
    std::cout << "test_mpz_class_division passed." << std::endl;
}

void test_mpz_class_modulus()
{
    mpfrxx::mpz_class a(10);
    mpfrxx::mpz_class b(4);
    assert(mpfrxx::mpz_class(a % b) == 2);
    std::cout << "test_mpz_class_modulus passed." << std::endl;
}

void testDefaultConstructor_mpq_class()
{
    mpfrxx::mpq_class value;
    assert(value == 0);
    std::cout << "testDefaultConstructor_mpq_class passed." << std::endl;
}

void testCopyConstructor_mpq_class()
{
    mpfrxx::mpq_class value(mpfrxx::mpz_class(1), mpfrxx::mpz_class(3));
    mpfrxx::mpq_class copy(value);
    assert(copy == value);
    std::cout << "testCopyConstructor_mpq_class passed." << std::endl;
}

void testAssignmentOperator_mpq_class()
{
    mpfrxx::mpq_class value(mpfrxx::mpz_class(1), mpfrxx::mpz_class(3));
    mpfrxx::mpq_class assigned;
    assigned = value;
    assert(assigned == value);
    std::cout << "testAssignmentOperator_mpq_class passed." << std::endl;
}

void testInitializationAndAssignmentInt_mpq_class()
{
    mpfrxx::mpq_class value(3);
    mpfrxx::mpq_class assigned;
    assigned = 4;
    assert(value.get_str() == "3");
    assert(assigned.get_str() == "4");
    std::cout << "testInitializationAndAssignmentInt_mpq_class passed." << std::endl;
}

void testAssignmentOperator_the_rule_of_five_mpq_class()
{
    mpfrxx::mpq_class a(mpfrxx::mpz_class(1), mpfrxx::mpz_class(3));

    std::cout << "##testing the rule 1 of 5: copy constructor\n";
    mpfrxx::mpq_class b(a);
    assert(b == a && " test failed");
    std::cout << "##testing the rule 1 of 5: copy constructor test passed.\n" << std::endl;

    std::cout << "##testing the rule 4 of 5: move constructor\n";
    mpfrxx::mpq_class c(std::move(a));
    assert(c == b && " test failed");
    std::cout << "##testing the rule 4 of 5: move constructor test passed.\n" << std::endl;

    std::cout << "##testing the rule 2 of 5: copy assignment\n";
    mpfrxx::mpq_class d;
    d = b;
    assert(d == b && " test failed");
    std::cout << "##testing the rule 2 of 5: copy assignment test passed.\n" << std::endl;

    std::cout << "##testing the rule 5 of 5: copy assignment\n";
    mpfrxx::mpq_class e;
    e = std::move(c);
    assert(e == b);
    std::cout << "##testing the rule 5 of 5: copy assignment test passed.\n" << std::endl;

    assert(b.get_str() == "1/3");
    assert(d.get_str() == "1/3");
    assert(e.get_str() == "1/3");
    std::cout << "testAssignmentOperator_the_rule_of_five_mpq_class passed." << std::endl;
}

void testInitializationAndAssignmentString_mpq_class()
{
    mpfrxx::mpq_class decimal("3/4");
    mpfrxx::mpq_class hex("10/20", 16);
    assert(decimal.get_str() == "3/4");
    assert(hex.get_str() == "1/2");
    std::cout << "testInitializationAndAssignmentString_mpq_class passed." << std::endl;
}

void test_template_cmp_mpq_class()
{
    mpfrxx::mpq_class value(mpfrxx::mpz_class(3), mpfrxx::mpz_class(4));
    assert(cmp(value, mpfrxx::mpq_class("3/4")) == 0);
    assert(cmp(value, mpfrxx::mpq_class("1/2")) > 0);
    assert(cmp(mpfrxx::mpq_class("1"), value) > 0);
    std::cout << "test_template_cmp_mpq_class passed." << std::endl;
}

void test_arithmetic_operators_mpq_class_hardcoded1()
{
    mpfrxx::mpq_class a("1/3");
    mpfrxx::mpq_class b("1/6");
    assert(mpfrxx::mpq_class(a + b).get_str() == "1/2");
    assert(mpfrxx::mpq_class(a - b).get_str() == "1/6");
    assert(mpfrxx::mpq_class(a * b).get_str() == "1/18");
    assert(mpfrxx::mpq_class(a / b).get_str() == "2");
    std::cout << "test_arithmetic_operators_mpq_class_hardcoded1 passed." << std::endl;
}

void test_arithmetic_operators_mpq_class_hardcoded2()
{
    mpfrxx::mpq_class a("3/7");
    mpfrxx::mpq_class b("2/5");
    assert(mpfrxx::mpq_class(a + b).get_str() == "29/35");
    assert(mpfrxx::mpq_class(a - b).get_str() == "1/35");
    assert(mpfrxx::mpq_class(a * b).get_str() == "6/35");
    assert(mpfrxx::mpq_class(a / b).get_str() == "15/14");
    std::cout << "test_arithmetic_operators_mpq_class_hardcoded2 passed." << std::endl;
}

void test_mpq_class_literal()
{
    auto value = gmpxx::literals::operator""_mpq("3/4", 3);
    assert(value.get_str() == "3/4");
    std::cout << "test_mpq_class_literal passed." << std::endl;
}

void test_mpq_class_functions()
{
    mpfrxx::mpq_class value("6/8");
    value.canonicalize();
    assert(value.get_str() == "3/4");
    assert(value.get_num().get_str() == "3");
    assert(value.get_den().get_str() == "4");
    assert(value.get_d() == 0.75);
    mpfrxx::mpq_class other("1/2");
    value.swap(other);
    assert(value.get_str() == "1/2");
    swap(value, other);
    assert(value.get_str() == "3/4");
    std::cout << "test_mpq_class_functions passed." << std::endl;
}

void test_mpz_class_comparison_int()
{
    mpfrxx::mpz_class value(123);
    assert(value == 123);
    assert(value > 122);
    assert(value < 124);
    std::cout << "test_mpz_class_comparison_int tests passed successfully." << std::endl;
}

void testExactAliasConstruction()
{
    test_exact_aliases();
    std::cout << "testExactAliasConstruction passed." << std::endl;
}

void test_mixed_exact_mpfr_promotion()
{
    mpfrxx::mpz_class z(3);
    mpfrxx::mpq_class q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));
    mpfrxx::mpfr_class r(2.5, 192);
    mpfrxx::mpfr_class result = z + q + r;
    require_mpfr_close(result, "6");
    std::cout << "test_mixed_exact_mpfr_promotion passed." << std::endl;
}

void test_mpfr_class_const_pi()
{
    mpfrxx::mpfr_class pi = mpfrxx::mpfr_class::with_precision(192);
    mpfr_const_pi(pi.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_const_pi(expected.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(pi, expected);
    std::cout << "test_mpfr_class_const_pi passed." << std::endl;
}

void test_mpfr_class_const_log2()
{
    mpfrxx::mpfr_class log2 = mpfrxx::mpfr_class::with_precision(192);
    mpfr_const_log2(log2.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_const_log2(expected.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(log2, expected);
    std::cout << "test_mpfr_class_const_log2 passed." << std::endl;
}

void test_div2exp_mul2exp_mpfr_class()
{
    mpfrxx::mpfr_class value("12", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_div_2ui(result.mpfr_data(), value.mpfr_data(), 2, MPFR_RNDN);
    require_mpfr_close(result, "3");
    mpfr_mul_2ui(result.mpfr_data(), value.mpfr_data(), 3, MPFR_RNDN);
    require_mpfr_close(result, "96");
    std::cout << "test_div2exp_mul2exp_mpfr_class passed." << std::endl;
}

void test_log_mpfr_class()
{
    mpfrxx::mpfr_class value = mpfrxx::mpfr_class::with_precision(192);
    mpfr_exp(value.mpfr_data(), mpfrxx::mpfr_class("2", 192).mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_log(result.mpfr_data(), value.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "2");
    std::cout << "test_log_mpfr_class passed." << std::endl;
}

void test_exp_mpfr_class()
{
    mpfrxx::mpfr_class value("2", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_exp(result.mpfr_data(), value.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_exp(expected.mpfr_data(), value.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(result, expected);
    std::cout << "test_exp_mpfr_class passed." << std::endl;
}

void test_log_exp_mpfr_class()
{
    mpfrxx::mpfr_class one("1", 192);
    mpfrxx::mpfr_class exp_value = mpfrxx::mpfr_class::with_precision(192);
    mpfr_exp(exp_value.mpfr_data(), one.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class log_value = mpfrxx::mpfr_class::with_precision(192);
    mpfr_log(log_value.mpfr_data(), exp_value.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(log_value, "1");
    std::cout << "test_log_exp_mpfr_class passed." << std::endl;
}

void test_cos()
{
    mpfrxx::mpfr_class x("0.5", 192);
    mpfrxx::mpfr_class y = mpfrxx::mpfr_class::with_precision(192);
    mpfr_cos(y.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_cos(expected.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(y, expected);
    std::cout << "test_cos passed." << std::endl;
}

void test_sin()
{
    mpfrxx::mpfr_class x("0.5", 192);
    mpfrxx::mpfr_class y = mpfrxx::mpfr_class::with_precision(192);
    mpfr_sin(y.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_sin(expected.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(y, expected);
    std::cout << "test_sin passed." << std::endl;
}

void test_tan()
{
    mpfrxx::mpfr_class x("0.5", 192);
    mpfrxx::mpfr_class y = mpfrxx::mpfr_class::with_precision(192);
    mpfr_tan(y.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_tan(expected.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(y, expected);
    std::cout << "test_tan passed." << std::endl;
}

void test_pow()
{
    mpfrxx::mpfr_class x("2", 192);
    mpfrxx::mpfr_class y("3", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_pow(result.mpfr_data(), x.mpfr_data(), y.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "8");
    std::cout << "test_pow passed." << std::endl;
}

void test_log2()
{
    mpfrxx::mpfr_class x("8", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_log2(result.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "3");
    std::cout << "test_log2 passed." << std::endl;
}

void test_log10()
{
    mpfrxx::mpfr_class x("100", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_log10(result.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "2");
    std::cout << "test_log10 passed." << std::endl;
}

void test_atan()
{
    mpfrxx::mpfr_class x("1", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_atan(result.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_atan(expected.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(result, expected);
    std::cout << "test_atan passed." << std::endl;
}

void test_atan2()
{
    mpfrxx::mpfr_class y("1", 192);
    mpfrxx::mpfr_class x("1", 192);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(192);
    mpfr_atan2(result.mpfr_data(), y.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    mpfrxx::mpfr_class expected = mpfrxx::mpfr_class::with_precision(192);
    mpfr_atan2(expected.mpfr_data(), y.mpfr_data(), x.mpfr_data(), MPFR_RNDN);
    require_mpfr_same_string(result, expected);
    std::cout << "test_atan2 passed." << std::endl;
}

void test_casts()
{
    mpfrxx::mpz_class z(123);
    mpfrxx::mpq_class q("3/2");
    mpfrxx::mpfr_class rz = mpfrxx::mpfr_class::with_precision(160);
    mpfrxx::mpfr_class rq = mpfrxx::mpfr_class::with_precision(160);
    mpfr_set_z(rz.mpfr_data(), z.get_mpz_t(), MPFR_RNDN);
    mpfr_set_q(rq.mpfr_data(), q.get_mpq_t(), MPFR_RNDN);
    mpfrxx::mpz_class truncated;
    mpfr_get_z(truncated.get_mpz_t(), mpfrxx::mpfr_class("123.75", 160).mpfr_data(), MPFR_RNDZ);
    require_mpfr_close(rz, "123");
    require_mpfr_close(rq, "1.5");
    assert(truncated == 123);
    std::cout << "test_casts passed." << std::endl;
}

void test_precisions_mixed()
{
    mpfrxx::mpfr_class low(1.0, 64);
    mpfrxx::mpfr_class high(2.0, 256);
    mpfrxx::mpfr_class result = low + high;
    assert(result.get_prec() == 256);
    low += high;
    assert(low.get_prec() == 64);
    std::cout << "test_precisions_mixed passed." << std::endl;
}

void test_misc()
{
    mpfrxx::mpfr_class a("1.25", 160);
    mpfrxx::mpfr_class b("2.50", 160);
    assert(a.get_str() == "1.25");
    assert(b.get_str() == "2.5");
    assert(cmp(a, b) < 0);
    std::cout << "test_misc passed." << std::endl;
}

void test_reminder()
{
    mpfrxx::mpfr_class x("5.5", 160);
    mpfrxx::mpfr_class y("2", 160);
    mpfrxx::mpfr_class result = mpfrxx::mpfr_class::with_precision(160);
    mpfr_remainder(result.mpfr_data(), x.mpfr_data(), y.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(result, "-0.5");
    std::cout << "test_reminder passed." << std::endl;
}

void test_mpfr_class_extention()
{
    mpfrxx::mpfr_class f(2), g(1), h(3);
    mpfrxx::mpfr_class result;
    result = test_func(f * h, g);
    std::cout << "The result of test_func(f * h, g) is: " << result << std::endl;
    require_mpfr_close(result, "6");

    mpfrxx::mpfr_class value("16", 192);
    mpfrxx::mpfr_class root = mpfrxx::mpfr_class::with_precision(192);
    mpfr_sqrt(root.mpfr_data(), value.mpfr_data(), MPFR_RNDN);
    require_mpfr_close(root, "4");
    std::cout << "test_mpfr_class_extention passed." << std::endl;
}

void test_int64_t_uint64_t_constructor()
{
    mpfrxx::mpfr_class signed_value(std::numeric_limits<std::int64_t>::min());
    mpfrxx::mpfr_class unsigned_value(std::numeric_limits<std::uint64_t>::max());
    mpfrxx::mpz_class expected_signed(std::numeric_limits<std::int64_t>::min());
    mpfrxx::mpz_class expected_unsigned(std::numeric_limits<std::uint64_t>::max());
    assert(mpfr_cmp_z(signed_value.mpfr_data(), expected_signed.get_mpz_t()) == 0);
    assert(mpfr_cmp_z(unsigned_value.mpfr_data(), expected_unsigned.get_mpz_t()) == 0);
    std::cout << "test_int64_t_uint64_t_constructor passed." << std::endl;
}

void test_int32_t_uint32_t_constructor()
{
    mpfrxx::mpfr_class signed_value(std::numeric_limits<std::int32_t>::min());
    mpfrxx::mpfr_class unsigned_value(std::numeric_limits<std::uint32_t>::max());
    require_mpfr_close(signed_value, std::to_string(std::numeric_limits<std::int32_t>::min()));
    require_mpfr_close(unsigned_value, std::to_string(std::numeric_limits<std::uint32_t>::max()));
    std::cout << "test_int32_t_uint32_t_constructor passed." << std::endl;
}

void test_int64_t_uint64_t_int32_t_uint32_t_assignment()
{
    mpfrxx::mpfr_class value;
    value = std::numeric_limits<std::int64_t>::min();
    assert(mpfr_cmp_z(value.mpfr_data(), mpfrxx::mpz_class(std::numeric_limits<std::int64_t>::min()).get_mpz_t()) == 0);
    value = std::numeric_limits<std::uint64_t>::max();
    assert(mpfr_cmp_z(value.mpfr_data(), mpfrxx::mpz_class(std::numeric_limits<std::uint64_t>::max()).get_mpz_t()) == 0);
    value = std::numeric_limits<std::int32_t>::min();
    require_mpfr_close(value, std::to_string(std::numeric_limits<std::int32_t>::min()));
    value = std::numeric_limits<std::uint32_t>::max();
    require_mpfr_close(value, std::to_string(std::numeric_limits<std::uint32_t>::max()));
    std::cout << "test_int64_t_uint64_t_int32_t_uint32_t_assignment passed." << std::endl;
}

void test_int128_t_uint128_t_assignment()
{
#if defined(__SIZEOF_INT128__)
    {
        __int128_t testValue = static_cast<__int128_t>(0x0123456789ABCDEF) * 0xFEDCBA9876543210;
        mpfrxx::mpz_class value;
        value = testValue;

        const std::string expected = helper::int128_to_string(testValue);
        const std::string actual = value.get_str();

        std::cout << "Testing __int128_t assignment:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "__int128_t assignment test failed.");
        std::cout << "__int128_t assignment test passed!" << std::endl;
    }
    {
        __uint128_t testValue = static_cast<__uint128_t>(0xFEDCBA9876543210) * 0xFFFFFFFFFFFFFFFF;
        mpfrxx::mpz_class value;
        value = testValue;

        const std::string expected = helper::uint128_to_string(testValue);
        const std::string actual = value.get_str();

        std::cout << "Testing __uint128_t assignment:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "__uint128_t assignment test failed.");
        std::cout << "__uint128_t assignment test passed!" << std::endl;
    }
#endif
}

void test_int128_t_uint128_t_constructor()
{
#if defined(__SIZEOF_INT128__)
    {
        __int128_t testValue = static_cast<__int128_t>(0x0123456789ABCDEF) * 0xFEDCBA9876543210;
        mpfrxx::mpz_class value(testValue);

        const std::string expected = helper::int128_to_string(testValue);
        const std::string actual = value.get_str();

        std::cout << "Testing __int128_t construction:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "__int128_t construction test failed.");
        std::cout << "__int128_t construction test passed!" << std::endl;
    }
    {
        __int128_t testValue = -static_cast<__int128_t>(0x0123456789ABCDEF) * 0xFEDCBA9876543210;
        mpfrxx::mpz_class value(testValue);

        const std::string expected = helper::int128_to_string(testValue);
        const std::string actual = value.get_str();

        std::cout << "Testing __int128_t construction:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "__int128_t construction test failed.");
        std::cout << "__int128_t construction test passed!" << std::endl;
    }
    {
        __uint128_t testValue = static_cast<__uint128_t>(0xFEDCBA9876543210) * 0xFFFFFFFFFFFFFFFF;
        mpfrxx::mpz_class value(testValue);

        const std::string expected = helper::uint128_to_string(testValue);
        const std::string actual = value.get_str();

        std::cout << "Testing __uint128_t construction:" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;

        assert(actual == expected && "__uint128_t construction test failed.");
        std::cout << "__uint128_t construction test passed!" << std::endl;
    }
#endif
}

void test_mpc_class_construction_assignment()
{
    mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    mpfrxx::mpc_class copy(z);
    require_mpc_close(copy, "1", "2");
    mpfrxx::mpc_class assigned = mpfrxx::mpc_class::with_precision(160);
    assigned = z;
    require_mpc_close(assigned, "1", "2");
    std::cout << "test_mpc_class_construction_assignment passed." << std::endl;
}

void test_mpc_class_arithmetic()
{
    test_mpc_construction_arithmetic();
    std::cout << "test_mpc_class_arithmetic passed." << std::endl;
}

void test_mpc_class_io()
{
    test_mpc_streams();
    std::cout << "test_mpc_class_io passed." << std::endl;
}

} // namespace

int main()
{
    std::cout << "MPFRXX_MKII version: 2.0.0" << std::endl;
    // mpfr_class
    testDefaultPrecision();
    testDefaultConstructor();
    testCopyConstructor();
    testAssignmentOperator();
    testAssignmentOperator_the_rule_of_five();
    testInitializationAndAssignmentDouble();
    testInitializationAndAssignmentString();
    testAddition();
    testMultplication();
    testDivision();
    testSubtraction();
    testComparisonOperators();
    testSqrt();
    testNeg();
    testAbs();
    test_mpfr_class_double_addition();
    test_mpfr_class_double_subtraction();
    test_mpfr_class_double_multiplication();
    test_mpfr_class_double_division();
    testOutputOperator();
    testCeilFunction();
    testFloor();
    testHypot();
    testSgn();
    test_get_d();
    test_get_ui();
    test_get_si();
    test_mpfr_class_constructor_precision();
    test_mpfr_class_constructor_with_mpfr();
    test_mpfr_class_constructor_with_mpf();
    test_mpfr_class_literal();
    test_mpfr_class_swap();
    test_template_cmp();
    test_set_str();
    test_mpfr_class_get_str();
    test_trunc_function();
    test_fits_sint_p();
    test_fits_slong_p();
    test_fits_sshort_p();
    test_fits_uint_p();
    test_fits_ulong_p();
    test_fits_ushort_p();

    // mpfrxx exact aliases
    testDefaultConstructor_mpz_class();
    testCopyConstructor_mpz_class();
    testAssignmentOperator_mpz_class();
    testAssignmentOperator_the_rule_of_five_mpz_class();
    testInitializationAndAssignmentDouble_mpz_class();
    testInitializationAndAssignment_int64_t_uint64_t_mpz_class();
    testInitializationAndAssignment_mpz_class_mpfr_class();
    testInitializationAndAssignment_mpfr_class_mpz_class();
    testInitializationAndAssignmentString_mpz_class();
    test_template_cmp_mpz_class();
    test_mpz_class_literal();
    test_arithmetic_operators_mpz_class();
    testFitsFunctions_mpz_class();
    testAbsFunction_mpz_class();
    testConversionFunctions_mpz_class();
    testMathFunctions_mpz_class();
    test_mpz_class_extention();
    test_set_str_mpz_class();
    test_factorial_mpz_class();
    test_primorial_mpz_class();
    test_fibonacci_mpz_class();
    test_mpz_class_swap();
    testOutputOperator_mpz_class();
    test_mpz_class_addition();
    test_mpz_class_subtraction();
    test_mpz_class_multiplication();
    test_mpz_class_division();
    test_mpz_class_modulus();
    testDefaultConstructor_mpq_class();
    testCopyConstructor_mpq_class();
    testAssignmentOperator_mpq_class();
    testInitializationAndAssignmentInt_mpq_class();
    testAssignmentOperator_the_rule_of_five_mpq_class();
    testInitializationAndAssignmentString_mpq_class();
    test_template_cmp_mpq_class();
    test_arithmetic_operators_mpq_class_hardcoded1();
    test_arithmetic_operators_mpq_class_hardcoded2();
    test_mpq_class_literal();
    test_mpq_class_functions();
    test_mpz_class_comparison_int();
    testExactAliasConstruction();
    test_mixed_exact_mpfr_promotion();

    // mpfr_class math/reference functions
    test_mpfr_class_const_pi();
    test_mpfr_class_const_log2();
    test_div2exp_mul2exp_mpfr_class();
    test_log_mpfr_class();
    test_exp_mpfr_class();
    test_log_exp_mpfr_class();
    test_mpfr_strings_streams_math();
    test_casts();
    test_precisions_mixed();
    test_misc();
    test_reminder();
    test_mpfr_class_extention();
    test_cos();
    test_sin();
    test_tan();
    test_pow();
    test_log2();
    test_log10();
    test_atan();
    test_atan2();
    test_int64_t_uint64_t_constructor();
    test_int32_t_uint32_t_constructor();
    test_int64_t_uint64_t_int32_t_uint32_t_assignment();
    test_int128_t_uint128_t_constructor();
    test_int128_t_uint128_t_assignment();

    // mpc_class
    test_mpc_class_construction_assignment();
    test_mpc_class_arithmetic();
    test_mpc_class_io();

    std::cout << "All tests passed." << std::endl;
    return 0;
}
