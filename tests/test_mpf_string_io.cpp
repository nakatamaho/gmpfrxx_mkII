#include <gmpxx_mkII.h>

#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

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
