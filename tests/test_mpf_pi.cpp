#include <gmpxx_mkII.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void require_pi_prefix(const gmpxx::mpf_class& value, std::size_t digits)
{
    mp_exp_t exponent = 0;
    const std::string got = value.get_str(exponent, 10, digits + 8);
    // Reference: WolframAlpha N[pi, 1000], also mirrored in the upstream gmpxx_mkII tests.
    const std::string expected =
        "31415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679"
        "82148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964"
        "42881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372"
        "45870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330"
        "57270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833"
        "67336244065664308602139494639522473719070217986094370277053921717629317675238467481846766940513200056"
        "81271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199"
        "56112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459"
        "45534690830264252230825334468503526193118817101000313783875288658753320838142061717766914730359825349"
        "0428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199";

    if (exponent != 1) {
        std::cerr << "unexpected pi exponent: got " << exponent << '\n';
        std::abort();
    }
    if (got.size() < digits || got.substr(0, digits) != expected.substr(0, digits)) {
        std::cerr << "unexpected pi digits at " << digits << " digits\n";
        std::cerr << "got      " << got << '\n';
        std::cerr << "expected " << expected.substr(0, digits) << '\n';
        std::abort();
    }
}

void require_log_two_prefix(const gmpxx::mpf_class& value, std::size_t digits)
{
    mp_exp_t exponent = 0;
    const std::string got = value.get_str(exponent, 10, digits + 8);
    // Reference: WolframAlpha N[ln(2), 1000], also mirrored in the upstream gmpxx_mkII tests.
    const std::string expected =
        "6931471805599453094172321214581765680755001343602552541206800094933936219696947156058633269964186875"
        "42001481020570685733685520235758130557032670751635075961930727570828371435190307038623891673471123350"
        "11536449795523912047517268157493206515552473413952588295045300709532636664265410423915781495204374043"
        "03855008019441706416715186447128399681717845469570262716310645461502572074024816377733896385506952606"
        "68341137273873722928956493547025762652098859693201965058554764703306793654432547632744951250406069438"
        "14710468994650622016772042452452961268794654619316517468139267250410380254625965686914419287160829380"
        "31727143677826548775664850856740776484514644399404614226031930967354025744460703080960850474866385231"
        "38181676751438667476647890881437141985494231519973548803751658612753529166100071053558249879414729509"
        "29311389715599820565439287170007218085761025236889213244971389320378439353088774825970171559107088236"
        "83627589842589185353024363421436706118923678919237231467232172053401649256872747782344535348";

    if (exponent != 0) {
        std::cerr << "unexpected log_two exponent: got " << exponent << '\n';
        std::abort();
    }
    if (got.size() < digits || got.substr(0, digits) != expected.substr(0, digits)) {
        std::cerr << "unexpected log_two digits at " << digits << " digits\n";
        std::cerr << "got      " << got << '\n';
        std::cerr << "expected " << expected.substr(0, digits) << '\n';
        std::abort();
    }
}

void require_same_value(const gmpxx::mpf_class& lhs, const gmpxx::mpf_class& rhs)
{
    if (lhs.precision() != rhs.precision()) {
        std::cerr << "precision mismatch: " << lhs.precision() << " vs " << rhs.precision() << '\n';
        std::abort();
    }
    if (mpf_cmp(lhs.mpf_data(), rhs.mpf_data()) != 0) {
        std::cerr << "value mismatch at precision " << lhs.precision() << '\n';
        std::abort();
    }
}

std::size_t pi_reference_digits_for_precision(mp_bitcnt_t precision)
{
    const std::size_t decimal_digits = static_cast<std::size_t>((precision * 30103) / 100000);
    const std::size_t stable_digits = decimal_digits > 8 ? decimal_digits - 8 : decimal_digits;
    return stable_digits < 998 ? stable_digits : 998;
}

