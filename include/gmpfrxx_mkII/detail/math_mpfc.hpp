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

#ifndef GMPFRXX_MKII_DETAIL_MATH_MPFC_HPP
#define GMPFRXX_MKII_DETAIL_MATH_MPFC_HPP

#include <gmpfrxx_mkII/detail/math_mpf.hpp>
#include <gmpfrxx_mkII/detail/mpfc_impl.hpp>

#include <cstdint>
#include <type_traits>

namespace gmpxx {

namespace mpfc_math_detail {

inline mpf_class zero(mp_bitcnt_t precision)
{
    return mpf_class::with_precision(precision);
}

inline mpf_class one(mp_bitcnt_t precision)
{
    mpf_class result = mpf_class::with_precision(precision);
    mpf_set_ui(result.mpf_data(), 1);
    return result;
}

inline mpf_class half(mp_bitcnt_t precision)
{
    mpf_class result = one(precision);
    mpf_div_2exp(result.mpf_data(), result.mpf_data(), 1);
    return result;
}

inline bool is_negative(const mpf_class& value)
{
    return mpf_sgn(value.mpf_data()) < 0;
}

inline bool is_zero(const mpf_class& value)
{
    return mpf_sgn(value.mpf_data()) == 0;
}

template <typename Expr>
using enable_mpfc_expr_t = std::enable_if_t<
    gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfc_class>>;

} // namespace mpfc_math_detail

inline mpfc_class conj(const mpfc_class& value)
{
    return mpfc_class(value.real(), -value.imag());
}

inline mpf_class real(const mpfc_class& value)
{
    return value.real();
}

inline mpf_class imag(const mpfc_class& value)
{
    return value.imag();
}

inline mpf_class norm(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    mpf_class result = mpf_class::with_precision(precision);
    result = value.real() * value.real() + value.imag() * value.imag();
    return result;
}

inline mpf_class abs(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    mpf_class real_abs = mpf_class::with_precision(precision);
    mpf_class imag_abs = mpf_class::with_precision(precision);
    mpf_abs(real_abs.mpf_data(), value.real().mpf_data());
    mpf_abs(imag_abs.mpf_data(), value.imag().mpf_data());

    const mpf_class* high = &real_abs;
    const mpf_class* low = &imag_abs;
    if (mpf_cmp(real_abs.mpf_data(), imag_abs.mpf_data()) < 0) {
        high = &imag_abs;
        low = &real_abs;
    }

    if (mpf_sgn(high->mpf_data()) == 0) {
        return mpfc_math_detail::zero(precision);
    }

    mpf_class ratio = mpf_class::with_precision(precision);
    mpf_div(ratio.mpf_data(), low->mpf_data(), high->mpf_data());
    mpf_mul(ratio.mpf_data(), ratio.mpf_data(), ratio.mpf_data());
    mpf_add_ui(ratio.mpf_data(), ratio.mpf_data(), 1);
    mpf_sqrt(ratio.mpf_data(), ratio.mpf_data());

    mpf_class result = mpf_class::with_precision(precision);
    mpf_mul(result.mpf_data(), high->mpf_data(), ratio.mpf_data());
    return result;
}

inline mpf_class arg(const mpfc_class& value)
{
    return atan2(value.imag(), value.real());
}

inline mpfc_class polar(const mpf_class& radius, const mpf_class& angle)
{
    return mpfc_class(
        mpf_class(radius * cos(angle)),
        mpf_class(radius * sin(angle)));
}

inline mpfc_class sqrt(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    const mpf_class zero = mpfc_math_detail::zero(precision);

    if (mpfc_math_detail::is_zero(value.imag())) {
        if (mpfc_math_detail::is_negative(value.real())) {
            return mpfc_class(zero, sqrt(-value.real()));
        }
        return mpfc_class(sqrt(value.real()), zero);
    }

    const mpf_class half = mpfc_math_detail::half(precision);
    const mpf_class magnitude = abs(value);

    if (!mpfc_math_detail::is_negative(value.real())) {
        mpf_class real_part = sqrt((magnitude + value.real()) * half);
        mpf_class imag_part = value.imag() / (real_part + real_part);
        return mpfc_class(real_part, imag_part);
    }

    mpf_class imag_magnitude = sqrt((magnitude - value.real()) * half);
    mpf_class real_part = gmpxx::abs(value.imag()) / (imag_magnitude + imag_magnitude);
    if (mpfc_math_detail::is_negative(value.imag())) {
        imag_magnitude = -imag_magnitude;
    }
    return mpfc_class(real_part, imag_magnitude);
}

inline mpfc_class exp(const mpfc_class& value)
{
    const mpf_class real_exp = exp(value.real());
    return mpfc_class(real_exp * cos(value.imag()), real_exp * sin(value.imag()));
}

inline mpfc_class log(const mpfc_class& value)
{
    return mpfc_class(log(abs(value)), atan2(value.imag(), value.real()));
}

inline mpfc_class sin(const mpfc_class& value)
{
    return mpfc_class(
        sin(value.real()) * cosh(value.imag()),
        cos(value.real()) * sinh(value.imag()));
}

inline mpfc_class cos(const mpfc_class& value)
{
    return mpfc_class(
        cos(value.real()) * cosh(value.imag()),
        -sin(value.real()) * sinh(value.imag()));
}

inline mpfc_class tan(const mpfc_class& value)
{
    return sin(value) / cos(value);
}

inline mpfc_class sinh(const mpfc_class& value)
{
    return mpfc_class(
        sinh(value.real()) * cos(value.imag()),
        cosh(value.real()) * sin(value.imag()));
}

inline mpfc_class cosh(const mpfc_class& value)
{
    return mpfc_class(
        cosh(value.real()) * cos(value.imag()),
        sinh(value.real()) * sin(value.imag()));
}

inline mpfc_class tanh(const mpfc_class& value)
{
    return sinh(value) / cosh(value);
}

inline mpfc_class asin(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    const mpf_class zero = mpfc_math_detail::zero(precision);
    const mpf_class one_real = mpfc_math_detail::one(precision);
    const mpfc_class one(one_real, zero);
    const mpfc_class imaginary_unit(zero, one_real);
    const mpfc_class negative_imaginary_unit(zero, -one_real);
    return negative_imaginary_unit *
           log(imaginary_unit * value + sqrt(one - value * value));
}

inline mpfc_class acos(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    mpf_class half_pi = const_pi(precision);
    mpf_div_2exp(half_pi.mpf_data(), half_pi.mpf_data(), 1);
    return mpfc_class(half_pi, mpfc_math_detail::zero(precision)) - asin(value);
}

inline mpfc_class atan(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    const mpf_class zero = mpfc_math_detail::zero(precision);
    const mpf_class one_real = mpfc_math_detail::one(precision);
    const mpfc_class one(one_real, zero);
    const mpfc_class imaginary_unit(zero, one_real);
    const mpfc_class half_i(zero, mpfc_math_detail::half(precision));
    return half_i * (log(one - imaginary_unit * value) -
                     log(one + imaginary_unit * value));
}

inline mpfc_class asinh(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    const mpfc_class one(mpfc_math_detail::one(precision),
                         mpfc_math_detail::zero(precision));
    return log(value + sqrt(value * value + one));
}

inline mpfc_class acosh(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    const mpfc_class one(mpfc_math_detail::one(precision),
                         mpfc_math_detail::zero(precision));
    return log(value + sqrt(value + one) * sqrt(value - one));
}

inline mpfc_class atanh(const mpfc_class& value)
{
    const mp_bitcnt_t precision = value.precision();
    const mpfc_class one(mpfc_math_detail::one(precision),
                         mpfc_math_detail::zero(precision));
    const mpf_class half = mpfc_math_detail::half(precision);
    return mpfc_class(half, mpfc_math_detail::zero(precision)) *
           (log(one + value) - log(one - value));
}

inline mpfc_class pow(const mpfc_class& base, std::uint64_t exponent)
{
    const mp_bitcnt_t precision = base.precision();
    mpfc_class result(mpfc_math_detail::one(precision), mpfc_math_detail::zero(precision));
    mpfc_class factor = base;

    while (exponent != 0) {
        if ((exponent & 1U) != 0) {
            result = result * factor;
        }
        exponent >>= 1U;
        if (exponent != 0) {
            factor = factor * factor;
        }
    }

    return result;
}

inline mpfc_class pow(const mpfc_class& base, std::int64_t exponent)
{
    if (exponent >= 0) {
        return pow(base, static_cast<std::uint64_t>(exponent));
    }
    const std::uint64_t magnitude = static_cast<std::uint64_t>(-(exponent + 1)) + 1U;
    const mp_bitcnt_t precision = base.precision();
    const mpfc_class one(mpfc_math_detail::one(precision), mpfc_math_detail::zero(precision));
    return one / pow(base, magnitude);
}

template <
    typename I,
    typename = std::enable_if_t<std::is_integral_v<std::remove_cv_t<I>> &&
                                std::is_unsigned_v<std::remove_cv_t<I>> &&
                                !std::is_same_v<std::remove_cv_t<I>, bool> &&
                                !std::is_same_v<std::remove_cv_t<I>, std::uint64_t>>>
inline mpfc_class pow(const mpfc_class& base, I exponent)
{
    return pow(base, static_cast<std::uint64_t>(exponent));
}

template <
    typename I,
    typename = std::enable_if_t<std::is_integral_v<std::remove_cv_t<I>> &&
                                std::is_signed_v<std::remove_cv_t<I>> &&
                                !std::is_same_v<std::remove_cv_t<I>, bool> &&
                                !std::is_same_v<std::remove_cv_t<I>, std::int64_t>>,
    typename = void>
inline mpfc_class pow(const mpfc_class& base, I exponent)
{
    return pow(base, static_cast<std::int64_t>(exponent));
}

inline mpfc_class pow(const mpfc_class& base, const mpf_class& exponent)
{
    const mp_bitcnt_t precision = std::max(base.precision(), exponent.precision());
    return exp(mpfc_class(mpf_class(exponent, precision), mpfc_math_detail::zero(precision)) * log(base));
}

inline mpfc_class pow(const mpfc_class& base, const mpfc_class& exponent)
{
    return exp(exponent * log(base));
}

inline mpfc_class pow(const mpf_class& base, const mpfc_class& exponent)
{
    const mp_bitcnt_t precision = std::max(base.precision(), exponent.precision());
    return pow(mpfc_class(mpf_class(base, precision), mpfc_math_detail::zero(precision)), exponent);
}

template <typename Scalar,
          typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>>>
inline mpfc_class pow(Scalar base, const mpfc_class& exponent)
{
    return pow(mpf_class(base, exponent.precision()), exponent);
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class sqrt(const Expr& expr)
{
    return sqrt(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class exp(const Expr& expr)
{
    return exp(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class log(const Expr& expr)
{
    return log(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class sin(const Expr& expr)
{
    return sin(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class cos(const Expr& expr)
{
    return cos(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class tan(const Expr& expr)
{
    return tan(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class sinh(const Expr& expr)
{
    return sinh(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class cosh(const Expr& expr)
{
    return cosh(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class tanh(const Expr& expr)
{
    return tanh(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class asin(const Expr& expr)
{
    return asin(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class acos(const Expr& expr)
{
    return acos(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class atan(const Expr& expr)
{
    return atan(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class asinh(const Expr& expr)
{
    return asinh(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class acosh(const Expr& expr)
{
    return acosh(mpfc_class(expr));
}

template <typename Expr, typename = mpfc_math_detail::enable_mpfc_expr_t<Expr>>
inline mpfc_class atanh(const Expr& expr)
{
    return atanh(mpfc_class(expr));
}

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MATH_MPFC_HPP
