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

#ifndef GMPFRXX_MKII_DETAIL_MATH_MPF_HPP
#define GMPFRXX_MKII_DETAIL_MATH_MPF_HPP

#include <gmpfrxx_mkII/detail/mpf_impl.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace gmpxx {

namespace mpf_math_detail {

inline constexpr mp_bitcnt_t minimum_target_precision = 32;
inline constexpr mp_bitcnt_t log_cancellation_probe_guard_bits = 32;

inline mp_bitcnt_t normalize_target_precision(mp_bitcnt_t precision)
{
    return std::max(precision, minimum_target_precision);
}

inline std::uint64_t iteration_limit_for_precision(mp_bitcnt_t precision)
{
    constexpr std::uint64_t minimum_iterations = 64;
    constexpr std::uint64_t precision_multiplier = 16;
    const std::uintmax_t precision_value = static_cast<std::uintmax_t>(precision);
    const std::uintmax_t max_value = static_cast<std::uintmax_t>(std::numeric_limits<std::uint64_t>::max());
    if (precision_value > max_value / precision_multiplier) {
        throw std::overflow_error("MPF iteration limit exceeds uint64_t");
    }
    return std::max(minimum_iterations, static_cast<std::uint64_t>(precision_value * precision_multiplier));
}

[[noreturn]] inline void throw_iteration_limit_exceeded(const char* function_name)
{
    throw std::runtime_error(std::string(function_name) + " failed to converge within iteration limit");
}

inline unsigned long ceil_log2_precision(mp_bitcnt_t value)
{
    if (value <= 1) {
        return 0;
    }
    --value;
    unsigned long bits = 0;
    while (value != 0) {
        value >>= 1;
        ++bits;
    }
    return bits;
}

inline mpf_class make_ui(unsigned long value, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_set_ui(result.mpf_data(), value);
    return result;
}

inline mpf_class make_u64(std::uint64_t value, mp_bitcnt_t precision)
{
    return mpf_class(value, precision);
}

inline std::uint64_t checked_taylor_counter_product(std::uint64_t lhs, std::uint64_t rhs)
{
    if (lhs != 0 && rhs > std::numeric_limits<std::uint64_t>::max() / lhs) {
        throw std::overflow_error("MPF Taylor denominator exceeds uint64_t");
    }
    return lhs * rhs;
}

inline std::uint64_t checked_taylor_counter_add(std::uint64_t lhs, std::uint64_t rhs)
{
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        throw std::overflow_error("MPF Taylor denominator exceeds uint64_t");
    }
    return lhs + rhs;
}

inline std::uint64_t checked_taylor_odd_denominator(std::uint64_t k)
{
    return checked_taylor_counter_add(checked_taylor_counter_product(std::uint64_t{2}, k), std::uint64_t{1});
}

inline mp_bitcnt_t checked_taylor_shift_count(mp_bitcnt_t scale, std::uint64_t factor)
{
    const std::uint64_t shift = checked_taylor_counter_product(static_cast<std::uint64_t>(scale), factor);
    if (shift > static_cast<std::uint64_t>(std::numeric_limits<mp_bitcnt_t>::max())) {
        throw std::overflow_error("MPF Taylor shift count exceeds mp_bitcnt_t");
    }
    return static_cast<mp_bitcnt_t>(shift);
}

inline void increment_taylor_counter(std::uint64_t& counter)
{
    if (counter == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("MPF Taylor counter exceeds uint64_t");
    }
    ++counter;
}

