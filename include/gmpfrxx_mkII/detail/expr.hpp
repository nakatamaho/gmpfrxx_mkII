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

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_EXPR_HPP
