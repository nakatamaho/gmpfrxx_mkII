#include <gmpxx_mkII.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void require_prefix(const gmpxx::mpf_class& value,
                    int expected_exponent,
                    const char* expected_digits,
                    std::size_t digits)
{
    mp_exp_t exponent = 0;
    const std::string got = value.get_str(exponent, 10, digits);
    if (exponent != expected_exponent) {
        std::cerr << "unexpected exponent: got " << exponent
                  << " expected " << expected_exponent << '\n';
        std::abort();
    }
    if (got.size() < digits || got.substr(0, digits) != std::string(expected_digits).substr(0, digits)) {
        std::cerr << "unexpected digits: got " << got
                  << " expected prefix " << expected_digits << '\n';
        std::abort();
    }
}

void require_zero(const gmpxx::mpf_class& value)
{
    if (mpf_cmp_ui(value.mpf_data(), 0) != 0) {
        std::abort();
    }
}

void require_close_bits(const gmpxx::mpf_class& lhs,
                        const gmpxx::mpf_class& rhs,
                        mp_bitcnt_t precision,
                        mp_bitcnt_t tolerance_bits)
{
    gmpxx::mpf_class diff = gmpxx::mpf_math_detail::abs_prec(
        gmpxx::mpf_math_detail::sub(lhs, rhs, precision), precision);
    gmpxx::mpf_class threshold = gmpxx::mpf_math_detail::make_ui(1, precision);
    mpf_div_2exp(threshold.mpf_data(), threshold.mpf_data(), tolerance_bits);
    if (mpf_cmp(diff.mpf_data(), threshold.mpf_data()) >= 0) {
        std::cerr << "values differ by more than 2^-" << tolerance_bits << '\n';
        std::abort();
    }
}

void require_high_precision_log_case(mp_bitcnt_t precision)
{
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class four(4, precision);
    gmpxx::mpf_class half = gmpxx::mpf_class::with_precision(precision);
    mpf_set_ui(half.mpf_data(), 1);
    mpf_div_2exp(half.mpf_data(), half.mpf_data(), 1);

    const mp_bitcnt_t tolerance_bits = precision - 32;
    const gmpxx::mpf_class log_two = gmpxx::mpf_math_detail::log_two(precision);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(two, precision),
                       log_two,
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(four, precision),
                       gmpxx::mpf_math_detail::mul_ui(log_two, 2, precision),
                       precision,
                       tolerance_bits);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(half, precision),
                       gmpxx::mpf_math_detail::sub(
                           gmpxx::mpf_math_detail::make_ui(0, precision), log_two, precision),
                       precision,
                       tolerance_bits);

    gmpxx::mpf_class tiny = gmpxx::mpf_math_detail::make_ui(1, precision);
    mpf_div_2exp(tiny.mpf_data(), tiny.mpf_data(), precision / 2);
    const gmpxx::mpf_class one_plus_tiny =
        gmpxx::mpf_math_detail::add(one, tiny, precision);
    require_close_bits(gmpxx::mpf_math_detail::compute_log(one_plus_tiny, precision),
                       gmpxx::mpf_math_detail::compute_log1p(tiny, precision),
                       precision,
                       tolerance_bits);
}

} // namespace

int main()
{
    constexpr mp_bitcnt_t precision = 192;

    const gmpxx::mpf_class zero(0, precision);
    const gmpxx::mpf_class one(1, precision);
    const gmpxx::mpf_class two(2, precision);
    const gmpxx::mpf_class ten(10, precision);
    const gmpxx::mpf_class small("0.000000000000000000000001", precision);

    require_zero(gmpxx::mpf_math_detail::compute_log(one, precision));
    require_prefix(gmpxx::mpf_math_detail::compute_log(two, precision),
                   0,
                   "693147180559945309417232121458176568075500134360255254120680",
                   44);
    require_prefix(gmpxx::mpf_math_detail::compute_log(ten, precision),
                   1,
                   "230258509299404568401799145468436420760110148862877297603333",
                   40);
    require_prefix(gmpxx::mpf_math_detail::compute_log1p(small, precision),
                   -23,
                   "100000000000000000000000",
                   1);

    bool saw_zero_domain_error = false;
    try {
        (void)gmpxx::mpf_math_detail::compute_log(zero, precision);
    } catch (const std::domain_error&) {
        saw_zero_domain_error = true;
    }
    if (!saw_zero_domain_error) {
        std::abort();
    }

    bool saw_negative_domain_error = false;
    try {
        (void)gmpxx::mpf_math_detail::compute_log(gmpxx::mpf_class(-1, precision), precision);
    } catch (const std::domain_error&) {
        saw_negative_domain_error = true;
    }
    if (!saw_negative_domain_error) {
        std::abort();
    }

    bool saw_log1p_pole = false;
    try {
        (void)gmpxx::mpf_math_detail::compute_log1p(gmpxx::mpf_class(-1, precision), precision);
    } catch (const std::domain_error&) {
        saw_log1p_pole = true;
    }
    if (!saw_log1p_pole) {
        std::abort();
    }

    for (mp_bitcnt_t high_precision : std::array<mp_bitcnt_t, 3>{{512, 1024, 2048}}) {
        require_high_precision_log_case(high_precision);
    }

    return 0;
}