inline mpf_class set_prec_copy(const mpf_class& value, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_set(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class add(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_add(result.mpf_data(), lhs.mpf_data(), rhs.mpf_data());
    return result;
}

inline mpf_class sub(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_sub(result.mpf_data(), lhs.mpf_data(), rhs.mpf_data());
    return result;
}

inline mpf_class mul(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_mul(result.mpf_data(), lhs.mpf_data(), rhs.mpf_data());
    return result;
}

inline mpf_class mul_ui(const mpf_class& lhs, unsigned long rhs, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_mul_ui(result.mpf_data(), lhs.mpf_data(), rhs);
    return result;
}

inline mpf_class sqr(const mpf_class& value, mp_bitcnt_t precision)
{
    return mul(value, value, precision);
}

inline mpf_class div(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    if (mpf_sgn(rhs.mpf_data()) == 0) {
        throw std::domain_error("mpf division by zero");
    }
    mpf_class result = mpf_class::with_precision(precision);
    mpf_div(result.mpf_data(), lhs.mpf_data(), rhs.mpf_data());
    return result;
}

inline mpf_class sqrt_prec(const mpf_class& value, mp_bitcnt_t precision)
{
    if (mpf_sgn(value.mpf_data()) < 0) {
        throw std::domain_error("mpf square root of negative value");
    }
    mpf_class result = mpf_class::with_precision(precision);
    mpf_sqrt(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class abs_prec(const mpf_class& value, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_abs(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class scaled_hypot_abs(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class lhs_abs = abs_prec(lhs, precision);
    mpf_class rhs_abs = abs_prec(rhs, precision);

    const mpf_class* high = &lhs_abs;
    const mpf_class* low = &rhs_abs;
    if (mpf_cmp(lhs_abs.mpf_data(), rhs_abs.mpf_data()) < 0) {
        high = &rhs_abs;
        low = &lhs_abs;
    }

    if (mpf_sgn(high->mpf_data()) == 0) {
        return mpf_class::with_precision(precision);
    }

    mpf_class ratio = div(*low, *high, precision);
    mpf_mul(ratio.mpf_data(), ratio.mpf_data(), ratio.mpf_data());
    mpf_add_ui(ratio.mpf_data(), ratio.mpf_data(), 1);
    mpf_sqrt(ratio.mpf_data(), ratio.mpf_data());

    mpf_class result = mpf_class::with_precision(precision);
    mpf_mul(result.mpf_data(), high->mpf_data(), ratio.mpf_data());
    return result;
}

inline mpf_class average(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class result = add(lhs, rhs, precision);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return result;
}

inline mpf_class inv_sqrt_ui(unsigned long value, mp_bitcnt_t precision)
{
    if (value == 0ul) {
        throw std::domain_error("mpf inverse square root of zero");
    }
    mpf_class denominator = make_ui(value, precision);
    mpf_sqrt(denominator.mpf_data(), denominator.mpf_data());

    mpf_class result = mpf_class::with_precision(precision);
    mpf_ui_div(result.mpf_data(), 1ul, denominator.mpf_data());
    return result;
}

struct sincos_result {
    sincos_result() : sin_value(), cos_value(1) {}

    explicit sincos_result(mp_bitcnt_t precision)
        : sin_value(make_ui(0, precision)),
          cos_value(make_ui(1, precision))
    {
    }

    mpf_class sin_value;
    mpf_class cos_value;
};

struct trig_constant_cache_state {
    std::mutex mutex;
    mp_bitcnt_t cached_precision{0};
    mpf_class pi_value;
    mpf_class pi_over_two_value;
    mpf_class two_over_pi_value;
    bool initialized{false};
};

inline mp_bitcnt_t working_precision_for_pi(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + 96;
}

inline const char* pi_decimal_literal()
{
    return "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679"
           "82148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964"
           "42881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372"
           "45870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330"
           "57270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833"
           "67336244065664308602139494639522473719070217986094370277053921717629317675238467481846766940513200056"
           "81271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199"
           "56112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459"
           "45534690830264252230825334468503526193118817101000313783875288658753320838142061717766914730359825349"
           "0428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199";
}

inline bool has_hardcoded_pi(mp_bitcnt_t target_precision)
{
    constexpr mp_bitcnt_t decimal_digits = 1000;
    constexpr mp_bitcnt_t fractional_digits = decimal_digits - 1;
    constexpr mp_bitcnt_t conservative_bits = (fractional_digits * 332) / 100;
    return target_precision <= conservative_bits;
}

inline mpf_class hardcoded_pi(mp_bitcnt_t target_precision)
{
    return mpf_class(pi_decimal_literal(), target_precision);
}

inline mpf_class compute_pi_gauss_legendre(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_pi(target);

    mpf_class zero = make_ui(0, work);
    mpf_class one = make_ui(1, work);
    mpf_class two = make_ui(2, work);
    mpf_class four = make_ui(4, work);
    mpf_class quarter = set_prec_copy(one, work);
    mpf_div_2exp(quarter.mpf_data(), quarter.mpf_data(), 2);

    mpf_class a = one;
    mpf_class b = inv_sqrt_ui(2, work);
    mpf_class t = quarter;
    mpf_class p = one;
    mpf_class epsilon = make_ui(1, work);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), work);
    mpf_class pi_previous = zero;
    mpf_class pi_current = zero;

    const std::uint64_t max_iterations = iteration_limit_for_precision(work);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        mpf_class a_next = average(a, b, work);
        mpf_class b_next = sqrt_prec(mul(a, b, work), work);
        mpf_class diff = sub(a, a_next, work);
        mpf_class t_next = sub(t, mul(p, sqr(diff, work), work), work);

        a = a_next;
        b = b_next;
        t = t_next;
        p = mul(p, two, work);

        mpf_class sum = add(a, b, work);
        mpf_class numerator = sqr(sum, work);
        mpf_class denominator = mul(t, four, work);

        pi_previous = pi_current;
        pi_current = div(numerator, denominator, work);
        if (mpf_cmp(abs_prec(sub(pi_current, pi_previous, work), work).mpf_data(), epsilon.mpf_data()) < 0) {
            return set_prec_copy(pi_current, target);
        }
    }

    throw_iteration_limit_exceeded("compute_pi_gauss_legendre");
}

struct pi_cache_state {
    std::mutex mutex;
    mp_bitcnt_t cached_precision{0};
    mpf_class cached_value;
    bool initialized{false};
};

inline pi_cache_state& pi_cache()
{
    static pi_cache_state cache;
    return cache;
}

inline mpf_class pi(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    if (has_hardcoded_pi(target)) {
        return hardcoded_pi(target);
    }

    pi_cache_state& cache = pi_cache();
    std::lock_guard<std::mutex> lock(cache.mutex);
    if (!cache.initialized || cache.cached_precision < target) {
        mpf_class computed = compute_pi_gauss_legendre(target);
        cache.cached_value.swap(computed);
        cache.cached_precision = target;
        cache.initialized = true;
    }
    return set_prec_copy(cache.cached_value, target);
}

inline mp_bitcnt_t working_precision_for_log_two(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + 128;
}

inline const char* log_two_decimal_literal()
{
    return "0.6931471805599453094172321214581765680755001343602552541206800094933936219696947156058633269964186875"
           "42001481020570685733685520235758130557032670751635075961930727570828371435190307038623891673471123350"
           "11536449795523912047517268157493206515552473413952588295045300709532636664265410423915781495204374043"
           "03855008019441706416715186447128399681717845469570262716310645461502572074024816377733896385506952606"
           "68341137273873722928956493547025762652098859693201965058554764703306793654432547632744951250406069438"
           "14710468994650622016772042452452961268794654619316517468139267250410380254625965686914419287160829380"
           "31727143677826548775664850856740776484514644399404614226031930967354025744460703080960850474866385231"
           "38181676751438667476647890881437141985494231519973548803751658612753529166100071053558249879414729509"
           "29311389715599820565439287170007218085761025236889213244971389320378439353088774825970171559107088236"
           "83627589842589185353024363421436706118923678919237231467232172053401649256872747782344535348";
}

inline bool has_hardcoded_log_two(mp_bitcnt_t target_precision)
{
    constexpr mp_bitcnt_t decimal_digits = 1000;
    constexpr mp_bitcnt_t fractional_digits = decimal_digits - 1;
    constexpr mp_bitcnt_t conservative_bits = (fractional_digits * 332) / 100;
    return target_precision <= conservative_bits;
}

inline mpf_class hardcoded_log_two(mp_bitcnt_t target_precision)
{
    return mpf_class(log_two_decimal_literal(), target_precision);
}

inline mpf_class theta_series_threshold(mp_bitcnt_t precision)
{
    mpf_class threshold = make_ui(1, precision);
    mpf_div_2exp(threshold.mpf_data(), threshold.mpf_data(), precision);
    return threshold;
}

inline mpf_class agm_converged(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class a = set_prec_copy(lhs, precision);
    mpf_class b = set_prec_copy(rhs, precision);
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        mpf_class a_next = average(a, b, precision);
        mpf_class b_next = sqrt_prec(mul(a, b, precision), precision);
        if (mpf_cmp(abs_prec(sub(a_next, b_next, precision), precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return average(a_next, b_next, precision);
        }
        a = a_next;
        b = b_next;
    }
    throw_iteration_limit_exceeded("agm_converged");
}

inline mpf_class theta3_from_power_of_two_q(mp_bitcnt_t q_exponent, mp_bitcnt_t precision)
{
    const mpf_class threshold = theta_series_threshold(precision);
    mpf_class sum = make_ui(1, precision);
    mpf_class term = make_ui(1, precision);
    mpf_div_2exp(term.mpf_data(), term.mpf_data(), q_exponent);

    std::uint64_t k = 1;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        mpf_class contribution = mul_ui(term, 2ul, precision);
        if (mpf_cmp(contribution.mpf_data(), threshold.mpf_data()) < 0) {
            return sum;
        }
        sum = add(sum, contribution, precision);
        mpf_div_2exp(term.mpf_data(), term.mpf_data(),
                     checked_taylor_shift_count(q_exponent, checked_taylor_odd_denominator(k)));
        increment_taylor_counter(k);
    }
    throw_iteration_limit_exceeded("theta3_from_power_of_two_q");
}

inline mpf_class theta2_from_power_of_two_q(mp_bitcnt_t q_exponent, mp_bitcnt_t precision)
{
    const mpf_class threshold = theta_series_threshold(precision);
    mpf_class q = make_ui(1, precision);
    mpf_div_2exp(q.mpf_data(), q.mpf_data(), q_exponent);
    mpf_class term = sqrt_prec(sqrt_prec(q, precision), precision);
    mpf_class sum = make_ui(0, precision);

    std::uint64_t k = 0;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        mpf_class contribution = mul_ui(term, 2ul, precision);
        if (mpf_cmp(contribution.mpf_data(), threshold.mpf_data()) < 0) {
            return sum;
        }
        sum = add(sum, contribution, precision);
        const std::uint64_t factor =
            checked_taylor_counter_add(checked_taylor_counter_product(std::uint64_t{2}, k), std::uint64_t{2});
        mpf_div_2exp(term.mpf_data(), term.mpf_data(), checked_taylor_shift_count(q_exponent, factor));
        increment_taylor_counter(k);
    }
    throw_iteration_limit_exceeded("theta2_from_power_of_two_q");
}

inline mpf_class compute_log_two_theta_agm(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_log_two(target);
    const mp_bitcnt_t q_exponent = (work / 2) - 2;

    mpf_class theta2 = theta2_from_power_of_two_q(q_exponent, work);
    mpf_class theta3 = theta3_from_power_of_two_q(q_exponent, work);
    mpf_class agm_value = agm_converged(sqr(theta2, work), sqr(theta3, work), work);
    mpf_class q_scale = make_u64(static_cast<std::uint64_t>(q_exponent), work);
    mpf_class denominator = mul(q_scale, agm_value, work);
    return set_prec_copy(div(pi(work), denominator, work), target);
}

struct log_two_cache_state {
    std::mutex mutex;
    mp_bitcnt_t cached_precision{0};
    mpf_class cached_value;
    bool initialized{false};
};

inline log_two_cache_state& log_two_cache()
{
    static log_two_cache_state cache;
    return cache;
}

inline mpf_class log_two(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    if (has_hardcoded_log_two(target)) {
        return hardcoded_log_two(target);
    }

    log_two_cache_state& cache = log_two_cache();
    std::lock_guard<std::mutex> lock(cache.mutex);
    if (!cache.initialized || cache.cached_precision < target) {
        mpf_class computed = compute_log_two_theta_agm(target);
        cache.cached_value.swap(computed);
        cache.cached_precision = target;
        cache.initialized = true;
    }
    return set_prec_copy(cache.cached_value, target);
}

inline mp_bitcnt_t guard_bits_for_log1p(mp_bitcnt_t)
{
    return 160;
}

inline mp_bitcnt_t working_precision_for_log1p(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + guard_bits_for_log1p(target_precision);
}

inline mpf_class log1p_taylor_small(const mpf_class& x, mp_bitcnt_t precision)
{
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    mpf_class sum = set_prec_copy(x, precision);
    mpf_class power = set_prec_copy(x, precision);

    std::uint64_t k = 2;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        power = mul(power, x, precision);
        mpf_class term = div(power, make_u64(k, precision), precision);
        if ((k & std::uint64_t{1}) == std::uint64_t{0}) {
            term = sub(make_ui(0, precision), term, precision);
        }
        sum = add(sum, term, precision);
        if (mpf_cmp(abs_prec(term, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return sum;
        }
        increment_taylor_counter(k);
    }
    throw_iteration_limit_exceeded("log1p_taylor_small");
}

inline mpf_class log1p_atanh_series(const mpf_class& x, mp_bitcnt_t precision)
{
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    mpf_class two = make_ui(2, precision);
    mpf_class y = div(x, add(two, x, precision), precision);
    mpf_class y2 = sqr(y, precision);

    mpf_class sum = set_prec_copy(y, precision);
    mpf_class term = set_prec_copy(y, precision);

    std::uint64_t k = 1;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        term = mul(term, y2, precision);
        const std::uint64_t denominator = checked_taylor_odd_denominator(k);
        mpf_class contribution = div(term, make_u64(denominator, precision), precision);
        sum = add(sum, contribution, precision);
        if (mpf_cmp(abs_prec(contribution, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return mul_ui(sum, 2ul, precision);
        }
        increment_taylor_counter(k);
    }
    throw_iteration_limit_exceeded("log1p_atanh_series");
}

inline mpf_class compute_log1p(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_log1p(target);
    const mpf_class x = set_prec_copy(x_input, work);
    const mpf_class zero = make_ui(0, work);
    const mpf_class one = make_ui(1, work);
    const mpf_class one_plus_x = add(one, x, work);

    if (mpf_cmp(one_plus_x.mpf_data(), zero.mpf_data()) == 0) {
        throw std::domain_error("log1p(x) pole at x = -1");
    }
    if (mpf_cmp(one_plus_x.mpf_data(), zero.mpf_data()) < 0) {
        throw std::domain_error("log1p(x) is undefined for x < -1");
    }
    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) == 0) {
        return make_ui(0, target);
    }

    mpf_class small_threshold = make_ui(1, work);
    mpf_div_2exp(small_threshold.mpf_data(), small_threshold.mpf_data(), work / 2);

    mpf_class result = make_ui(0, work);
    if (mpf_cmp(abs_prec(x, work).mpf_data(), small_threshold.mpf_data()) <= 0) {
        result = log1p_taylor_small(x, work);
    } else {
        result = log1p_atanh_series(x, work);
    }
    return set_prec_copy(result, target);
}

inline mp_bitcnt_t guard_bits_for_log(mp_bitcnt_t)
{
    return 96;
}

inline mp_bitcnt_t log_cancellation_bits(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t probe_precision = target + log_cancellation_probe_guard_bits;
    const mpf_class one = make_ui(1, probe_precision);
    const mpf_class x = set_prec_copy(x_input, probe_precision);
    const mpf_class delta = abs_prec(sub(x, one, probe_precision), probe_precision);

    if (mpf_cmp(delta.mpf_data(), make_ui(0, probe_precision).mpf_data()) == 0) {
        return target;
    }

    mp_exp_t delta_exponent = 0;
    mpf_get_d_2exp(&delta_exponent, delta.mpf_data());
    const long numerator_bits = static_cast<long>(ceil_log2_precision(target)) + 1;
    const long estimate = numerator_bits - delta_exponent + 2;
    return estimate > 0 ? static_cast<mp_bitcnt_t>(estimate) : 0;
}

inline mp_bitcnt_t working_precision_for_log(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) +
           guard_bits_for_log(target_precision) +
           log_cancellation_bits(x_input, target_precision);
}

inline mp_bitcnt_t checked_mp_exp_magnitude(mp_exp_t value)
{
    const std::uintmax_t bit_limit = static_cast<std::uintmax_t>(std::numeric_limits<mp_bitcnt_t>::max());
    std::uintmax_t magnitude = 0;
    if (value >= 0) {
        magnitude = static_cast<std::uintmax_t>(value);
    } else {
        magnitude = static_cast<std::uintmax_t>(-(value + 1));
        ++magnitude;
    }
    if (magnitude > bit_limit) {
        throw std::overflow_error("exp(x) scaling shift exceeds mp_bitcnt_t");
    }
    return static_cast<mp_bitcnt_t>(magnitude);
}

inline mp_exp_t checked_mp_bitcnt_to_mp_exp(mp_bitcnt_t value, const char* message)
{
    if (static_cast<std::uintmax_t>(value) >
        static_cast<std::uintmax_t>(std::numeric_limits<mp_exp_t>::max())) {
        throw std::overflow_error(message);
    }
    return static_cast<mp_exp_t>(value);
}

inline mpf_class mul_signed_exp(const mpf_class& value, mp_exp_t multiplier, mp_bitcnt_t precision)
{
    if (multiplier == 0) {
        return make_ui(0, precision);
    }
    mpf_class factor(checked_mp_exp_magnitude(multiplier), precision);
    mpf_class magnitude = mul(value, factor, precision);
    if (multiplier < 0) {
        return sub(make_ui(0, precision), magnitude, precision);
    }
    return magnitude;
}

inline void mpz_set_mp_exp(mpz_t target, mp_exp_t value)
{
    if constexpr (std::numeric_limits<mp_exp_t>::min() >= std::numeric_limits<long>::min() &&
                  std::numeric_limits<mp_exp_t>::max() <= std::numeric_limits<long>::max()) {
        mpz_set_si(target, static_cast<long>(value));
    } else {
        using unsigned_exp_t = std::make_unsigned_t<mp_exp_t>;
        unsigned_exp_t magnitude = 0;
        if (value >= 0) {
            magnitude = static_cast<unsigned_exp_t>(value);
        } else {
            magnitude = static_cast<unsigned_exp_t>(-(value + 1));
            ++magnitude;
        }
        mpz_import(target, 1, -1, sizeof(magnitude), 0, 0, &magnitude);
        if (value < 0) {
            mpz_neg(target, target);
        }
    }
}

inline mp_exp_t checked_log_scaling_exponent(mp_exp_t desired_exponent, mp_exp_t x_exponent)
{
    mpz_t value;
    mpz_t x_value;
    mpz_init(value);
    mpz_init(x_value);
    mpz_set_mp_exp(value, desired_exponent);
    mpz_set_mp_exp(x_value, x_exponent);
    mpz_sub(value, value, x_value);
    mpz_add_ui(value, value, 1);
    if (!mpz_fits_slong_p(value)) {
        mpz_clear(x_value);
        mpz_clear(value);
        throw std::overflow_error("log(x) scaling exponent is too large");
    }
    const long result = mpz_get_si(value);
    mpz_clear(x_value);
    mpz_clear(value);
    if constexpr (sizeof(mp_exp_t) < sizeof(long)) {
        if (result < static_cast<long>(std::numeric_limits<mp_exp_t>::min()) ||
            result > static_cast<long>(std::numeric_limits<mp_exp_t>::max())) {
            throw std::overflow_error("log(x) scaling exponent is too large");
        }
    }
    return static_cast<mp_exp_t>(result);
}

inline void ensure_mpf_mul_exponent_fits(const mpf_class& lhs, const mpf_class& rhs)
{
    if (mpf_sgn(lhs.mpf_data()) == 0 || mpf_sgn(rhs.mpf_data()) == 0) {
        return;
    }

    mp_exp_t lhs_exponent = 0;
    mp_exp_t rhs_exponent = 0;
    mpf_get_d_2exp(&lhs_exponent, lhs.mpf_data());
    mpf_get_d_2exp(&rhs_exponent, rhs.mpf_data());

    mpz_t sum;
    mpz_t rhs_value;
    mpz_init(sum);
    mpz_init(rhs_value);
    mpz_set_mp_exp(sum, lhs_exponent);
    mpz_set_mp_exp(rhs_value, rhs_exponent);
    mpz_add(sum, sum, rhs_value);
    mpz_clear(rhs_value);

    mpz_t min_value;
    mpz_t max_value;
    mpz_init(min_value);
    mpz_init(max_value);
    mpz_set_mp_exp(min_value, std::numeric_limits<mp_exp_t>::min());
    mpz_set_mp_exp(max_value, std::numeric_limits<mp_exp_t>::max());
    const bool too_large = mpz_cmp(sum, max_value) > 0;
    mpz_sub_ui(sum, sum, 1);
    const bool too_small = mpz_cmp(sum, min_value) < 0;
    mpz_clear(max_value);
    mpz_clear(min_value);
    mpz_clear(sum);

    if (too_large) {
        throw std::overflow_error("mpf multiplication result exponent is too large");
    }
    if (too_small) {
        throw std::overflow_error("mpf multiplication result exponent is too small");
    }
}

inline mpf_class compute_log(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_log(x_input, target);
    const mpf_class x = set_prec_copy(x_input, work);
    const mpf_class zero = make_ui(0, work);
    const mpf_class one = make_ui(1, work);
    const mpf_class two = make_ui(2, work);

    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) == 0) {
        throw std::domain_error("log(x) pole at x = 0");
    }
    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) < 0) {
        throw std::domain_error("log(x) is undefined for x < 0");
    }
    if (mpf_cmp(x.mpf_data(), one.mpf_data()) == 0) {
        return make_ui(0, target);
    }

    const mpf_class delta = sub(x, one, work);
    mpf_class near_one_threshold = make_ui(1, work);
    mpf_div_2exp(near_one_threshold.mpf_data(), near_one_threshold.mpf_data(), 1);
    if (mpf_cmp(abs_prec(delta, work).mpf_data(), near_one_threshold.mpf_data()) < 0) {
        return compute_log1p(delta, target);
    }

    mp_exp_t x_exponent = 0;
    mpf_get_d_2exp(&x_exponent, x.mpf_data());
    const mp_bitcnt_t desired_exponent_bits = work / 2;
    if (desired_exponent_bits > std::numeric_limits<mp_bitcnt_t>::max() - 16) {
        throw std::overflow_error("log(x) scaling exponent is too large");
    }
    const mp_exp_t desired_exponent =
        checked_mp_bitcnt_to_mp_exp(desired_exponent_bits + 16, "log(x) scaling exponent is too large");
    const mp_exp_t exponent = checked_log_scaling_exponent(desired_exponent, x_exponent);

    mpf_class s = set_prec_copy(x, work);
    if (exponent >= 0) {
        const mp_bitcnt_t shift = checked_mp_exp_magnitude(exponent);
        ensure_mpf_shift_result_exponent_fits(
            s.mpf_data(), shift, true, "log(x) scaling exponent is too large");
        mpf_mul_2exp(s.mpf_data(), s.mpf_data(), shift);
    } else {
        const mp_bitcnt_t shift = checked_mp_exp_magnitude(exponent);
        ensure_mpf_shift_result_exponent_fits(
            s.mpf_data(), shift, false, "log(x) scaling exponent is too small");
        mpf_div_2exp(s.mpf_data(), s.mpf_data(), shift);
    }

    mpf_class b = div(make_ui(4, work), s, work);
    mpf_class agm_value = agm_converged(one, b, work);
    mpf_class leading = div(pi(work), mul(two, agm_value, work), work);
    mpf_class correction = mul_signed_exp(log_two(work), exponent, work);
    return set_prec_copy(sub(leading, correction, work), target);
}

