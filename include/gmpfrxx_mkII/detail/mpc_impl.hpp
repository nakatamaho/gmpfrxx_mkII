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

#ifndef GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/mpc_environment.hpp>
#include <gmpfrxx_mkII/detail/mpfr_impl.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cstdint>
#include <locale>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include <mpc.h>

namespace mpfrxx {

class mpc_class {
public:
    mpc_class()
        : mpc_class(precision_tag{},
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits())
    {
    }

    mpc_class(const mpc_class& other)
    {
        mpc_init3(value_, other.real_precision(), other.imag_precision());
        mpc_set(value_, other.value_, default_rounding());
    }

    mpc_class(mpc_class&& other) noexcept
    {
        mpc_init3(value_, other.real_precision(), other.imag_precision());
        mpc_swap(value_, other.value_);
    }

    mpc_class(const mpfr_class& real, const mpfr_class& imag)
    {
        mpc_init3(value_, real.precision(), imag.precision());
        mpc_set_fr_fr(value_, real.mpfr_data(), imag.mpfr_data(), default_rounding());
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpc_class>>>
    mpc_class(const Expr& expr);

    ~mpc_class()
    {
        mpc_clear(value_);
    }

    mpc_class& operator=(const mpc_class& other)
    {
        if (this != &other) {
            mpc_set(value_, other.value_, default_rounding());
        }
        return *this;
    }

    mpc_class& operator=(mpc_class&& other) noexcept
    {
        if (this != &other) {
            mpc_swap(value_, other.value_);
        }
        return *this;
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpc_class>>>
    mpc_class& operator=(const Expr& expr);

    static mpc_class with_precision(mpfr_prec_t precision)
    {
        return mpc_class(precision_tag{}, precision, precision);
    }

    static mpc_class with_precision(mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
    {
        return mpc_class(precision_tag{}, real_precision, imag_precision);
    }

    static mpc_class with_precision(mpfr_prec_t precision, double real, double imag)
    {
        mpc_class result = with_precision(precision);
        mpc_set_d_d(result.value_, real, imag, default_rounding());
        return result;
    }

    static mpc_class with_precision(mpfr_prec_t real_precision, mpfr_prec_t imag_precision, double real, double imag)
    {
        mpc_class result = with_precision(real_precision, imag_precision);
        mpc_set_d_d(result.value_, real, imag, default_rounding());
        return result;
    }

    mpfr_prec_t precision() const noexcept
    {
        const mpfr_prec_t real = real_precision();
        const mpfr_prec_t imag = imag_precision();
        return real == imag ? real : std::min(real, imag);
    }

    mpfr_prec_t real_precision() const noexcept
    {
        mpfr_prec_t real = 0;
        mpfr_prec_t imag = 0;
        mpc_get_prec2(&real, &imag, value_);
        return real;
    }

    mpfr_prec_t imag_precision() const noexcept
    {
        mpfr_prec_t real = 0;
        mpfr_prec_t imag = 0;
        mpc_get_prec2(&real, &imag, value_);
        return imag;
    }

    double real_to_double() const
    {
        return mpfr_get_d(mpc_realref(value_), mpfrxx::default_rounding_mode());
    }

    double imag_to_double() const
    {
        return mpfr_get_d(mpc_imagref(value_), mpfrxx::default_rounding_mode());
    }

    const mpc_t& mpc_data() const noexcept
    {
        return value_;
    }

    mpc_t& mpc_data() noexcept
    {
        return value_;
    }

    static mpc_rnd_t default_rounding() noexcept
    {
        return mpfrxx::default_mpc_rounding_mode();
    }

private:
    struct precision_tag {};

    mpc_class(precision_tag, mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
    {
        mpc_init3(value_, real_precision, imag_precision);
        mpc_set_ui(value_, 0, default_rounding());
    }

    mpc_t value_;
};

} // namespace mpfrxx

namespace std {

template <>
struct common_type<gmpxx::mpz_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, gmpxx::mpz_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<gmpxx::mpq_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, gmpxx::mpq_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpfr_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, mpfrxx::mpfr_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

} // namespace std

namespace gmpfrxx_mkII {
namespace detail {

struct mpc_expression_precision_bits {
    mpfr_prec_t real;
    mpfr_prec_t imag;
};

inline mpc_expression_precision_bits max_mpc_precision(
    mpc_expression_precision_bits lhs,
    mpc_expression_precision_bits rhs)
{
    return mpc_expression_precision_bits{
        std::max(lhs.real, rhs.real),
        std::max(lhs.imag, rhs.imag),
    };
}

template <typename T>
struct is_supported_mpc_scalar
    : std::bool_constant<is_supported_expression_integral_v<T> ||
                         std::is_same_v<std::remove_cv_t<T>, float> ||
                         std::is_same_v<std::remove_cv_t<T>, double>> {};

template <typename T>
inline constexpr bool is_supported_mpc_scalar_v =
    is_supported_mpc_scalar<std::remove_cv_t<T>>::value;

template <typename T, typename = void>
struct normalized_mpc_scalar;

template <typename T>
struct normalized_mpc_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                 std::is_signed_v<std::remove_cv_t<T>>>> {
    using type = std::int64_t;
};

template <typename T>
struct normalized_mpc_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                 std::is_unsigned_v<std::remove_cv_t<T>>>> {
    using type = std::uint64_t;
};

template <>
struct normalized_mpc_scalar<float> {
    using type = double;
};

template <>
struct normalized_mpc_scalar<double> {
    using type = double;
};

template <typename T>
using normalized_mpc_scalar_t = typename normalized_mpc_scalar<std::remove_cv_t<T>>::type;

template <typename T, typename = void>
struct is_mpc_expression_operand : std::false_type {};

template <>
struct is_mpc_expression_operand<mpfrxx::mpc_class> : std::true_type {};

template <>
struct is_mpc_expression_operand<mpfrxx::mpfr_class> : std::true_type {};

template <>
struct is_mpc_expression_operand<gmpxx::mpz_class> : std::true_type {};

template <>
struct is_mpc_expression_operand<gmpxx::mpq_class> : std::true_type {};

template <typename T>
struct is_mpc_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> &&
                     (std::is_same_v<typename T::result_type, mpfrxx::mpc_class> ||
                      std::is_same_v<typename T::result_type, mpfrxx::mpfr_class> ||
                      std::is_same_v<typename T::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename T::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
struct is_mpc_expression_operand<T, std::enable_if_t<is_supported_mpc_scalar_v<T>>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpc_expression_operand_v =
    is_mpc_expression_operand<std::decay_t<T>>::value;

template <typename T, typename = void>
struct is_mpc_object_or_node : std::false_type {};

template <typename T>
struct is_mpc_object_or_node<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, mpfrxx::mpc_class>>> : std::true_type {};

template <typename T>
struct is_mpc_object_or_node<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     std::is_same_v<typename std::decay_t<T>::result_type, mpfrxx::mpc_class>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpc_object_or_node_v = is_mpc_object_or_node<T>::value;

inline object_leaf<mpfrxx::mpc_class> make_mpc_operand(const mpfrxx::mpc_class& value)
{
    return object_leaf<mpfrxx::mpc_class>(value);
}

inline object_leaf<mpfrxx::mpfr_class> make_mpc_operand(const mpfrxx::mpfr_class& value)
{
    return object_leaf<mpfrxx::mpfr_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpc_operand(const gmpxx::mpz_class& value)
{
    return object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpc_operand(const gmpxx::mpq_class& value)
{
    return object_leaf<gmpxx::mpq_class>(value);
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_mpc_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

template <typename Scalar, typename = std::enable_if_t<is_supported_mpc_scalar_v<Scalar>>>
auto make_mpc_operand(Scalar value)
{
    using storage_type = normalized_mpc_scalar_t<Scalar>;
    return scalar_leaf<storage_type, mpfrxx::mpc_class>(static_cast<storage_type>(value));
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<mpfrxx::mpc_class>& expr)
{
    return mpc_expression_precision_bits{expr.get().real_precision(), expr.get().imag_precision()};
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return mpc_expression_precision_bits{expr.get().precision(), 0};
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return mpc_expression_precision_bits{0, 0};
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return mpc_expression_precision_bits{0, 0};
}

template <typename T, typename Result>
mpc_expression_precision_bits mpc_expression_precision(const scalar_leaf<T, Result>&)
{
    return mpc_expression_precision_bits{mpfrxx::default_mpc_real_precision_bits(), 0};
}

template <typename Op, typename Expr, typename Result>
mpc_expression_precision_bits mpc_expression_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpc_expression_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mpc_expression_precision_bits mpc_expression_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return max_mpc_precision(mpc_expression_precision(expr.lhs()), mpc_expression_precision(expr.rhs()));
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<mpfrxx::mpc_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    mpc_set(dest, expr.get().mpc_data(), rnd);
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<mpfrxx::mpfr_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    mpc_set_fr(dest, expr.get().mpfr_data(), rnd);
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<gmpxx::mpz_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    mpc_set_z(dest, expr.get().mpz_data(), rnd);
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<gmpxx::mpq_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    mpc_set_q(dest, expr.get().mpq_data(), rnd);
}

template <typename T, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const scalar_leaf<T, Result>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    if constexpr (std::is_same_v<T, double>) {
        mpc_set_d(dest, expr.value(), rnd);
    } else if constexpr (std::is_same_v<T, std::int64_t> ||
                         std::is_same_v<T, std::uint64_t>) {
        const gmpxx::mpz_class integer(expr.value());
        mpc_set_z(dest, integer.mpz_data(), rnd);
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPC scalar leaf");
    }
}

inline bool mpc_expression_references(const mpc_t target, const object_leaf<mpfrxx::mpc_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpc_data()[0]);
}

inline bool mpc_expression_references(const mpc_t, const object_leaf<mpfrxx::mpfr_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

template <typename T, typename Result>
bool mpc_expression_references(const mpc_t, const scalar_leaf<T, Result>&)
{
    return false;
}

template <typename Op, typename Expr, typename Result>
bool mpc_expression_references(const mpc_t target, const unary_expr<Op, Expr, Result>& expr)
{
    return mpc_expression_references(target, expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
bool mpc_expression_references(const mpc_t target, const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return mpc_expression_references(target, expr.lhs()) ||
           mpc_expression_references(target, expr.rhs());
}

template <typename Op>
void mpc_apply_binary(mpc_t dest, const mpc_t lhs, const mpc_t rhs, mpc_rnd_t rnd)
{
    if constexpr (std::is_same_v<Op, add_op>) {
        mpc_add(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpc_sub(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpc_mul(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpc_div(dest, lhs, rhs, rnd);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPC expression operation");
    }
}

template <typename Expr, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const unary_expr<pos_op, Expr, Result>& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    mpc_evaluate(dest, expr.expr(), eval_precision, rnd);
}

template <typename Expr, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const unary_expr<neg_op, Expr, Result>& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    mpc_evaluate(dest, expr.expr(), eval_precision, rnd);
    mpc_neg(dest, dest, rnd);
}

template <typename Expr>
void mpc_evaluate_to_temporary(
    mpc_t temp,
    const Expr& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    mpc_init3(temp, eval_precision.real, eval_precision.imag);
    mpc_evaluate(temp, expr, eval_precision, rnd);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    if constexpr (std::is_same_v<Result, mpfrxx::mpfr_class>) {
        mpfr_t real;
        mpfr_init2(real, eval_precision.real);
        mpfr_evaluate(real, expr, eval_precision.real, MPC_RND_RE(rnd));
        mpc_set_fr(dest, real, rnd);
        mpfr_clear(real);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        mpz_t exact;
        mpz_init(exact);
        mpz_evaluate(exact, expr);
        mpc_set_z(dest, exact, rnd);
        mpz_clear(exact);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        mpq_t exact;
        mpq_init(exact);
        mpq_evaluate(exact, expr);
        mpc_set_q(dest, exact, rnd);
        mpq_clear(exact);
        return;
    }

    if (mpc_expression_references(dest, expr)) {
        mpc_t lhs;
        mpc_t rhs;
        mpc_evaluate_to_temporary(lhs, expr.lhs(), eval_precision, rnd);
        mpc_evaluate_to_temporary(rhs, expr.rhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, lhs, rhs, rnd);
        mpc_clear(rhs);
        mpc_clear(lhs);
        return;
    }

    if constexpr (std::is_same_v<Lhs, object_leaf<mpfrxx::mpc_class>> &&
                  std::is_same_v<Rhs, object_leaf<mpfrxx::mpc_class>>) {
        mpc_apply_binary<Op>(dest, expr.lhs().get().mpc_data(), expr.rhs().get().mpc_data(), rnd);
    } else if constexpr (std::is_same_v<Rhs, object_leaf<mpfrxx::mpc_class>>) {
        mpc_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, dest, expr.rhs().get().mpc_data(), rnd);
    } else if constexpr (std::is_same_v<Lhs, object_leaf<mpfrxx::mpc_class>> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpc_evaluate(dest, expr.rhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, expr.lhs().get().mpc_data(), dest, rnd);
    } else {
        mpc_t rhs;
        mpc_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpc_evaluate_to_temporary(rhs, expr.rhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, dest, rhs, rnd);
        mpc_clear(rhs);
    }
}

template <typename Op, typename Rhs>
void mpc_compound_assign(mpfrxx::mpc_class& lhs, Rhs&& rhs)
{
    auto operand = make_mpc_operand(std::forward<Rhs>(rhs));
    using operand_type = std::decay_t<decltype(operand)>;
    const mpc_expression_precision_bits precision{lhs.real_precision(), lhs.imag_precision()};
    const mpc_rnd_t rnd = mpfrxx::mpc_class::default_rounding();
    if constexpr (std::is_same_v<operand_type, object_leaf<mpfrxx::mpc_class>>) {
        mpc_apply_binary<Op>(lhs.mpc_data(), lhs.mpc_data(), operand.get().mpc_data(), rnd);
    } else {
        mpc_t value;
        mpc_evaluate_to_temporary(value, operand, precision, rnd);
        mpc_apply_binary<Op>(lhs.mpc_data(), lhs.mpc_data(), value, rnd);
        mpc_clear(value);
    }
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_expression_operand_v<Lhs> &&
                                        is_mpc_expression_operand_v<Rhs> &&
                                        (is_mpc_object_or_node_v<Lhs> ||
                                         is_mpc_object_or_node_v<Rhs>),
                                    short> = 0>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<add_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_expression_operand_v<Lhs> &&
                                        is_mpc_expression_operand_v<Rhs> &&
                                        (is_mpc_object_or_node_v<Lhs> ||
                                         is_mpc_object_or_node_v<Rhs>),
                                    short> = 0>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<sub_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_expression_operand_v<Lhs> &&
                                        is_mpc_expression_operand_v<Rhs> &&
                                        (is_mpc_object_or_node_v<Lhs> ||
                                         is_mpc_object_or_node_v<Rhs>),
                                    short> = 0>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<mul_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_expression_operand_v<Lhs> &&
                                        is_mpc_expression_operand_v<Rhs> &&
                                        (is_mpc_object_or_node_v<Lhs> ||
                                         is_mpc_object_or_node_v<Rhs>),
                                    short> = 0>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<div_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Expr, std::enable_if_t<is_mpc_expression_operand_v<Expr> &&
                                              is_mpc_object_or_node_v<Expr>,
                                          short> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpc_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), mpfrxx::mpc_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpc_expression_operand_v<Expr> &&
                                              is_mpc_object_or_node_v<Expr>,
                                          short> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpc_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), mpfrxx::mpc_class>(std::move(operand));
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

template <typename Expr, typename>
mpc_class::mpc_class(const Expr& expr)
{
    const auto precision = gmpfrxx_mkII::detail::mpc_expression_precision(expr);
    mpc_init3(value_, precision.real, precision.imag);
    gmpfrxx_mkII::detail::mpc_evaluate(value_, expr, precision, default_rounding());
}

template <typename Expr, typename>
mpc_class& mpc_class::operator=(const Expr& expr)
{
    const gmpfrxx_mkII::detail::mpc_expression_precision_bits precision{
        this->real_precision(),
        this->imag_precision(),
    };
    gmpfrxx_mkII::detail::mpc_evaluate(value_, expr, precision, default_rounding());
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

inline bool operator==(const mpc_class& lhs, const mpc_class& rhs)
{
    return mpc_cmp(lhs.mpc_data(), rhs.mpc_data()) == 0;
}

inline bool operator!=(const mpc_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpc_class& lhs, const mpfr_class& rhs)
{
    return mpfr_cmp(mpc_realref(lhs.mpc_data()), rhs.mpfr_data()) == 0 &&
           mpfr_zero_p(mpc_imagref(lhs.mpc_data())) != 0;
}

inline bool operator==(const mpfr_class& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpc_class& lhs, const mpfr_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const mpfr_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpc_class& lhs, const gmpxx::mpz_class& rhs)
{
    return lhs == mpfr_class(rhs, lhs.real_precision());
}

inline bool operator==(const gmpxx::mpz_class& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpc_class& lhs, const gmpxx::mpz_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const gmpxx::mpz_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpc_class& lhs, const gmpxx::mpq_class& rhs)
{
    return lhs == mpfr_class(rhs, lhs.real_precision());
}

inline bool operator==(const gmpxx::mpq_class& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpc_class& lhs, const gmpxx::mpq_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const gmpxx::mpq_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_scalar_v<Scalar>, int> = 0>
inline bool operator==(const mpc_class& lhs, Scalar rhs)
{
    return lhs == mpfr_class(rhs, lhs.real_precision());
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_scalar_v<Scalar>, int> = 0>
inline bool operator==(Scalar lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_scalar_v<Scalar>, int> = 0>
inline bool operator!=(const mpc_class& lhs, Scalar rhs)
{
    return !(lhs == rhs);
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_scalar_v<Scalar>, int> = 0>
inline bool operator!=(Scalar lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator+=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::add_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator-=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::sub_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator*=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::mul_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator/=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::div_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

namespace detail {

template <typename Expr>
inline mpc_class materialize_mpc_math_operand(const Expr& expr)
{
    if constexpr (std::is_same_v<std::decay_t<Expr>, mpc_class>) {
        return expr;
    } else {
        return mpc_class(expr);
    }
}

template <typename Expr, typename Function>
inline mpc_class unary_mpc_math(const Expr& expr, Function function)
{
    const mpc_class operand = materialize_mpc_math_operand(expr);
    mpc_class result = mpc_class::with_precision(operand.real_precision(), operand.imag_precision());
    function(result.mpc_data(), operand.mpc_data(), mpc_class::default_rounding());
    return result;
}

template <typename Expr, typename Function>
inline mpfr_class unary_mpc_real_math(const Expr& expr, mpfr_prec_t precision, Function function)
{
    const mpc_class operand = materialize_mpc_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(precision);
    function(result.mpfr_data(), operand.mpc_data(), mpfrxx::default_rounding_mode());
    return result;
}

template <typename Lhs, typename Rhs, typename Function>
inline mpc_class binary_mpc_math(const Lhs& lhs, const Rhs& rhs, Function function)
{
    const mpc_class left = materialize_mpc_math_operand(lhs);
    const mpc_class right = materialize_mpc_math_operand(rhs);
    const mpfr_prec_t real_precision = std::max(left.real_precision(), right.real_precision());
    const mpfr_prec_t imag_precision = std::max(left.imag_precision(), right.imag_precision());
    mpc_class result = mpc_class::with_precision(real_precision, imag_precision);
    function(result.mpc_data(), left.mpc_data(), right.mpc_data(), mpc_class::default_rounding());
    return result;
}

template <typename A, typename B, typename C, typename Function>
inline mpc_class ternary_mpc_math(const A& a, const B& b, const C& c, Function function)
{
    const mpc_class first = materialize_mpc_math_operand(a);
    const mpc_class second = materialize_mpc_math_operand(b);
    const mpc_class third = materialize_mpc_math_operand(c);
    const mpfr_prec_t real_precision = std::max({first.real_precision(),
                                                 second.real_precision(),
                                                 third.real_precision()});
    const mpfr_prec_t imag_precision = std::max({first.imag_precision(),
                                                 second.imag_precision(),
                                                 third.imag_precision()});
    mpc_class result = mpc_class::with_precision(real_precision, imag_precision);
    function(result.mpc_data(), first.mpc_data(), second.mpc_data(), third.mpc_data(), mpc_class::default_rounding());
    return result;
}

} // namespace detail

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class sqrt(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_sqrt(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class exp(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_exp(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class log(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_log(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class log10(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_log10(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class sqr(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_sqr(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class conj(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_conj(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class proj(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        mpc_proj(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class real(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.real_precision());
    mpc_real(result.mpfr_data(), operand.mpc_data(), mpfrxx::default_rounding_mode());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class imag(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.imag_precision());
    mpc_imag(result.mpfr_data(), operand.mpc_data(), mpfrxx::default_rounding_mode());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class abs(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    return detail::unary_mpc_real_math(operand,
                                       std::max(operand.real_precision(), operand.imag_precision()),
                                       [](mpfr_t rop, const mpc_t op, mpfr_rnd_t rnd) {
                                           mpc_abs(rop, op, rnd);
                                       });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class norm(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    return detail::unary_mpc_real_math(operand,
                                       std::max(operand.real_precision(), operand.imag_precision()),
                                       [](mpfr_t rop, const mpc_t op, mpfr_rnd_t rnd) {
                                           mpc_norm(rop, op, rnd);
                                       });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class arg(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    return detail::unary_mpc_real_math(operand,
                                       std::max(operand.real_precision(), operand.imag_precision()),
                                       [](mpfr_t rop, const mpc_t op, mpfr_rnd_t rnd) {
                                           mpc_arg(rop, op, rnd);
                                       });
}

#define GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>, \
                         int> = 0> \
    inline mpc_class name(const Expr& expr) \
    { \
        return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) { \
            mpc_##name(rop, op, rnd); \
        }); \
    }

GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(sin)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(cos)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(tan)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(sinh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(cosh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(tanh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(asin)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(acos)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(atan)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(asinh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(acosh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(atanh)

#undef GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Rhs>),
                     int> = 0>
inline mpc_class pow(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpc_math(lhs, rhs, [](mpc_t rop, const mpc_t op1, const mpc_t op2, mpc_rnd_t rnd) {
        mpc_pow(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Rhs>),
                     int> = 0>
inline mpc_class agm(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpc_math(lhs, rhs, [](mpc_t rop, const mpc_t op1, const mpc_t op2, mpc_rnd_t rnd) {
        mpc_agm(rop, op1, op2, rnd);
    });
}

template <
    typename A,
    typename B,
    typename C,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<A> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<B> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<C> &&
                         (gmpfrxx_mkII::detail::is_mpc_object_or_node_v<A> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<B> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<C>),
                     int> = 0>
inline mpc_class fma(const A& a, const B& b, const C& c)
{
    return detail::ternary_mpc_math(a, b, c, [](mpc_t rop,
                                                const mpc_t op1,
                                                const mpc_t op2,
                                                const mpc_t op3,
                                                mpc_rnd_t rnd) {
        mpc_fma(rop, op1, op2, op3, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline std::pair<mpc_class, mpc_class> sin_cos(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    mpc_class sine = mpc_class::with_precision(operand.real_precision(), operand.imag_precision());
    mpc_class cosine = mpc_class::with_precision(operand.real_precision(), operand.imag_precision());
    mpc_sin_cos(sine.mpc_data(),
                cosine.mpc_data(),
                operand.mpc_data(),
                mpc_class::default_rounding(),
                mpc_class::default_rounding());
    return {std::move(sine), std::move(cosine)};
}

inline mpc_class rootofunity(unsigned long order,
                             unsigned long index,
                             mpfr_prec_t real_precision,
                             mpfr_prec_t imag_precision)
{
    mpc_class result = mpc_class::with_precision(real_precision, imag_precision);
    mpc_rootofunity(result.mpc_data(), order, index, mpc_class::default_rounding());
    return result;
}

inline mpc_class rootofunity(unsigned long order, unsigned long index, mpfr_prec_t precision)
{
    return rootofunity(order, index, precision, precision);
}

inline mpc_class rootofunity(unsigned long order, unsigned long index)
{
    return rootofunity(order,
                       index,
                       mpfrxx::default_mpc_real_precision_bits(),
                       mpfrxx::default_mpc_imag_precision_bits());
}

namespace detail {

inline std::string mpc_format_component(std::ostream& out, const mpfr_class& value)
{
    std::ostringstream component;
    component.imbue(out.getloc());
    component.flags(out.flags());
    component.precision(out.precision());
    component.fill(out.fill());
    component << value;

    std::string text = component.str();
    const char decimal_point = std::use_facet<std::numpunct<char>>(out.getloc()).decimal_point();
    if (decimal_point != '.') {
        std::replace(text.begin(), text.end(), '.', decimal_point);
    }
    return text;
}

} // namespace detail

inline std::ostream& operator<<(std::ostream& out, const mpc_class& value)
{
    out.width(0);
    const mpfr_class real(mpc_realref(value.mpc_data()), value.real_precision());
    const mpfr_class imag(mpc_imagref(value.mpc_data()), value.imag_precision());
    out << '(' << detail::mpc_format_component(out, real) << ','
        << detail::mpc_format_component(out, imag) << ')';
    return out;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                         std::is_same_v<typename std::decay_t<Expr>::result_type, mpc_class>,
                     int> = 0>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << mpc_class(expr);
}

inline std::istream& operator>>(std::istream& in, mpc_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    mpfr_class real = mpfr_class::with_precision(value.real_precision());
    mpfr_class imag = mpfr_class::with_precision(value.imag_precision());
    char open = '\0';
    char comma = '\0';
    char close = '\0';

    if ((in >> open) && open == '(' &&
        (in >> real) &&
        (in >> comma) && comma == ',' &&
        (in >> imag) &&
        (in >> close) && close == ')') {
        value = mpc_class(real, imag);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

namespace literals {

inline mpc_class operator"" _mpc_i(long double value)
{
    return mpc_class::with_precision(
        default_mpc_real_precision_bits(),
        default_mpc_imag_precision_bits(),
        0.0,
        static_cast<double>(value));
}

inline mpc_class operator"" _mpc_i(const char* value, std::size_t)
{
    const mpfr_prec_t real_precision = default_mpc_real_precision_bits();
    const mpfr_prec_t imag_precision = default_mpc_imag_precision_bits();
    mpfr_class real = mpfr_class::with_precision(real_precision);
    mpfr_class imag(value, imag_precision, 0);
    return mpc_class(real, imag);
}

} // namespace literals

using literals::operator"" _mpc_i;

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP
