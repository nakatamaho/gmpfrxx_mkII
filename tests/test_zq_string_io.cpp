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

#include <iomanip>
#include <cassert>
#include <cstdlib>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<mpz_srcptr>()),
                             std::ostream&>);
static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<mpq_srcptr>()),
                             std::ostream&>);
static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<mpz_ptr>()),
                             std::istream&>);
static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<mpq_ptr>()),
                             std::istream&>);
static_assert(std::is_same_v<decltype(print_mpz(std::declval<std::ostream&>(), std::declval<mpz_srcptr>())),
                             void>);
static_assert(std::is_same_v<decltype(print_mpq(std::declval<std::ostream&>(), std::declval<mpq_srcptr>())),
                             void>);

int main()
{
    gmpxx::mpz_class z("ff", 16);
    if (z.get_str() != "255" || z.get_str(16) != "ff") {
        std::abort();
    }

    const std::string z_before = z.get_str();
    if (z.set_str("not-an-integer") == 0 || z.get_str() != z_before) {
        std::abort();
    }

    z.set("-42");
    if (z.get_str() != "-42") {
        std::abort();
    }
    z = "0x2a";
    if (z.get_str() != "42") {
        std::abort();
    }
    z = std::string("052");
    if (z.get_str() != "42") {
        std::abort();
    }
    z.set("-42");

    std::ostringstream out;
    out << std::showbase << std::hex << std::uppercase << z;
    if (out.str() != "-0X2A") {
        std::abort();
    }

    gmpxx::mpq_class q("6/8");
    if (q.get_str() != "3/4") {
        std::abort();
    }

    gmpxx::mpq_class hex_q(std::string("10/20"), 16);
    if (hex_q.get_str() != "1/2") {
        std::abort();
    }

    const std::string q_before = q.get_str();
    if (q.set_str("not-a-rational") == 0 || q.get_str() != q_before) {
        std::abort();
    }

    q.set("-10/8");
    if (q.get_str() != "-5/4") {
        std::abort();
    }
    q = "0x10/0x20";
    if (q.get_str() != "1/2") {
        std::abort();
    }
    q = std::string("052/0104");
    if (q.get_str() != "21/34") {
        std::abort();
    }
    q.set("-10/8");

    out.str("");
    out.clear();
    out << std::dec << std::noshowbase << std::nouppercase << std::noshowpos;
    out << std::showpos << q;
    if (out.str() != "-5/4") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::dec << std::noshowbase << std::nouppercase << std::noshowpos;
    out << std::showbase << std::hex << gmpxx::mpq_class("31/16");
    if (out.str() != "0x1f/0x10") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::dec << std::noshowbase << std::nouppercase << std::noshowpos
        << std::internal << std::setw(7) << std::setfill('_') << gmpxx::mpz_class("-42");
    if (out.str() != "-____42") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::dec << std::noshowbase << std::nouppercase << std::noshowpos
        << std::showbase << std::hex << gmpxx::mpq_class("0");
    if (out.str() != "0x0") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::showbase << std::uppercase << std::hex << gmpxx::mpq_class("11/13");
    if (out.str() != "0XB/0XD") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::dec << std::nouppercase << std::showbase << std::internal << std::setfill('_')
        << std::setw(8) << std::hex << gmpxx::mpz_class("255");
    if (out.str() != "0x____ff") {
        std::abort();
    }

    out.str("");
    out.clear();
    const gmpxx::mpz_class za(10);
    const gmpxx::mpz_class zb(15);
    out << std::hex << std::showbase << (za + zb);
    if (out.str() != "0x19") {
        std::abort();
    }

    out.str("");
    out.clear();
    const gmpxx::mpq_class qa("1/3");
    const gmpxx::mpq_class qb("1/6");
    out << std::dec << (qa + qb);
    if (out.str() != "1/2") {
        std::abort();
    }

    out.str("");
    out.clear();
    print_mpz(out, z.get_mpz_t());
    if (out.str() != "-42") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::showbase << std::hex << gmpxx::mpz_class("255").get_mpz_t();
    if (out.str() != "0xff") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::dec << std::noshowbase << std::nouppercase << std::noshowpos;
    print_mpq(out, gmpxx::mpq_class("6/8").get_mpq_t());
    if (out.str() != "3/4") {
        std::abort();
    }

    out.str("");
    out.clear();
    out << std::showbase << std::hex << gmpxx::mpq_class("31/16").get_mpq_t();
    if (out.str() != "0x1f/0x10") {
        std::abort();
    }

    gmpxx::mpz_class parsed_z(-1);
    std::istringstream z_input("  +0x2a rest");
    z_input >> std::setbase(0);
    z_input >> parsed_z;
    if (parsed_z.get_str() != "42") {
        std::abort();
    }
    std::string rest;
    z_input >> rest;
    if (rest != "rest") {
        std::abort();
    }

    std::istringstream octal_z_input("052");
    octal_z_input >> std::setbase(0);
    octal_z_input >> parsed_z;
    if (parsed_z.get_str() != "42") {
        std::abort();
    }

    std::istringstream partial_decimal_z_input("1f");
    partial_decimal_z_input >> std::dec >> parsed_z;
    if (partial_decimal_z_input.fail() || parsed_z.get_str() != "1" ||
        partial_decimal_z_input.tellg() != std::streampos(1)) {
        std::abort();
    }

    parsed_z = -1;
    std::istringstream noskipws_z_input(" 123");
    noskipws_z_input >> std::noskipws >> parsed_z;
    if (!noskipws_z_input.fail() || parsed_z.get_str() != "-1") {
        std::abort();
    }
    noskipws_z_input.clear();
    if (noskipws_z_input.tellg() != std::streampos(0)) {
        std::abort();
    }

    std::istringstream negative_hex_z_input("-0x123");
    negative_hex_z_input.flags(std::ios::fmtflags(0));
    negative_hex_z_input >> parsed_z;
    if (negative_hex_z_input.fail() || parsed_z.get_str() != "-291") {
        std::abort();
    }

    const std::string parsed_z_before = parsed_z.get_str();
    std::istringstream invalid_z_input("not-an-integer");
    invalid_z_input >> parsed_z;
    if (!invalid_z_input.fail() || parsed_z.get_str() != parsed_z_before) {
        std::abort();
    }

    mpz_t raw_z;
    mpz_init_set_si(raw_z, -1);
    std::istringstream raw_z_input("  +0x2a rest");
    raw_z_input >> std::setbase(0);
    raw_z_input >> raw_z;
    if (mpz_get_si(raw_z) != 42) {
        std::abort();
    }
    raw_z_input >> rest;
    if (rest != "rest") {
        std::abort();
    }
    std::istringstream invalid_raw_z_input("not-an-integer");
    invalid_raw_z_input >> raw_z;
    if (!invalid_raw_z_input.fail() || mpz_get_si(raw_z) != 42) {
        std::abort();
    }
    mpz_clear(raw_z);

    gmpxx::mpq_class parsed_q("1/7");
    std::istringstream q_input("+6/+8 tail");
    q_input >> parsed_q;
    if (parsed_q.get_str() != "6/8") {
        std::abort();
    }
    q_input >> rest;
    if (rest != "tail") {
        std::abort();
    }

    std::istringstream hex_q_input("10/20");
    hex_q_input >> std::hex >> parsed_q;
    if (hex_q_input.fail() || parsed_q.get_str() != "16/32") {
        std::abort();
    }

    std::istringstream auto_hex_q_input("0x5/0x8");
    auto_hex_q_input.flags(std::ios::fmtflags(0));
    auto_hex_q_input >> parsed_q;
    if (auto_hex_q_input.fail() || parsed_q.get_str() != "5/8") {
        std::abort();
    }

    std::istringstream spaced_q_input("123 /456");
    spaced_q_input >> parsed_q;
    if (spaced_q_input.fail() || parsed_q.get_str() != "123" ||
        spaced_q_input.tellg() != std::streampos(3)) {
        std::abort();
    }

    parsed_q = gmpxx::mpq_class("7/9");
    std::istringstream bad_spaced_q_input("123/ 456");
    bad_spaced_q_input >> parsed_q;
    if (!bad_spaced_q_input.fail() || parsed_q.get_str() != "7/9") {
        std::abort();
    }
    bad_spaced_q_input.clear();
    if (bad_spaced_q_input.tellg() != std::streampos(4)) {
        std::abort();
    }

    std::istringstream zero_denominator_input("1/0");
    zero_denominator_input >> parsed_q;
    if (zero_denominator_input.fail() || parsed_q.get_str() != "1/0") {
        std::abort();
    }

    mpq_t raw_q;
    mpq_init(raw_q);
    mpq_set_ui(raw_q, 1, 7);
    std::istringstream raw_q_input("+6/+8 tail");
    raw_q_input >> raw_q;
    if (mpz_cmp_ui(mpq_numref(raw_q), 6) != 0 || mpz_cmp_ui(mpq_denref(raw_q), 8) != 0) {
        std::abort();
    }
    raw_q_input >> rest;
    if (rest != "tail") {
        std::abort();
    }
    std::istringstream invalid_raw_q_input("1/0");
    invalid_raw_q_input >> raw_q;
    if (invalid_raw_q_input.fail() ||
        mpz_cmp_ui(mpq_numref(raw_q), 1) != 0 ||
        mpz_cmp_ui(mpq_denref(raw_q), 0) != 0) {
        std::abort();
    }
    mpq_clear(raw_q);

    bool threw = false;
    try {
        gmpxx::mpz_class invalid_z("not-an-integer");
        (void)invalid_z;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    threw = false;
    try {
        gmpxx::mpq_class invalid_q("not-a-rational");
        (void)invalid_q;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }

    return 0;
}