inline mp_bitcnt_t guard_bits_for_exp(mp_bitcnt_t)
{
    return 96;
}

inline mp_bitcnt_t working_precision_for_exp(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + guard_bits_for_exp(target_precision);
}

inline void ensure_exp_scaling_exponent_fits(const mpf_class& value, mp_exp_t shift)
{
    mp_exp_t exponent = 0;
    mpf_get_d_2exp(&exponent, value.mpf_data());
    if (shift > 0) {
        if (exponent > std::numeric_limits<mp_exp_t>::max() - shift) {
            throw std::overflow_error("exp(x) result exponent is too large");
        }
    } else if (shift < 0) {
        if (exponent < std::numeric_limits<mp_exp_t>::min() - shift) {
            throw std::overflow_error("exp(x) result exponent is too small");
        }
    }
}

inline mp_exp_t round_to_nearest_mp_exp(const mpf_class& value, mp_bitcnt_t precision)
{
    const mpf_class zero = make_ui(0, precision);
    mpf_class half = make_ui(1, precision);
    mpf_div_2exp(half.mpf_data(), half.mpf_data(), 1);
    mpf_class adjusted = set_prec_copy(value, precision);
    if (mpf_cmp(adjusted.mpf_data(), zero.mpf_data()) >= 0) {
        adjusted = add(adjusted, half, precision);
    } else {
        adjusted = sub(adjusted, half, precision);
    }

    mpz_class rounded_integer;
    mpz_set_f(rounded_integer.mpz_data(), adjusted.mpf_data());
    if (!mpz_fits_slong_p(rounded_integer.mpz_data())) {
        throw std::overflow_error("exp(x) scaling exponent is too large");
    }
    const long rounded = mpz_get_si(rounded_integer.mpz_data());
    if constexpr (sizeof(mp_exp_t) < sizeof(long)) {
        if (rounded < static_cast<long>(std::numeric_limits<mp_exp_t>::min()) ||
            rounded > static_cast<long>(std::numeric_limits<mp_exp_t>::max())) {
            throw std::overflow_error("exp(x) scaling exponent is too large");
        }
    }
    return static_cast<mp_exp_t>(rounded);
}

