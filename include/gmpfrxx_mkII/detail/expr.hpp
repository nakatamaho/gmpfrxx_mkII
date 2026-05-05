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

template <typename T>
class object_leaf {
public:
    using result_type = T;

    explicit object_leaf(const T& value) noexcept : value_(&value) {}

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
class unary_expr {
public:
    using op_type = Op;
    using expr_type = Expr;
    using result_type = Result;

    explicit unary_expr(Expr expr) : expr_(std::move(expr)) {}

    const Expr& expr() const noexcept { return expr_; }

private:
    Expr expr_;
};

template <typename Op, typename Lhs, typename Rhs, typename Result>
class binary_expr {
public:
    using op_type = Op;
    using lhs_type = Lhs;
    using rhs_type = Rhs;
    using result_type = Result;

    binary_expr(Lhs lhs, Rhs rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    const Lhs& lhs() const noexcept { return lhs_; }
    const Rhs& rhs() const noexcept { return rhs_; }

private:
    Lhs lhs_;
    Rhs rhs_;
};

template <typename T>
struct is_expression_node<object_leaf<T>> : std::true_type {};

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

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_EXPR_HPP
