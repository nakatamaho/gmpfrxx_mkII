#ifndef GMPFRXX_MKII_DETAIL_ZQ_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_ZQ_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>

#include <gmp.h>

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace gmpxx {

class mpz_class {
public:
    mpz_class()
    {
        mpz_init(value_);
    }

    mpz_class(const mpz_class& other)
    {
        mpz_init_set(value_, other.value_);
    }

    mpz_class(mpz_class&& other) noexcept
    {
        mpz_init(value_);
        mpz_swap(value_, other.value_);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    explicit mpz_class(T value)
    {
        mpz_init(value_);
        set_integral(value);
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpz_class>>>
    mpz_class(const Expr& expr);

    ~mpz_class()
    {
        mpz_clear(value_);
    }

    mpz_class& operator=(const mpz_class& other)
    {
        if (this != &other) {
            mpz_set(value_, other.value_);
        }
        return *this;
    }

    mpz_class& operator=(mpz_class&& other) noexcept
    {
        if (this != &other) {
            mpz_swap(value_, other.value_);
        }
        return *this;
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpz_class>>>
    mpz_class& operator=(const Expr& expr);

    const mpz_t& mpz_data() const noexcept
    {
        return value_;
    }

    mpz_t& mpz_data() noexcept
    {
        return value_;
    }

private:
    void set_unsigned(std::uint64_t value)
    {
        if (value == 0) {
            mpz_set_ui(value_, 0);
            return;
        }
        mpz_import(value_, 1, -1, sizeof(value), 0, 0, &value);
    }

    template <typename T>
    void set_integral(T value)
    {
        using clean_type = std::remove_cv_t<T>;
        if constexpr (std::is_signed_v<clean_type>) {
            const std::int64_t signed_value = static_cast<std::int64_t>(value);
            std::uint64_t magnitude = static_cast<std::uint64_t>(signed_value);
            const bool negative = signed_value < 0;
            if (negative) {
                magnitude = ~magnitude + std::uint64_t{1};
            }
            set_unsigned(magnitude);
            if (negative) {
                mpz_neg(value_, value_);
            }
        } else {
            set_unsigned(static_cast<std::uint64_t>(value));
        }
    }

    mpz_t value_;
};

class mpq_class {
public:
    mpq_class()
    {
        mpq_init(value_);
    }

    mpq_class(const mpq_class& other)
    {
        mpq_init(value_);
        mpq_set(value_, other.value_);
    }

    mpq_class(mpq_class&& other) noexcept
    {
        mpq_init(value_);
        mpq_swap(value_, other.value_);
    }

    explicit mpq_class(const mpz_class& numerator)
    {
        mpq_init(value_);
        mpq_set_z(value_, numerator.mpz_data());
    }

    mpq_class(const mpz_class& numerator, const mpz_class& denominator)
    {
        mpq_init(value_);
        mpq_set_num(value_, numerator.mpz_data());
        mpq_set_den(value_, denominator.mpz_data());
        mpq_canonicalize(value_);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    explicit mpq_class(T value) : mpq_class(mpz_class(value))
    {
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpq_class>>>
    mpq_class(const Expr& expr);

    ~mpq_class()
    {
        mpq_clear(value_);
    }

    mpq_class& operator=(const mpq_class& other)
    {
        if (this != &other) {
            mpq_set(value_, other.value_);
        }
        return *this;
    }

    mpq_class& operator=(mpq_class&& other) noexcept
    {
        if (this != &other) {
            mpq_swap(value_, other.value_);
        }
        return *this;
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpq_class>>>
    mpq_class& operator=(const Expr& expr);

    const mpq_t& mpq_data() const noexcept
    {
        return value_;
    }

    mpq_t& mpq_data() noexcept
    {
        return value_;
    }

private:
    mpq_t value_;
};

} // namespace gmpxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename T, typename = void>
struct is_zq_expression_operand : std::false_type {};

template <>
struct is_zq_expression_operand<gmpxx::mpz_class> : std::true_type {};

template <>
struct is_zq_expression_operand<gmpxx::mpq_class> : std::true_type {};

template <typename T>
struct is_zq_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> &&
                     (std::is_same_v<typename T::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename T::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_zq_expression_operand_v =
    is_zq_expression_operand<std::decay_t<T>>::value;

template <typename T, typename = void>
struct is_zq_object_or_node : std::false_type {};

template <typename T>
struct is_zq_object_or_node<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, gmpxx::mpz_class> ||
                     std::is_same_v<std::decay_t<T>, gmpxx::mpq_class>>> : std::true_type {};

template <typename T>
struct is_zq_object_or_node<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     (std::is_same_v<typename std::decay_t<T>::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename std::decay_t<T>::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_zq_object_or_node_v = is_zq_object_or_node<T>::value;

inline object_leaf<gmpxx::mpz_class> make_zq_operand(const gmpxx::mpz_class& value)
{
    return object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_zq_operand(const gmpxx::mpq_class& value)
{
    return object_leaf<gmpxx::mpq_class>(value);
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_zq_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

template <typename Op, typename LhsResult, typename RhsResult>
struct zq_binary_result {
    using type = std::conditional_t<
        std::is_same_v<Op, div_op> ||
            std::is_same_v<LhsResult, gmpxx::mpq_class> ||
            std::is_same_v<RhsResult, gmpxx::mpq_class>,
        gmpxx::mpq_class,
        gmpxx::mpz_class>;
};

template <typename Op, typename Lhs, typename Rhs>
using zq_binary_result_t = typename zq_binary_result<
    Op,
    typename std::decay_t<Lhs>::result_type,
    typename std::decay_t<Rhs>::result_type>::type;

inline void mpz_evaluate(mpz_t dest, const object_leaf<gmpxx::mpz_class>& expr)
{
    mpz_set(dest, expr.get().mpz_data());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpz_evaluate(mpz_t dest, const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    static_assert(std::is_same_v<Result, gmpxx::mpz_class>, "MPZ evaluation requires an MPZ expression");
    mpz_t lhs;
    mpz_t rhs;
    mpz_init(lhs);
    mpz_init(rhs);
    mpz_evaluate(lhs, expr.lhs());
    mpz_evaluate(rhs, expr.rhs());

    if constexpr (std::is_same_v<Op, add_op>) {
        mpz_add(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpz_sub(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpz_mul(dest, lhs, rhs);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPZ expression operation");
    }

    mpz_clear(rhs);
    mpz_clear(lhs);
}

inline void mpq_evaluate(mpq_t dest, const object_leaf<gmpxx::mpz_class>& expr)
{
    mpq_set_z(dest, expr.get().mpz_data());
}

inline void mpq_evaluate(mpq_t dest, const object_leaf<gmpxx::mpq_class>& expr)
{
    mpq_set(dest, expr.get().mpq_data());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpq_evaluate(mpq_t dest, const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    mpq_t lhs;
    mpq_t rhs;
    mpq_init(lhs);
    mpq_init(rhs);
    mpq_evaluate(lhs, expr.lhs());
    mpq_evaluate(rhs, expr.rhs());

    if constexpr (std::is_same_v<Op, add_op>) {
        mpq_add(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpq_sub(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpq_mul(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpq_div(dest, lhs, rhs);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPQ expression operation");
    }
    mpq_canonicalize(dest);

    mpq_clear(rhs);
    mpq_clear(lhs);
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_zq_expression_operand_v<Lhs> &&
                                        is_zq_expression_operand_v<Rhs>,
                                    bool> = true>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_zq_operand(std::forward<Lhs>(lhs));
    auto right = make_zq_operand(std::forward<Rhs>(rhs));
    using result_type = zq_binary_result_t<add_op, decltype(left), decltype(right)>;
    return binary_expr<add_op, decltype(left), decltype(right), result_type>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_zq_expression_operand_v<Lhs> &&
                                        is_zq_expression_operand_v<Rhs>,
                                    bool> = true>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_zq_operand(std::forward<Lhs>(lhs));
    auto right = make_zq_operand(std::forward<Rhs>(rhs));
    using result_type = zq_binary_result_t<sub_op, decltype(left), decltype(right)>;
    return binary_expr<sub_op, decltype(left), decltype(right), result_type>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_zq_expression_operand_v<Lhs> &&
                                        is_zq_expression_operand_v<Rhs>,
                                    bool> = true>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_zq_operand(std::forward<Lhs>(lhs));
    auto right = make_zq_operand(std::forward<Rhs>(rhs));
    using result_type = zq_binary_result_t<mul_op, decltype(left), decltype(right)>;
    return binary_expr<mul_op, decltype(left), decltype(right), result_type>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_zq_expression_operand_v<Lhs> &&
                                        is_zq_expression_operand_v<Rhs>,
                                    bool> = true>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_zq_operand(std::forward<Lhs>(lhs));
    auto right = make_zq_operand(std::forward<Rhs>(rhs));
    using result_type = zq_binary_result_t<div_op, decltype(left), decltype(right)>;
    return binary_expr<div_op, decltype(left), decltype(right), result_type>(
        std::move(left), std::move(right));
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

template <typename Expr, typename>
mpz_class::mpz_class(const Expr& expr)
{
    mpz_init(value_);
    gmpfrxx_mkII::detail::mpz_evaluate(value_, expr);
}

template <typename Expr, typename>
mpz_class& mpz_class::operator=(const Expr& expr)
{
    gmpfrxx_mkII::detail::mpz_evaluate(value_, expr);
    return *this;
}

template <typename Expr, typename>
mpq_class::mpq_class(const Expr& expr)
{
    mpq_init(value_);
    gmpfrxx_mkII::detail::mpq_evaluate(value_, expr);
    mpq_canonicalize(value_);
}

template <typename Expr, typename>
mpq_class& mpq_class::operator=(const Expr& expr)
{
    gmpfrxx_mkII::detail::mpq_evaluate(value_, expr);
    mpq_canonicalize(value_);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_ZQ_IMPL_HPP