inline mpf_class exp_taylor_reduced(const mpf_class& x, mp_bitcnt_t precision)
{
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    mpf_class sum = make_ui(1, precision);
    mpf_class term = make_ui(1, precision);
    std::uint64_t n = 1;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        term = div(mul(term, x, precision), make_u64(n, precision), precision);
        sum = add(sum, term, precision);
        if (mpf_cmp(abs_prec(term, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return sum;
        }
        increment_taylor_counter(n);
    }
    throw_iteration_limit_exceeded("exp_taylor_reduced");
}

inline mpf_class compute_exp(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_exp(target);
    const mpf_class x = set_prec_copy(x_input, work);
    const mpf_class zero = make_ui(0, work);

    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) == 0) {
        return make_ui(1, target);
    }

    const mpf_class log2_value = log_two(work);
    const mpf_class quotient = div(x, log2_value, work);
    const mp_exp_t k = round_to_nearest_mp_exp(quotient, work);
    const mpf_class reduced = sub(x, mul_signed_exp(log2_value, k, work), work);

    mpf_class result = exp_taylor_reduced(reduced, work);
    ensure_exp_scaling_exponent_fits(result, k);
    if (k >= 0) {
        mpf_mul_2exp(result.mpf_data(), result.mpf_data(), checked_mp_exp_magnitude(k));
    } else {
        mpf_div_2exp(result.mpf_data(), result.mpf_data(), checked_mp_exp_magnitude(k));
    }
    return set_prec_copy(result, target);
}

inline mp_bitcnt_t guard_bits_for_expm1(mp_bitcnt_t)
{
    return 96;
}

inline mp_bitcnt_t working_precision_for_expm1(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + guard_bits_for_expm1(target_precision);
}

