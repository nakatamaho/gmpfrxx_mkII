#include <gmpxx_mkII.h>

#include <iomanip>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>

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

    const std::string parsed_z_before = parsed_z.get_str();
    std::istringstream invalid_z_input("not-an-integer");
    invalid_z_input >> parsed_z;
    if (!invalid_z_input.fail() || parsed_z.get_str() != parsed_z_before) {
        std::abort();
    }

    gmpxx::mpq_class parsed_q("1/7");
    std::istringstream q_input("+6/+8 tail");
    q_input >> parsed_q;
    if (parsed_q.get_str() != "3/4") {
        std::abort();
    }
    q_input >> rest;
    if (rest != "tail") {
        std::abort();
    }

    std::istringstream hex_q_input("10/20");
    hex_q_input >> std::hex >> parsed_q;
    if (parsed_q.get_str() != "1/2") {
        std::abort();
    }

    const std::string parsed_q_before = parsed_q.get_str();
    std::istringstream zero_denominator_input("1/0");
    zero_denominator_input >> parsed_q;
    if (!zero_denominator_input.fail() || parsed_q.get_str() != parsed_q_before) {
        std::abort();
    }

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
