#include <gmpxx_mkII.h>

#include <cstdlib>
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