inline mpf_class expm1_taylor_small(const mpf_class& x, mp_bitcnt_t precision)
{
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    mpf_class sum = set_prec_copy(x, precision);
    mpf_class term = set_prec_copy(x, precision);
    std::uint64_t n = 2;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        term = div(mul(term, x, precision), make_u64(n, precision), precision);
        sum = add(sum, term, precision);
        if (mpf_cmp(abs_prec(term, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return sum;
        }
        increment_taylor_counter(n);
    }
    throw_iteration_limit_exceeded("expm1_taylor_small");
}

inline mpf_class compute_expm1(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_expm1(target);
    const mpf_class x = set_prec_copy(x_input, work);

    if (mpf_cmp(x.mpf_data(), make_ui(0, work).mpf_data()) == 0) {
        return make_ui(0, target);
    }

    mpf_class small_threshold = make_ui(1, work);
    mpf_div_2exp(small_threshold.mpf_data(), small_threshold.mpf_data(), work / 2);

    mpf_class result = make_ui(0, work);
    if (mpf_cmp(abs_prec(x, work).mpf_data(), small_threshold.mpf_data()) <= 0) {
        result = expm1_taylor_small(x, work);
    } else {
        result = sub(compute_exp(x, work), make_ui(1, work), work);
    }
    return set_prec_copy(result, target);
}

inline mpf_class log_ten(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = target + guard_bits_for_log(target) + 8;
    return set_prec_copy(compute_log(make_ui(10, work), work), target);
}

inline mp_bitcnt_t guard_bits_for_trig(mp_bitcnt_t)
{
    return 128;
}

inline mp_bitcnt_t working_precision_for_trig(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + guard_bits_for_trig(target_precision);
}

inline mp_bitcnt_t trig_constant_precision(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    if (target > (std::numeric_limits<mp_bitcnt_t>::max() - 64) / 2) {
        throw std::overflow_error("MPF trigonometric constant precision exceeds mp_bitcnt_t");
    }
    return (2 * target) + 64;
}

inline mp_bitcnt_t checked_trig_precision_add(mp_bitcnt_t lhs, mp_bitcnt_t rhs)
{
    if (rhs > std::numeric_limits<mp_bitcnt_t>::max() - lhs) {
        throw std::overflow_error("MPF trigonometric argument reduction precision exceeds mp_bitcnt_t");
    }
    return lhs + rhs;
}

inline mp_bitcnt_t trig_constant_precision_for_argument(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    mp_bitcnt_t precision = trig_constant_precision(target);
    if (mpf_sgn(x_input.mpf_data()) == 0) {
        return precision;
    }

    mp_exp_t x_exponent = 0;
    mpf_get_d_2exp(&x_exponent, x_input.mpf_data());
    if (x_exponent <= 0) {
        return precision;
    }

    mp_bitcnt_t required = checked_trig_precision_add(target, guard_bits_for_trig(target));
    required = checked_trig_precision_add(required, checked_mp_exp_magnitude(x_exponent));
    return std::max(precision, required);
}

inline trig_constant_cache_state& trig_constant_cache()
{
    static trig_constant_cache_state cache;
    return cache;
}

inline sincos_result sincos_taylor_small(const mpf_class& x, mp_bitcnt_t precision)
{
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    sincos_result result(precision);
    result.sin_value = set_prec_copy(x, precision);
    result.cos_value = make_ui(1, precision);

    mpf_class x2 = sqr(x, precision);
    mpf_class sin_term = set_prec_copy(x, precision);
    mpf_class cos_term = make_ui(1, precision);

    std::uint64_t k = 1;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        const std::uint64_t sin_den1 = checked_taylor_counter_product(std::uint64_t{2}, k);
        const std::uint64_t sin_den2 = checked_taylor_counter_add(sin_den1, std::uint64_t{1});
        const std::uint64_t sin_den = checked_taylor_counter_product(sin_den1, sin_den2);
        sin_term = div(mul(sin_term, x2, precision), make_u64(sin_den, precision), precision);
        sin_term = sub(make_ui(0, precision), sin_term, precision);
        result.sin_value = add(result.sin_value, sin_term, precision);

        const std::uint64_t cos_den1 = sin_den1 - 1u;
        const std::uint64_t cos_den2 = sin_den1;
        const std::uint64_t cos_den = checked_taylor_counter_product(cos_den1, cos_den2);
        cos_term = div(mul(cos_term, x2, precision), make_u64(cos_den, precision), precision);
        cos_term = sub(make_ui(0, precision), cos_term, precision);
        result.cos_value = add(result.cos_value, cos_term, precision);

        if (mpf_cmp(abs_prec(sin_term, precision).mpf_data(), epsilon.mpf_data()) < 0 &&
            mpf_cmp(abs_prec(cos_term, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return result;
        }
        increment_taylor_counter(k);
    }
    throw_iteration_limit_exceeded("sincos_taylor_small");
}

inline void ensure_trig_constants_at_precision(mp_bitcnt_t cache_precision)
{
    cache_precision = normalize_target_precision(cache_precision);
    trig_constant_cache_state& cache = trig_constant_cache();
    std::lock_guard<std::mutex> lock(cache.mutex);
    if (!cache.initialized || cache.cached_precision < cache_precision) {
        mpf_class pi_value = compute_pi_gauss_legendre(cache_precision);
        cache.pi_value.swap(pi_value);

        mpf_class pi_over_two_value = set_prec_copy(cache.pi_value, cache_precision);
        mpf_div_2exp(pi_over_two_value.mpf_data(), pi_over_two_value.mpf_data(), 1);
        cache.pi_over_two_value.swap(pi_over_two_value);

        mpf_class two_over_pi_value = div(make_ui(2, cache_precision), cache.pi_value, cache_precision);
        cache.two_over_pi_value.swap(two_over_pi_value);
        cache.cached_precision = cache_precision;
        cache.initialized = true;
    }
}

inline void ensure_trig_constants(mp_bitcnt_t target_precision)
{
    ensure_trig_constants_at_precision(trig_constant_precision(target_precision));
}

inline mpf_class trig_pi_over_two_at_precision(mp_bitcnt_t precision)
{
    ensure_trig_constants_at_precision(precision);
    trig_constant_cache_state& cache = trig_constant_cache();
    std::lock_guard<std::mutex> lock(cache.mutex);
    return set_prec_copy(cache.pi_over_two_value, precision);
}

inline mpf_class trig_pi_over_two(mp_bitcnt_t target_precision)
{
    return trig_pi_over_two_at_precision(trig_constant_precision(target_precision));
}

inline mpf_class trig_two_over_pi_at_precision(mp_bitcnt_t precision)
{
    ensure_trig_constants_at_precision(precision);
    trig_constant_cache_state& cache = trig_constant_cache();
    std::lock_guard<std::mutex> lock(cache.mutex);
    return set_prec_copy(cache.two_over_pi_value, precision);
}

inline mpf_class trig_two_over_pi(mp_bitcnt_t target_precision)
{
    return trig_two_over_pi_at_precision(trig_constant_precision(target_precision));
}

inline sincos_result compute_sincos(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_trig(target);
    const mpf_class zero = make_ui(0, work);
    const mp_bitcnt_t const_precision = trig_constant_precision_for_argument(x_input, target);
    const mpf_class pio2 = trig_pi_over_two_at_precision(const_precision);
    const mpf_class two_over_pi = trig_two_over_pi_at_precision(const_precision);

    const mpf_class scaled_x = set_prec_copy(x_input, const_precision);
    const mpf_class q = mul(scaled_x, two_over_pi, const_precision);
    mpz_class k(std::int64_t{0});
    mpz_set_f(k.mpz_data(), q.mpf_data());
    mpf_class integer_part = mpf_class::with_precision(const_precision);
    mpf_set_z(integer_part.mpf_data(), k.mpz_data());
    const mpf_class frac = sub(q, integer_part, const_precision);
    const mpf_class half = div(make_ui(1, const_precision), make_ui(2, const_precision), const_precision);
    const mpf_class neg_half = sub(make_ui(0, const_precision), half, const_precision);
    const int frac_vs_half = mpf_cmp(frac.mpf_data(), half.mpf_data());
    const int frac_vs_neg_half = mpf_cmp(frac.mpf_data(), neg_half.mpf_data());
    if (frac_vs_half > 0) {
        mpz_add_ui(k.mpz_data(), k.mpz_data(), 1);
    } else if (frac_vs_half == 0) {
        if (mpz_odd_p(k.mpz_data())) {
            mpz_add_ui(k.mpz_data(), k.mpz_data(), 1);
        }
    } else if (frac_vs_neg_half < 0) {
        mpz_sub_ui(k.mpz_data(), k.mpz_data(), 1);
    } else if (frac_vs_neg_half == 0) {
        if (mpz_odd_p(k.mpz_data())) {
            mpz_sub_ui(k.mpz_data(), k.mpz_data(), 1);
        }
    }

    mpf_class k_mpf = mpf_class::with_precision(const_precision);
    mpf_set_z(k_mpf.mpf_data(), k.mpz_data());
    const mpf_class remainder_hi = sub(scaled_x, mul(pio2, k_mpf, const_precision), const_precision);
    const mpf_class reduced_argument = set_prec_copy(remainder_hi, work);
    const sincos_result base = sincos_taylor_small(reduced_argument, work);

    const unsigned long quadrant = mpz_fdiv_ui(k.mpz_data(), 4ul);
    sincos_result result(work);
    switch (quadrant) {
    case 0:
        result.sin_value = set_prec_copy(base.sin_value, work);
        result.cos_value = set_prec_copy(base.cos_value, work);
        break;
    case 1:
        result.sin_value = set_prec_copy(base.cos_value, work);
        result.cos_value = set_prec_copy(sub(zero, base.sin_value, work), work);
        break;
    case 2:
        result.sin_value = set_prec_copy(sub(zero, base.sin_value, work), work);
        result.cos_value = set_prec_copy(sub(zero, base.cos_value, work), work);
        break;
    default:
        result.sin_value = set_prec_copy(sub(zero, base.cos_value, work), work);
        result.cos_value = set_prec_copy(base.sin_value, work);
        break;
    }

    mpf_class sin_value = set_prec_copy(result.sin_value, target);
    mpf_class cos_value = set_prec_copy(result.cos_value, target);
    result.sin_value.swap(sin_value);
    result.cos_value.swap(cos_value);
    return result;
}

inline mpf_class compute_sin(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    return compute_sincos(x_input, target_precision).sin_value;
}

inline mpf_class compute_cos(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    return compute_sincos(x_input, target_precision).cos_value;
}

inline mp_bitcnt_t guard_bits_for_atan(mp_bitcnt_t)
{
    return 96;
}

inline mp_bitcnt_t working_precision_for_atan(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + guard_bits_for_atan(target_precision);
}

inline mpf_class atan_taylor_small(const mpf_class& x, mp_bitcnt_t precision)
{
    mpf_class epsilon = make_ui(1, precision);
    mpf_div_2exp(epsilon.mpf_data(), epsilon.mpf_data(), precision);

    const mpf_class x2 = sqr(x, precision);
    mpf_class sum = set_prec_copy(x, precision);
    mpf_class power = set_prec_copy(x, precision);

    std::uint64_t k = 1;
    const std::uint64_t max_iterations = iteration_limit_for_precision(precision);
    for (std::uint64_t iteration = 0; iteration < max_iterations; ++iteration) {
        power = mul(power, x2, precision);
        mpf_class contribution = div(power, make_u64(checked_taylor_odd_denominator(k), precision), precision);
        if ((k & std::uint64_t{1}) == std::uint64_t{1}) {
            contribution = sub(make_ui(0, precision), contribution, precision);
        }
        sum = add(sum, contribution, precision);
        if (mpf_cmp(abs_prec(contribution, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return sum;
        }
        increment_taylor_counter(k);
    }
    throw_iteration_limit_exceeded("atan_taylor_small");
}

inline mpf_class compute_atan(const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_atan(target);
    const mpf_class x = set_prec_copy(x_input, work);
    const mpf_class zero = make_ui(0, work);
    const mpf_class one = make_ui(1, work);

    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) == 0) {
        return make_ui(0, target);
    }

    const bool negate = mpf_cmp(x.mpf_data(), zero.mpf_data()) < 0;
    const mpf_class ax = negate ? sub(zero, x, work) : x;

    mpf_class y = set_prec_copy(ax, work);
    std::uint64_t reductions = 0;
    const mpf_class threshold = div(one, make_ui(2, work), work);
    while (mpf_cmp(y.mpf_data(), threshold.mpf_data()) > 0) {
        if (reductions >= iteration_limit_for_precision(work)) {
            throw_iteration_limit_exceeded("compute_atan argument reduction");
        }
        mpf_class sqrt_term = sqrt_prec(add(one, sqr(y, work), work), work);
        y = div(y, add(one, sqrt_term, work), work);
        ++reductions;
    }

    mpf_class result = atan_taylor_small(y, work);
    for (std::uint64_t i = 0; i < reductions; ++i) {
        result = mul_ui(result, 2ul, work);
    }

    if (negate) {
        result = sub(zero, result, work);
    }
    return set_prec_copy(result, target);
}

inline mpf_class compute_atan2(const mpf_class& y_input, const mpf_class& x_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_atan(target);
    const mpf_class y = set_prec_copy(y_input, work);
    const mpf_class x = set_prec_copy(x_input, work);
    const mpf_class zero = make_ui(0, work);

    if (mpf_cmp(y.mpf_data(), zero.mpf_data()) == 0) {
        if (mpf_cmp(x.mpf_data(), zero.mpf_data()) > 0) {
            return make_ui(0, target);
        }
        if (mpf_cmp(x.mpf_data(), zero.mpf_data()) < 0) {
            return set_prec_copy(pi(target + 2), target);
        }
        return make_ui(0, target);
    }

    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) == 0) {
        mpf_class pio2 = pi(target + 2);
        mpf_div_2exp(pio2.mpf_data(), pio2.mpf_data(), 1);
        if (mpf_cmp(y.mpf_data(), zero.mpf_data()) > 0) {
            return set_prec_copy(pio2, target);
        }
        return set_prec_copy(sub(make_ui(0, target + 2), pio2, target + 2), target);
    }

    const mpf_class ratio = div(y, x, work);
    mpf_class result = compute_atan(ratio, work);
    const mpf_class pi_value = pi(work);
    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) < 0) {
        if (mpf_cmp(y.mpf_data(), zero.mpf_data()) > 0) {
            result = add(result, pi_value, work);
        } else {
            result = sub(result, pi_value, work);
        }
    }
    return set_prec_copy(result, target);
}

inline mp_bitcnt_t guard_bits_for_pow(mp_bitcnt_t)
{
    return 96;
}

inline mp_bitcnt_t working_precision_for_pow(mp_bitcnt_t target_precision)
{
    return normalize_target_precision(target_precision) + guard_bits_for_pow(target_precision);
}

inline bool mpf_is_exact_integer(const mpf_class& x, mpz_class& integer_value)
{
    mpz_set_f(integer_value.mpz_data(), x.mpf_data());
    mpf_class rounded = mpf_class::with_precision(x.precision());
    mpf_set_z(rounded.mpf_data(), integer_value.mpz_data());
    return mpf_cmp(rounded.mpf_data(), x.mpf_data()) == 0;
}

inline mpf_class pow_integer_unsigned(const mpf_class& base_input, const mpz_class& exponent, mp_bitcnt_t precision)
{
    mpf_class result = make_ui(1, precision);
    mpf_class base = set_prec_copy(base_input, precision);
    mpz_class e = exponent;

    while (mpz_sgn(e.mpz_data()) > 0) {
        if (mpz_odd_p(e.mpz_data())) {
            ensure_mpf_mul_exponent_fits(result, base);
            result = mul(result, base, precision);
        }
        mpz_fdiv_q_2exp(e.mpz_data(), e.mpz_data(), 1);
        if (mpz_sgn(e.mpz_data()) > 0) {
            ensure_mpf_mul_exponent_fits(base, base);
            base = sqr(base, precision);
        }
    }
    return result;
}

inline mpf_class compute_pow(const mpf_class& x_input, const mpf_class& y_input, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_pow(target);
    const mpf_class x = set_prec_copy(x_input, work);
    const mpf_class y = set_prec_copy(y_input, work);
    const mpf_class zero = make_ui(0, work);
    const mpf_class one = make_ui(1, work);

    if (mpf_cmp(y.mpf_data(), zero.mpf_data()) == 0) {
        return make_ui(1, target);
    }

    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) == 0) {
        if (mpf_cmp(y.mpf_data(), zero.mpf_data()) > 0) {
            return make_ui(0, target);
        }
        throw std::domain_error("pow(0, y) cannot return infinity for y < 0");
    }

    mpz_class integer_exponent;
    if (mpf_is_exact_integer(y, integer_exponent)) {
        const bool negative_exponent = mpz_sgn(integer_exponent.mpz_data()) < 0;
        if (negative_exponent) {
            mpz_neg(integer_exponent.mpz_data(), integer_exponent.mpz_data());
        }
        mpf_class magnitude = pow_integer_unsigned(x, integer_exponent, work);
        if (negative_exponent) {
            magnitude = div(one, magnitude, work);
        }
        return set_prec_copy(magnitude, target);
    }

    if (mpf_cmp(x.mpf_data(), zero.mpf_data()) < 0) {
        throw std::domain_error("pow(x, y) is undefined for x < 0 and non-integer y");
    }

    mpf_class exponent_product = mul(y, compute_log(x, work), work);
    return set_prec_copy(compute_exp(exponent_product, work), target);
}

inline int gamma_spouge_terms(mp_bitcnt_t precision)
{
    constexpr long double log2_two_pi = 2.6514961294723187820171016809L;
    const long double requested_bits = static_cast<long double>(normalize_target_precision(precision)) +
                                       static_cast<long double>(guard_bits_for_log(precision)) + 32.0L;
    int terms = static_cast<int>(std::ceil(requested_bits / log2_two_pi));
    if (terms < 24) {
        terms = 24;
    }
    return terms;
}

inline mpf_class gamma_spouge_coefficient(int k, int a, mp_bitcnt_t work)
{
    if (k == 0) {
        return sqrt_prec(mul_ui(pi(work + 8), 2ul, work), work);
    }
    if (k < 0 || a <= k) {
        throw std::invalid_argument("Spouge coefficient requires 0 <= k < a");
    }

    const mpf_class a_minus_k = make_ui(static_cast<unsigned long>(a - k), work);
    const mpf_class exponent = sub(make_ui(static_cast<unsigned long>(k), work), mpf_class("0.5", work), work);
    mpf_class coefficient = mul(compute_pow(a_minus_k, exponent, work), compute_exp(a_minus_k, work), work);

    mpf_class factorial = make_ui(1, work);
    for (int n = 2; n <= k - 1; ++n) {
        factorial = mul(factorial, make_ui(static_cast<unsigned long>(n), work), work);
    }
    coefficient = div(coefficient, factorial, work);
    if ((k % 2) == 0) {
        coefficient = sub(make_ui(0, work), coefficient, work);
    }
    return coefficient;
}

inline bool gamma_real_pole(const mpf_class& x)
{
    mpz_class integer_value;
    if (!mpf_is_exact_integer(x, integer_value)) {
        return false;
    }
    return mpz_sgn(integer_value.mpz_data()) <= 0;
}

inline mpf_class gamma_spouge_positive(const mpf_class& x, mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const int a = gamma_spouge_terms(target);
    const mp_bitcnt_t work = target + guard_bits_for_log(target) + 32 + static_cast<mp_bitcnt_t>(a);
    const mpf_class x_work = set_prec_copy(x, work);
    const mpf_class half("0.5", work);
    const mpf_class base = add(x_work, make_ui(static_cast<unsigned long>(a - 1), work), work);

    mpf_class sum = gamma_spouge_coefficient(0, a, work);
    for (int k = 1; k < a; ++k) {
        const mpf_class denominator = add(x_work, make_ui(static_cast<unsigned long>(k - 1), work), work);
        sum = add(sum, div(gamma_spouge_coefficient(k, a, work), denominator, work), work);
    }

    mpf_class result = mul(
        mul(compute_pow(base, sub(x_work, half, work), work),
            compute_exp(sub(make_ui(0, work), base, work), work),
            work),
        sum,
        work);
    return set_prec_copy(result, target);
}

} // namespace mpf_math_detail

inline mpf_class pi(mp_bitcnt_t target_precision)
{
    return mpf_math_detail::pi(target_precision);
}

inline mpf_class const_pi()
{
    return pi(default_mpf_precision_bits());
}

inline mpf_class const_pi(mp_bitcnt_t target_precision)
{
    return pi(target_precision);
}

inline mpf_class log_two(mp_bitcnt_t target_precision)
{
    return mpf_math_detail::log_two(target_precision);
}

inline mpf_class const_log2()
{
    return log_two(default_mpf_precision_bits());
}

inline mpf_class const_log2(mp_bitcnt_t target_precision)
{
    return log_two(target_precision);
}

inline mpf_class e(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(target_precision);
    return mpf_math_detail::compute_exp(mpf_class(1, target), target);
}

inline mpf_class const_e()
{
    return e(default_mpf_precision_bits());
}

inline mpf_class const_e(mp_bitcnt_t target_precision)
{
    return e(target_precision);
}

inline mpf_class inv_log_two(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(target_precision);
    return mpf_math_detail::div(mpf_class(1, target), log_two(target), target);
}

inline mpf_class inv_log_two()
{
    return inv_log_two(default_mpf_precision_bits());
}

inline mpf_class log_ten(mp_bitcnt_t target_precision)
{
    return mpf_math_detail::log_ten(target_precision);
}

inline mpf_class const_log10()
{
    return log_ten(default_mpf_precision_bits());
}

inline mpf_class const_log10(mp_bitcnt_t target_precision)
{
    return log_ten(target_precision);
}

inline mpf_class pi_over_two(mp_bitcnt_t target_precision)
{
    mpf_class result = pi(target_precision);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return result;
}

inline mpf_class pi_over_two()
{
    return pi_over_two(default_mpf_precision_bits());
}

inline mpf_class pi_over_four(mp_bitcnt_t target_precision)
{
    mpf_class result = pi(target_precision);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 2);
    return result;
}

inline mpf_class pi_over_four()
{
    return pi_over_four(default_mpf_precision_bits());
}

inline mpf_class two_pi(mp_bitcnt_t target_precision)
{
    mpf_class result = pi(target_precision);
    mpf_mul_2exp(result.mpf_data(), result.mpf_data(), 1);
    return result;
}

inline mpf_class two_pi()
{
    return two_pi(default_mpf_precision_bits());
}

inline mpf_class sqrt(const mpf_class& value)
{
    return mpf_math_detail::sqrt_prec(value, value.precision());
}

