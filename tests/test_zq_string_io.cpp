#include <gmpxx_mkII.h>

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