std::size_t log_two_reference_digits_for_precision(mp_bitcnt_t precision)
{
    const std::size_t decimal_digits = static_cast<std::size_t>((precision * 30103) / 100000);
    const std::size_t stable_digits = decimal_digits > 8 ? decimal_digits - 8 : decimal_digits;
    return stable_digits < 998 ? stable_digits : 998;
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
    if (gmpxx::default_mpf_precision_bits() == 512) {
        require_same_value(default_pi, gmpxx::pi(static_cast<mp_bitcnt_t>(512)));
    }

    gmpxx::mpf_class pi_512_before = gmpxx::mpf_class::with_precision(512);
    gmpxx::mpf_class pi_1024_before = gmpxx::mpf_class::with_precision(1024);
    gmpxx::mpf_class pi_2048_before = gmpxx::mpf_class::with_precision(2048);
    for (mp_bitcnt_t precision : std::array<mp_bitcnt_t, 4>{{512, 1024, 2048, 4096}}) {
        const auto value = gmpxx::pi(precision);
        if (value.precision() < precision) {
            std::abort();
        }
        require_pi_prefix(value, pi_reference_digits_for_precision(precision));
        if (precision == 512) {
            pi_512_before = value;
        } else if (precision == 1024) {
            pi_1024_before = value;
        } else if (precision == 2048) {
            pi_2048_before = value;
        }
    }

    require_same_value(gmpxx::pi(static_cast<mp_bitcnt_t>(2048)), pi_2048_before);
    require_same_value(gmpxx::pi(static_cast<mp_bitcnt_t>(1024)), pi_1024_before);
    require_same_value(gmpxx::pi(static_cast<mp_bitcnt_t>(512)), pi_512_before);

    const auto log_two_128 = gmpxx::log_two(static_cast<mp_bitcnt_t>(128));
    if (log_two_128.precision() < 128) {
        std::abort();
    }
    require_log_two_prefix(log_two_128, 26);

    const auto log_two_256 = gmpxx::const_log2(static_cast<mp_bitcnt_t>(256));
    if (log_two_256.precision() < 256) {
        std::abort();
    }
    require_log_two_prefix(log_two_256, 56);

    const auto log_two_96_again = gmpxx::log_two(static_cast<mp_bitcnt_t>(96));
    if (log_two_96_again.precision() < 96) {
        std::abort();
    }
    require_log_two_prefix(log_two_96_again, 25);

    const auto default_log_two = gmpxx::const_log2();
    if (default_log_two.precision() < gmpxx::default_mpf_precision_bits()) {
        std::abort();
    }
    require_log_two_prefix(default_log_two, 56);
    if (gmpxx::default_mpf_precision_bits() == 512) {
        require_same_value(default_log_two, gmpxx::log_two(static_cast<mp_bitcnt_t>(512)));
    }

    gmpxx::mpf_class log_two_512_before = gmpxx::mpf_class::with_precision(512);
    gmpxx::mpf_class log_two_1024_before = gmpxx::mpf_class::with_precision(1024);
    gmpxx::mpf_class log_two_2048_before = gmpxx::mpf_class::with_precision(2048);
    for (mp_bitcnt_t precision : std::array<mp_bitcnt_t, 4>{{512, 1024, 2048, 4096}}) {
        const auto value = gmpxx::log_two(precision);
        if (value.precision() < precision) {
            std::abort();
        }
        require_log_two_prefix(value, log_two_reference_digits_for_precision(precision));
        if (precision == 512) {
            log_two_512_before = value;
        } else if (precision == 1024) {
            log_two_1024_before = value;
        } else if (precision == 2048) {
            log_two_2048_before = value;
        }
    }

    require_same_value(gmpxx::log_two(static_cast<mp_bitcnt_t>(2048)), log_two_2048_before);
    require_same_value(gmpxx::log_two(static_cast<mp_bitcnt_t>(1024)), log_two_1024_before);
    require_same_value(gmpxx::log_two(static_cast<mp_bitcnt_t>(512)), log_two_512_before);

    return 0;
}