inline mpf_class abs(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_abs(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class neg(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_neg(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class ceil(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_ceil(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class floor(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_floor(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class trunc(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_trunc(result.mpf_data(), value.mpf_data());
    return result;
}

inline int sgn(const mpf_class& value)
{
    return mpf_sgn(value.mpf_data());
}

inline mpf_class hypot(const mpf_class& lhs, const mpf_class& rhs)
{
    const mp_bitcnt_t precision = std::max(lhs.precision(), rhs.precision());
    return mpf_math_detail::scaled_hypot_abs(lhs, rhs, precision);
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Rhs>, int> = 0>
inline mpf_class hypot(const mpf_class& lhs, Rhs rhs)
{
    return hypot(lhs, mpf_class(rhs, lhs.precision()));
}

template <typename Lhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Lhs>, int> = 0>
inline mpf_class hypot(Lhs lhs, const mpf_class& rhs)
{
    return hypot(mpf_class(lhs, rhs.precision()), rhs);
}

inline mpf_class mpf_remainder(const mpf_class& lhs, const mpf_class& rhs, mpz_class* quotient)
{
    if (mpf_sgn(rhs.mpf_data()) == 0) {
        throw std::domain_error("mpf_remainder division by zero");
    }
    const mp_bitcnt_t precision = std::max(lhs.precision(), rhs.precision());
    mpf_class qf = mpf_math_detail::div(lhs, rhs, precision);
    mpf_floor(qf.mpf_data(), qf.mpf_data());
    mpz_class q(qf);
    if (quotient != nullptr) {
        *quotient = q;
    }
    return mpf_math_detail::sub(lhs, mpf_math_detail::mul(mpf_class(q), rhs, precision), precision);
}

inline mpf_class mpf_remainder(const mpf_class& lhs, const mpf_class& rhs)
{
    return mpf_remainder(lhs, rhs, nullptr);
}

inline mpf_class exp(const mpf_class& value)
{
    return mpf_math_detail::compute_exp(value, value.precision());
}

inline mpf_class log(const mpf_class& value)
{
    return mpf_math_detail::compute_log(value, value.precision());
}

inline mpf_class log2(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_log(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::div(
            mpf_math_detail::compute_log(value_work, work),
            mpf_math_detail::log_two(work),
            work),
        target);
}

inline mpf_class log10(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_log(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::div(
            mpf_math_detail::compute_log(value_work, work),
            mpf_math_detail::log_ten(work),
            work),
        target);
}

inline mpf_class log1p(const mpf_class& value)
{
    return mpf_math_detail::compute_log1p(value, value.precision());
}

inline mpf_class exp2(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_exp(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::compute_exp(
            mpf_math_detail::mul(value_work, mpf_math_detail::log_two(work), work),
            work),
        target);
}

inline mpf_class exp10(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_exp(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::compute_exp(
            mpf_math_detail::mul(value_work, mpf_math_detail::log_ten(work), work),
            work),
        target);
}

inline mpf_class expm1(const mpf_class& value)
{
    return mpf_math_detail::compute_expm1(value, value.precision());
}

inline mpf_class sin(const mpf_class& value)
{
    return mpf_math_detail::compute_sin(value, value.precision());
}

inline mpf_class cos(const mpf_class& value)
{
    return mpf_math_detail::compute_cos(value, value.precision());
}

inline mpf_class tan(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_trig(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::div(
            mpf_math_detail::compute_sin(value_work, work),
            mpf_math_detail::compute_cos(value_work, work),
            work),
        target);
}

inline mpf_class asin(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_trig(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class zero = mpf_math_detail::make_ui(0, work);
    const mpf_class one = mpf_math_detail::make_ui(1, work);
    if (mpf_cmp(value_work.mpf_data(), one.mpf_data()) > 0 ||
        mpf_cmp(value_work.mpf_data(), mpf_math_detail::sub(zero, one, work).mpf_data()) < 0) {
        throw std::domain_error("asin(x) is undefined for |x| > 1");
    }
    mpf_class radicand = mpf_math_detail::sub(
        one, mpf_math_detail::sqr(value_work, work), work);
    if (mpf_cmp(radicand.mpf_data(), zero.mpf_data()) < 0) {
        radicand = zero;
    }
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::compute_atan2(
            value_work, mpf_math_detail::sqrt_prec(radicand, work), work),
        target);
}

inline mpf_class acos(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_trig(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class zero = mpf_math_detail::make_ui(0, work);
    const mpf_class one = mpf_math_detail::make_ui(1, work);
    if (mpf_cmp(value_work.mpf_data(), one.mpf_data()) > 0 ||
        mpf_cmp(value_work.mpf_data(), mpf_math_detail::sub(zero, one, work).mpf_data()) < 0) {
        throw std::domain_error("acos(x) is undefined for |x| > 1");
    }
    mpf_class radicand = mpf_math_detail::sub(
        one, mpf_math_detail::sqr(value_work, work), work);
    if (mpf_cmp(radicand.mpf_data(), zero.mpf_data()) < 0) {
        radicand = zero;
    }
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::compute_atan2(
            mpf_math_detail::sqrt_prec(radicand, work), value_work, work),
        target);
}

inline mpf_class atan(const mpf_class& value)
{
    return mpf_math_detail::compute_atan(value, value.precision());
}

inline mpf_class sinh(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_exp(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class exp_x = mpf_math_detail::compute_exp(value_work, work);
    const mpf_class exp_neg_x = mpf_math_detail::compute_exp(
        mpf_math_detail::sub(mpf_math_detail::make_ui(0, work), value_work, work), work);
    mpf_class result = mpf_math_detail::sub(exp_x, exp_neg_x, work);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return mpf_math_detail::set_prec_copy(result, target);
}

inline mpf_class cosh(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_exp(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class exp_x = mpf_math_detail::compute_exp(value_work, work);
    const mpf_class exp_neg_x = mpf_math_detail::compute_exp(
        mpf_math_detail::sub(mpf_math_detail::make_ui(0, work), value_work, work), work);
    mpf_class result = mpf_math_detail::add(exp_x, exp_neg_x, work);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return mpf_math_detail::set_prec_copy(result, target);
}

inline mpf_class tanh(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_exp(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::div(sinh(value_work), cosh(value_work), work),
        target);
}

inline mpf_class atan2(const mpf_class& y, const mpf_class& x)
{
    return mpf_math_detail::compute_atan2(y, x, std::max(y.precision(), x.precision()));
}

inline mpf_class asinh(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_log(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class one = mpf_math_detail::make_ui(1, work);
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::compute_log(
            mpf_math_detail::add(
                value_work,
                mpf_math_detail::sqrt_prec(
                    mpf_math_detail::add(mpf_math_detail::sqr(value_work, work), one, work),
                    work),
                work),
            work),
        target);
}

inline mpf_class acosh(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_log(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class one = mpf_math_detail::make_ui(1, work);
    if (mpf_cmp(value_work.mpf_data(), one.mpf_data()) < 0) {
        throw std::domain_error("acosh(x) is undefined for x < 1");
    }
    return mpf_math_detail::set_prec_copy(
        mpf_math_detail::compute_log(
            mpf_math_detail::add(
                value_work,
                mpf_math_detail::sqrt_prec(
                    mpf_math_detail::sub(mpf_math_detail::sqr(value_work, work), one, work),
                    work),
                work),
            work),
        target);
}

inline mpf_class atanh(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    const mp_bitcnt_t work = target + mpf_math_detail::guard_bits_for_log1p(target) + 8;
    const mpf_class value_work = mpf_math_detail::set_prec_copy(value, work);
    const mpf_class one = mpf_math_detail::make_ui(1, work);
    const mpf_class neg_one = mpf_math_detail::sub(mpf_math_detail::make_ui(0, work), one, work);
    if (mpf_cmp(value_work.mpf_data(), neg_one.mpf_data()) <= 0 ||
        mpf_cmp(value_work.mpf_data(), one.mpf_data()) >= 0) {
        throw std::domain_error("atanh(x) is undefined for |x| >= 1");
    }
    mpf_class result = mpf_math_detail::compute_log1p(
        mpf_math_detail::div(
            mpf_math_detail::mul_ui(value_work, 2ul, work),
            mpf_math_detail::sub(one, value_work, work),
            work),
        work);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return mpf_math_detail::set_prec_copy(result, target);
}

inline mpf_class pow(const mpf_class& base, const mpf_class& exponent)
{
    return mpf_math_detail::compute_pow(base, exponent, std::max(base.precision(), exponent.precision()));
}

inline mpf_class gamma(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    if (mpf_math_detail::gamma_real_pole(value)) {
        throw std::domain_error("gamma(x) pole at non-positive integer");
    }

    const mpf_class half("0.5", target);
    if (mpf_cmp(value.mpf_data(), half.mpf_data()) < 0) {
        const mpf_class pi_value = pi(target);
        const mpf_class pi_x = mpf_math_detail::mul(pi_value, mpf_math_detail::set_prec_copy(value, target), target);
        const mpf_class sin_factor = sin(pi_x);
        if (mpf_cmp_ui(sin_factor.mpf_data(), 0) == 0) {
            throw std::domain_error("gamma(x) pole at non-positive integer");
        }
        const mpf_class one_minus_x = mpf_math_detail::sub(
            mpf_math_detail::make_ui(1, target), mpf_math_detail::set_prec_copy(value, target), target);
        return mpf_math_detail::div(
            pi_value,
            mpf_math_detail::mul(sin_factor, gamma(one_minus_x), target),
            target);
    }

    return mpf_math_detail::gamma_spouge_positive(value, target);
}

inline mpf_class reciprocal_gamma(const mpf_class& value)
{
    const mp_bitcnt_t target = mpf_math_detail::normalize_target_precision(value.precision());
    if (mpf_math_detail::gamma_real_pole(value)) {
        return mpf_math_detail::make_ui(0, target);
    }
    return mpf_math_detail::div(mpf_math_detail::make_ui(1, target), gamma(value), target);
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class sqrt(const Expr& expr)
{
    return sqrt(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class abs(const Expr& expr)
{
    return abs(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class exp(const Expr& expr)
{
    return exp(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class log(const Expr& expr)
{
    return log(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class log2(const Expr& expr)
{
    return log2(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class log10(const Expr& expr)
{
    return log10(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class log1p(const Expr& expr)
{
    return log1p(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class exp2(const Expr& expr)
{
    return exp2(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class exp10(const Expr& expr)
{
    return exp10(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class expm1(const Expr& expr)
{
    return expm1(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class sin(const Expr& expr)
{
    return sin(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class cos(const Expr& expr)
{
    return cos(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class tan(const Expr& expr)
{
    return tan(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class asin(const Expr& expr)
{
    return asin(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class acos(const Expr& expr)
{
    return acos(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class atan(const Expr& expr)
{
    return atan(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class atan2(const mpf_class& y, const Expr& x)
{
    return atan2(y, mpf_class(x));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class atan2(const Expr& y, const mpf_class& x)
{
    return atan2(mpf_class(y), x);
}

template <
    typename YExpr,
    typename XExpr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<YExpr>> &&
                                gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<XExpr>> &&
                                std::is_same_v<typename std::decay_t<YExpr>::result_type, mpf_class> &&
                                std::is_same_v<typename std::decay_t<XExpr>::result_type, mpf_class>>>
inline mpf_class atan2(const YExpr& y, const XExpr& x)
{
    return atan2(mpf_class(y), mpf_class(x));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class sinh(const Expr& expr)
{
    return sinh(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class cosh(const Expr& expr)
{
    return cosh(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class tanh(const Expr& expr)
{
    return tanh(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class asinh(const Expr& expr)
{
    return asinh(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class acosh(const Expr& expr)
{
    return acosh(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class atanh(const Expr& expr)
{
    return atanh(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class pow(const Expr& base, const mpf_class& exponent)
{
    return pow(mpf_class(base), exponent);
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class pow(const mpf_class& base, const Expr& exponent)
{
    return pow(base, mpf_class(exponent));
}

template <
    typename LhsExpr,
    typename RhsExpr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<LhsExpr>> &&
                                gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<RhsExpr>> &&
                                std::is_same_v<typename std::decay_t<LhsExpr>::result_type, mpf_class> &&
                                std::is_same_v<typename std::decay_t<RhsExpr>::result_type, mpf_class>>>
inline mpf_class pow(const LhsExpr& base, const RhsExpr& exponent)
{
    return pow(mpf_class(base), mpf_class(exponent));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class gamma(const Expr& expr)
{
    return gamma(mpf_class(expr));
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
inline mpf_class reciprocal_gamma(const Expr& expr)
{
    return reciprocal_gamma(mpf_class(expr));
}

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MATH_MPF_HPP
