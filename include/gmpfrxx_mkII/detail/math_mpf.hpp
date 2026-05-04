#ifndef GMPFRXX_MKII_DETAIL_MATH_MPF_HPP
#define GMPFRXX_MKII_DETAIL_MATH_MPF_HPP

#include <gmpfrxx_mkII/detail/mpf_impl.hpp>

#include <algorithm>
#include <cmath>
#include <mutex>
#include <type_traits>

namespace gmpxx {

namespace mpf_math_detail {

inline mp_bitcnt_t normalize_target_precision(mp_bitcnt_t precision)
{
    return std::max<mp_bitcnt_t>(precision, 32);
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
