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

#ifndef GMPFRXX_MKII_DETAIL_EXPR_HPP
#define GMPFRXX_MKII_DETAIL_EXPR_HPP

#include <gmpfrxx_mkII/detail/type_traits.hpp>

#include <optional>
#include <type_traits>
#include <utility>

namespace gmpfrxx_mkII {
namespace detail {

struct add_op {};
struct sub_op {};
struct mul_op {};
struct div_op {};
struct neg_op {};
struct pos_op {};
struct com_op {};
struct bit_and_op {};
struct bit_or_op {};
struct bit_xor_op {};
struct shl_op {};
struct shr_op {};

// Some public operator templates intentionally use distinct dummy SFINAE
// parameter types across numeric domains.  This keeps cross-domain candidates
// from becoming identical redeclarations when multiple implementation headers
// are included together.

template <typename T>
class object_leaf {
public:
    using result_type = T;

    explicit object_leaf(const T& value) noexcept : value_(&value) {}

    explicit object_leaf(T&& value)
        : owned_(std::move(value)), value_(&*owned_)
    {
    }

    object_leaf(const object_leaf& other)
        : owned_(other.owned_), value_(owned_ ? &*owned_ : other.value_)
    {
    }

    object_leaf(object_leaf&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : owned_(std::move(other.owned_)), value_(owned_ ? &*owned_ : other.value_)
    {
    }

    object_leaf& operator=(const object_leaf& other)
    {
        if (this != &other) {
            owned_ = other.owned_;
            value_ = owned_ ? &*owned_ : other.value_;
        }
        return *this;
    }

    object_leaf& operator=(object_leaf&& other) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        if (this != &other) {
            owned_ = std::move(other.owned_);
            value_ = owned_ ? &*owned_ : other.value_;
        }
        return *this;
    }

    const T& get() const noexcept { return *value_; }

private:
    std::optional<T> owned_;
    const T* value_;
};

template <typename T>
class borrowed_object_leaf {
public:
    using result_type = T;

    explicit borrowed_object_leaf(const T& value) noexcept : value_(&value) {}

    const T& get() const noexcept { return *value_; }

private:
    const T* value_;
};

template <typename T, typename Result>
class scalar_leaf {
public:
    using value_type = T;
    using result_type = Result;

    explicit scalar_leaf(T value) noexcept : value_(value) {}

    T value() const noexcept { return value_; }

private:
    T value_;
};

template <typename Op, typename Expr, typename Result>
class [[nodiscard]] unary_expr {
public:
    using op_type = Op;
    using expr_type = Expr;
    using result_type = Result;

    explicit unary_expr(Expr expr) : expr_(std::move(expr)) {}

    const Expr& expr() const & noexcept { return expr_; }
    Expr&& expr() && noexcept { return std::move(expr_); }

    Result eval() const { return Result(*this); }

private:
    Expr expr_;
};

template <typename Op, typename Lhs, typename Rhs, typename Result>
class [[nodiscard]] binary_expr {
public:
    using op_type = Op;
    using lhs_type = Lhs;
    using rhs_type = Rhs;
    using result_type = Result;

