#ifndef GMPFRXX_MKII_DETAIL_MATH_MPF_HPP
#define GMPFRXX_MKII_DETAIL_MATH_MPF_HPP

#include <gmpfrxx_mkII/detail/mpf_impl.hpp>

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace gmpxx {

namespace mpf_math_detail {

inline mpf_class make_from_double(double value, mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_set_d(result.mpf_data(), value);
    return result;
}

template <typename Function>
inline mpf_class apply_unary(const mpf_class& value, Function function)
{
    return make_from_double(function(value.to_double()), value.precision());
}

} // namespace mpf_math_detail

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
