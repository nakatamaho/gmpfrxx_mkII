#ifndef GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>

#include <algorithm>
#include <type_traits>
#include <utility>

#include <mpfr.h>

namespace mpfrxx {

class mpfr_class {
public:
    mpfr_class() : mpfr_class(precision_tag{}, default_precision()) {}

    mpfr_class(const mpfr_class& other)
    {
        mpfr_init2(value_, other.precision());
        mpfr_set(value_, other.value_, default_rounding());
    }

    mpfr_class(mpfr_class&& other) noexcept
    {
        mpfr_init2(value_, other.precision());
        mpfr_swap(value_, other.value_);
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
    mpfr_class(const Expr& expr);

    ~mpfr_class()
    {
        mpfr_clear(value_);
    }

    mpfr_class& operator=(const mpfr_class& other)
    {
        if (this != &other) {
            mpfr_set(value_, other.value_, default_rounding());
        }
        return *this;
    }

    mpfr_class& operator=(mpfr_class&& other) noexcept
    {
        if (this != &other) {
            mpfr_swap(value_, other.value_);
        }
        return *this;
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
    mpfr_class& operator=(const Expr& expr);

    static mpfr_class with_precision(mpfr_prec_t precision, double value)
    {
        mpfr_class result = with_precision(precision);
        mpfr_set_d(result.value_, value, default_rounding());
        return result;
    }

    static mpfr_class with_precision(mpfr_prec_t precision)
    {
        return mpfr_class(precision_tag{}, precision);
    }

    mpfr_prec_t precision() const noexcept
    {
        return mpfr_get_prec(value_);
    }

    double to_double() const
    {
        return mpfr_get_d(value_, default_rounding());
    }

    void set(double value)
    {
        mpfr_set_d(value_, value, default_rounding());
    }

    const mpfr_t& mpfr_data() const noexcept
    {
        return value_;
    }

    mpfr_t& mpfr_data() noexcept
    {
        return value_;
    }

    static constexpr mpfr_prec_t default_precision() noexcept
    {
        return 53;
    }

    static constexpr mpfr_rnd_t default_rounding() noexcept
    {
        return MPFR_RNDN;
    }

private:
    struct precision_tag {};

    mpfr_class(precision_tag, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        mpfr_set_ui(value_, 0, default_rounding());
    }

    mpfr_t value_;
};

} // namespace mpfrxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename T, typename = void>
struct is_mpfr_expression_operand : std::false_type {};

template <>
struct is_mpfr_expression_operand<mpfrxx::mpfr_class> : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, mpfrxx::mpfr_class>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_expression_operand_v =
    is_mpfr_expression_operand<std::decay_t<T>>::value;

inline object_leaf<mpfrxx::mpfr_class> make_mpfr_operand(const mpfrxx::mpfr_class& value)
{
    return object_leaf<mpfrxx::mpfr_class>(value);
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_mpfr_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return expr.get().precision();
}

template <typename T, typename Result>
mpfr_prec_t mpfr_expression_precision(const scalar_leaf<T, Result>&)
{
    return mpfrxx::mpfr_class::default_precision();
}

template <typename Op, typename Expr, typename Result>
mpfr_prec_t mpfr_expression_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpfr_expression_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mpfr_prec_t mpfr_expression_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return std::max(mpfr_expression_precision(expr.lhs()), mpfr_expression_precision(expr.rhs()));
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<mpfrxx::mpfr_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set(dest, expr.get().mpfr_data(), rnd);
}

template <typename T, typename Result>
void mpfr_evaluate(mpfr_t, const scalar_leaf<T, Result>&, mpfr_prec_t, mpfr_rnd_t) = delete;

template <typename Expr, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const unary_expr<pos_op, Expr, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    mpfr_evaluate(dest, expr.expr(), eval_precision, rnd);
}

template <typename Expr, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const unary_expr<neg_op, Expr, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    mpfr_evaluate(dest, expr.expr(), eval_precision, rnd);
    mpfr_neg(dest, dest, rnd);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    mpfr_t lhs;
    mpfr_t rhs;
    mpfr_init2(lhs, eval_precision);
    mpfr_init2(rhs, eval_precision);

    mpfr_evaluate(lhs, expr.lhs(), eval_precision, rnd);
    mpfr_evaluate(rhs, expr.rhs(), eval_precision, rnd);

    if constexpr (std::is_same_v<Op, add_op>) {
        mpfr_add(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpfr_sub(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpfr_mul(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpfr_div(dest, lhs, rhs, rnd);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPFR expression operation");
    }

    mpfr_clear(rhs);
    mpfr_clear(lhs);
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs>,
                                    int> = 0>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<add_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs>,
                                    int> = 0>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<sub_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs>,
                                    int> = 0>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<mul_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs>,
                                    int> = 0>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<div_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Expr, std::enable_if_t<is_mpfr_expression_operand_v<Expr>, int> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpfr_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), mpfrxx::mpfr_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpfr_expression_operand_v<Expr>, int> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpfr_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), mpfrxx::mpfr_class>(std::move(operand));
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

template <typename Expr, typename>
mpfr_class::mpfr_class(const Expr& expr)
{
    const mpfr_prec_t precision = gmpfrxx_mkII::detail::mpfr_expression_precision(expr);
    mpfr_init2(value_, precision);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, default_rounding());
}

template <typename Expr, typename>
mpfr_class& mpfr_class::operator=(const Expr& expr)
{
    const mpfr_prec_t precision = this->precision();
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, default_rounding());
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