    binary_expr(Lhs lhs, Rhs rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    const Lhs& lhs() const noexcept { return lhs_; }
    const Rhs& rhs() const noexcept { return rhs_; }

    Result eval() const { return Result(*this); }

private:
    Lhs lhs_;
    Rhs rhs_;
};

template <typename T>
struct is_expression_node<object_leaf<T>> : std::true_type {};

template <typename T>
struct is_expression_node<borrowed_object_leaf<T>> : std::true_type {};

template <typename T, typename Result>
struct is_expression_node<scalar_leaf<T, Result>> : std::true_type {};

template <typename Op, typename Expr, typename Result>
struct is_expression_node<unary_expr<Op, Expr, Result>> : std::true_type {};

template <typename Op, typename Lhs, typename Rhs, typename Result>
struct is_expression_node<binary_expr<Op, Lhs, Rhs, Result>> : std::true_type {};

template <typename Expr, typename Result>
inline auto operator-(const unary_expr<neg_op, Expr, Result>& expr)
{
    return unary_expr<pos_op, Expr, Result>(expr.expr());
}

template <typename Expr, typename Result>
inline auto operator-(unary_expr<neg_op, Expr, Result>&& expr)
{
    return unary_expr<pos_op, Expr, Result>(std::move(expr).expr());
}

template <typename Expr, typename Result>
inline auto operator-(const unary_expr<pos_op, Expr, Result>& expr)
{
    return unary_expr<neg_op, Expr, Result>(expr.expr());
}

template <typename Expr, typename Result>
inline auto operator-(unary_expr<pos_op, Expr, Result>&& expr)
{
    return unary_expr<neg_op, Expr, Result>(std::move(expr).expr());
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace std {

template <typename T>
struct common_type<gmpfrxx_mkII::detail::object_leaf<T>> {
    using type = T;
};

template <typename T>
struct common_type<gmpfrxx_mkII::detail::borrowed_object_leaf<T>> {
    using type = T;
};

template <typename T, typename Result>
struct common_type<gmpfrxx_mkII::detail::scalar_leaf<T, Result>> {
    using type = Result;
};

template <typename Op, typename Expr, typename Result>
struct common_type<gmpfrxx_mkII::detail::unary_expr<Op, Expr, Result>> {
    using type = Result;
};

template <typename Op, typename Lhs, typename Rhs, typename Result>
struct common_type<gmpfrxx_mkII::detail::binary_expr<Op, Lhs, Rhs, Result>> {
    using type = Result;
};

template <typename T, typename U>
struct common_type<gmpfrxx_mkII::detail::object_leaf<T>, U>
    : common_type<T, typename common_type<U>::type> {};

template <typename T, typename U>
struct common_type<U, gmpfrxx_mkII::detail::object_leaf<T>>
    : common_type<typename common_type<U>::type, T> {};

template <typename T, typename U>
struct common_type<gmpfrxx_mkII::detail::borrowed_object_leaf<T>, U>
    : common_type<T, typename common_type<U>::type> {};

template <typename T, typename U>
struct common_type<U, gmpfrxx_mkII::detail::borrowed_object_leaf<T>>
    : common_type<typename common_type<U>::type, T> {};

template <typename T, typename Result, typename U>
struct common_type<gmpfrxx_mkII::detail::scalar_leaf<T, Result>, U>
    : common_type<Result, typename common_type<U>::type> {};

template <typename T, typename Result, typename U>
struct common_type<U, gmpfrxx_mkII::detail::scalar_leaf<T, Result>>
    : common_type<typename common_type<U>::type, Result> {};

template <typename Op, typename Expr, typename Result, typename U>
struct common_type<gmpfrxx_mkII::detail::unary_expr<Op, Expr, Result>, U>
    : common_type<Result, typename common_type<U>::type> {};

template <typename Op, typename Expr, typename Result, typename U>
struct common_type<U, gmpfrxx_mkII::detail::unary_expr<Op, Expr, Result>>
    : common_type<typename common_type<U>::type, Result> {};

template <typename Op, typename Lhs, typename Rhs, typename Result, typename U>
struct common_type<gmpfrxx_mkII::detail::binary_expr<Op, Lhs, Rhs, Result>, U>
    : common_type<Result, typename common_type<U>::type> {};

template <typename Op, typename Lhs, typename Rhs, typename Result, typename U>
struct common_type<U, gmpfrxx_mkII::detail::binary_expr<Op, Lhs, Rhs, Result>>
    : common_type<typename common_type<U>::type, Result> {};

template <typename T, typename U>
struct common_type<gmpfrxx_mkII::detail::object_leaf<T>,
                   gmpfrxx_mkII::detail::object_leaf<U>>
    : common_type<T, U> {};

template <typename T, typename U>
struct common_type<gmpfrxx_mkII::detail::borrowed_object_leaf<T>,
                   gmpfrxx_mkII::detail::borrowed_object_leaf<U>>
    : common_type<T, U> {};

template <typename T, typename U>
struct common_type<gmpfrxx_mkII::detail::borrowed_object_leaf<T>,
                   gmpfrxx_mkII::detail::object_leaf<U>>
    : common_type<T, U> {};

template <typename T, typename U>
struct common_type<gmpfrxx_mkII::detail::object_leaf<T>,
                   gmpfrxx_mkII::detail::borrowed_object_leaf<U>>
    : common_type<T, U> {};

template <typename T, typename LeftResult, typename U, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::scalar_leaf<T, LeftResult>,
                   gmpfrxx_mkII::detail::scalar_leaf<U, RightResult>>
    : common_type<LeftResult, RightResult> {};

template <typename T, typename LeftResult, typename U>
struct common_type<gmpfrxx_mkII::detail::scalar_leaf<T, LeftResult>,
                   gmpfrxx_mkII::detail::object_leaf<U>>
    : common_type<LeftResult, U> {};

template <typename T, typename U, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::object_leaf<T>,
                   gmpfrxx_mkII::detail::scalar_leaf<U, RightResult>>
    : common_type<T, RightResult> {};

template <typename T, typename LeftResult, typename U>
struct common_type<gmpfrxx_mkII::detail::scalar_leaf<T, LeftResult>,
                   gmpfrxx_mkII::detail::borrowed_object_leaf<U>>
    : common_type<LeftResult, U> {};

template <typename T, typename U, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::borrowed_object_leaf<T>,
                   gmpfrxx_mkII::detail::scalar_leaf<U, RightResult>>
    : common_type<T, RightResult> {};

template <typename LeftOp, typename LeftExpr, typename LeftResult,
          typename RightOp, typename RightExpr, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::unary_expr<LeftOp, LeftExpr, LeftResult>,
                   gmpfrxx_mkII::detail::unary_expr<RightOp, RightExpr, RightResult>>
    : common_type<LeftResult, RightResult> {};

template <typename LeftOp, typename LeftExpr, typename LeftResult,
          typename RightOp, typename RightLhs, typename RightRhs, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::unary_expr<LeftOp, LeftExpr, LeftResult>,
                   gmpfrxx_mkII::detail::binary_expr<RightOp, RightLhs, RightRhs, RightResult>>
    : common_type<LeftResult, RightResult> {};

template <typename LeftOp, typename LeftLhs, typename LeftRhs, typename LeftResult,
          typename RightOp, typename RightExpr, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::binary_expr<LeftOp, LeftLhs, LeftRhs, LeftResult>,
                   gmpfrxx_mkII::detail::unary_expr<RightOp, RightExpr, RightResult>>
    : common_type<LeftResult, RightResult> {};

template <typename LeftOp, typename LeftLhs, typename LeftRhs, typename LeftResult,
          typename RightOp, typename RightLhs, typename RightRhs, typename RightResult>
struct common_type<gmpfrxx_mkII::detail::binary_expr<LeftOp, LeftLhs, LeftRhs, LeftResult>,
                   gmpfrxx_mkII::detail::binary_expr<RightOp, RightLhs, RightRhs, RightResult>>
    : common_type<LeftResult, RightResult> {};

} // namespace std

#endif // GMPFRXX_MKII_DETAIL_EXPR_HPP
