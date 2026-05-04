#include <gmpxx_mkII.h>

#include <cstdlib>
#include <string>

namespace {

void require_pi_prefix(const gmpxx::mpf_class& value, std::size_t digits)
{
    mp_exp_t exponent = 0;
    const std::string got = value.get_str(exponent, 10, digits);
    const std::string expected =
        "3141592653589793238462643383279502884197169399375105820974944592";

    if (exponent != 1) {
        std::abort();
    }
    if (got.size() < digits || got.substr(0, digits) != expected.substr(0, digits)) {
        std::abort();
    }
}

} // namespace

int main()
{
    const auto pi_128 = gmpxx::pi(static_cast<mp_bitcnt_t>(128));
    if (pi_128.precision() < 128) {
        std::abort();
    }
    require_pi_prefix(pi_128, 28);

    const auto pi_256 = gmpxx::const_pi(static_cast<mp_bitcnt_t>(256));
    if (pi_256.precision() < 256) {
        std::abort();
    }
    require_pi_prefix(pi_256, 60);

    const auto pi_96_again = gmpxx::pi(static_cast<mp_bitcnt_t>(96));
    if (pi_96_again.precision() < 96) {
        std::abort();
    }
    require_pi_prefix(pi_96_again, 25);

    const auto default_pi = gmpxx::const_pi();
    if (default_pi.precision() < gmpxx::default_mpf_precision_bits()) {
        std::abort();
    }
    require_pi_prefix(default_pi, 60);

    return 0;
}
