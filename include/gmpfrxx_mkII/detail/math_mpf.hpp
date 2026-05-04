#ifndef GMPFRXX_MKII_DETAIL_MATH_MPF_HPP
#define GMPFRXX_MKII_DETAIL_MATH_MPF_HPP

#include <gmpfrxx_mkII/detail/mpf_impl.hpp>

#include <algorithm>
#include <cmath>
#include <mutex>
#include <stdexcept>
#include <type_traits>

namespace gmpxx {

namespace mpf_math_detail {

inline constexpr mp_bitcnt_t minimum_target_precision = 32;
inline constexpr mp_bitcnt_t log_cancellation_probe_guard_bits = 32;

inline mp_bitcnt_t normalize_target_precision(mp_bitcnt_t precision)
{
    return std::max(precision, minimum_target_precision);
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

inline mpf_class make_from_double(double value, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_set_d(result.mpf_data(), value);
    return result;
}

inline mpf_class make_ui(unsigned long value, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_set_ui(result.mpf_data(), value);
    return result;
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
    mpf_class result = mpf_class::with_precision(precision);
    mpf_div(result.mpf_data(), lhs.mpf_data(), rhs.mpf_data());
    return result;
}

inline mpf_class sqrt_prec(const mpf_class& value, mp_bitcnt_t precision)
{
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

inline mpf_class average(const mpf_class& lhs, const mpf_class& rhs, mp_bitcnt_t precision)
{
    mpf_class result = add(lhs, rhs, precision);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return result;
}

inline mpf_class inv_sqrt_ui(unsigned long value, mp_bitcnt_t precision)
{
    mpf_class denominator = make_ui(value, precision);
    mpf_sqrt(denominator.mpf_data(), denominator.mpf_data());

    mpf_class result = mpf_class::with_precision(precision);
    mpf_ui_div(result.mpf_data(), 1ul, denominator.mpf_data());
    return result;
}

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
    switch (target_precision) {
    case 512:
    case 1024:
    case 2048:
        return true;
    default:
        return false;
    }
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

    while (true) {
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
            break;
        }
    }

    return set_prec_copy(pi_current, target);
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

    while (true) {
        mpf_class a_next = average(a, b, precision);
        mpf_class b_next = sqrt_prec(mul(a, b, precision), precision);
        if (mpf_cmp(abs_prec(sub(a_next, b_next, precision), precision).mpf_data(), epsilon.mpf_data()) < 0) {
            return average(a_next, b_next, precision);
        }
        a = a_next;
        b = b_next;
    }
}

inline mpf_class theta3_from_power_of_two_q(mp_bitcnt_t q_exponent, mp_bitcnt_t precision)
{
    const mpf_class threshold = theta_series_threshold(precision);
    mpf_class sum = make_ui(1, precision);
    mpf_class term = make_ui(1, precision);
    mpf_div_2exp(term.mpf_data(), term.mpf_data(), q_exponent);

    for (unsigned long k = 1;; ++k) {
        mpf_class contribution = mul_ui(term, 2ul, precision);
        if (mpf_cmp(contribution.mpf_data(), threshold.mpf_data()) < 0) {
            break;
        }
        sum = add(sum, contribution, precision);
        mpf_div_2exp(term.mpf_data(), term.mpf_data(), q_exponent * (2ul * k + 1ul));
    }
    return sum;
}

inline mpf_class theta2_from_power_of_two_q(mp_bitcnt_t q_exponent, mp_bitcnt_t precision)
{
    const mpf_class threshold = theta_series_threshold(precision);
    mpf_class q = make_ui(1, precision);
    mpf_div_2exp(q.mpf_data(), q.mpf_data(), q_exponent);
    mpf_class term = sqrt_prec(sqrt_prec(q, precision), precision);
    mpf_class sum = make_ui(0, precision);

    for (unsigned long k = 0;; ++k) {
        mpf_class contribution = mul_ui(term, 2ul, precision);
        if (mpf_cmp(contribution.mpf_data(), threshold.mpf_data()) < 0) {
            break;
        }
        sum = add(sum, contribution, precision);
        mpf_div_2exp(term.mpf_data(), term.mpf_data(), q_exponent * (2ul * k + 2ul));
    }
    return sum;
}

inline mpf_class compute_log_two_theta_agm(mp_bitcnt_t target_precision)
{
    const mp_bitcnt_t target = normalize_target_precision(target_precision);
    const mp_bitcnt_t work = working_precision_for_log_two(target);
    const mp_bitcnt_t q_exponent = (work / 2) - 2;

    mpf_class theta2 = theta2_from_power_of_two_q(q_exponent, work);
    mpf_class theta3 = theta3_from_power_of_two_q(q_exponent, work);
    mpf_class agm_value = agm_converged(sqr(theta2, work), sqr(theta3, work), work);
    mpf_class q_scale = make_ui(static_cast<unsigned long>(q_exponent), work);
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

    for (unsigned long k = 2;; ++k) {
        power = mul(power, x, precision);
        mpf_class term = div(power, make_ui(k, precision), precision);
        if ((k & 1ul) == 0ul) {
            term = sub(make_ui(0, precision), term, precision);
        }
        sum = add(sum, term, precision);
        if (mpf_cmp(abs_prec(term, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            break;
        }
    }
    return sum;
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

    for (unsigned long k = 1;; ++k) {
        term = mul(term, y2, precision);
        const unsigned long denominator = 2ul * k + 1ul;
        mpf_class contribution = div(term, make_ui(denominator, precision), precision);
        sum = add(sum, contribution, precision);
        if (mpf_cmp(abs_prec(contribution, precision).mpf_data(), epsilon.mpf_data()) < 0) {
            break;
        }
    }
    return mul_ui(sum, 2ul, precision);
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

inline mpf_class mul_signed_exp(const mpf_class& value, mp_exp_t multiplier, mp_bitcnt_t precision)
{
    if (multiplier == 0) {
        return make_ui(0, precision);
    }
    const unsigned long magnitude_ui =
        static_cast<unsigned long>(multiplier > 0 ? multiplier : -multiplier);
    mpf_class magnitude = mul_ui(value, magnitude_ui, precision);
    if (multiplier < 0) {
        return sub(make_ui(0, precision), magnitude, precision);
    }
    return magnitude;
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
    const mp_exp_t desired_exponent = static_cast<mp_exp_t>(work / 2 + 16);
    const mp_exp_t exponent = desired_exponent - x_exponent + 1;

    mpf_class s = set_prec_copy(x, work);
    if (exponent >= 0) {
        mpf_mul_2exp(s.mpf_data(), s.mpf_data(), static_cast<mp_bitcnt_t>(exponent));
    } else {
        mpf_div_2exp(s.mpf_data(), s.mpf_data(), static_cast<mp_bitcnt_t>(-exponent));
    }

    mpf_class b = div(make_ui(4, work), s, work);
    mpf_class agm_value = agm_converged(one, b, work);
    mpf_class leading = div(pi(work), mul(two, agm_value, work), work);
    mpf_class correction = mul_signed_exp(log_two(work), exponent, work);
    return set_prec_copy(sub(leading, correction, work), target);
}

template <typename Function>
inline mpf_class apply_unary(const mpf_class& value, Function function)
{
    return make_from_double(function(value.to_double()), value.precision());
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

inline mpf_class sqrt(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_sqrt(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class abs(const mpf_class& value)
{
    mpf_class result = mpf_class::with_precision(value.precision());
    mpf_abs(result.mpf_data(), value.mpf_data());
    return result;
}

inline mpf_class exp(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::exp));
}

inline mpf_class log(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::log));
}

inline mpf_class sin(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::sin));
}

inline mpf_class cos(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::cos));
}

inline mpf_class tan(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::tan));
}

inline mpf_class sinh(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::sinh));
}

inline mpf_class cosh(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::cosh));
}

inline mpf_class tanh(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::tanh));
}

inline mpf_class atan2(const mpf_class& y, const mpf_class& x)
{
    return mpf_math_detail::make_from_double(
        std::atan2(y.to_double(), x.to_double()), std::max(y.precision(), x.precision()));
}

inline mpf_class pow(const mpf_class& base, const mpf_class& exponent)
{
    return mpf_math_detail::make_from_double(
        std::pow(base.to_double(), exponent.to_double()), std::max(base.precision(), exponent.precision()));
}

inline mpf_class gamma(const mpf_class& value)
{
    return mpf_math_detail::apply_unary(value, static_cast<double (*)(double)>(std::tgamma));
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
inline mpf_class gamma(const Expr& expr)
{
    return gamma(mpf_class(expr));
}

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MATH_MPF_HPP
