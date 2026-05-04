#ifndef GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/mpfr_impl.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <utility>

#include <mpc.h>

namespace mpfrxx {

class mpc_class {
public:
    mpc_class() : mpc_class(precision_tag{}, mpfrxx::default_precision_bits()) {}

    mpc_class(const mpc_class& other)
    {
        mpc_init2(value_, other.precision());
        mpc_set(value_, other.value_, default_rounding());
    }

    mpc_class(mpc_class&& other) noexcept
    {
        mpc_init2(value_, other.precision());
        mpc_swap(value_, other.value_);
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
        return mpc_class(precision_tag{}, precision);
    }

    static mpc_class with_precision(mpfr_prec_t precision, double real, double imag)
    {
        mpc_class result = with_precision(precision);
        mpc_set_d_d(result.value_, real, imag, default_rounding());
        return result;
    }

    mpfr_prec_t precision() const noexcept
    {
        return mpc_get_prec(value_);
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
        return MPC_RNDNN;
    }

private:
    struct precision_tag {};

    mpc_class(precision_tag, mpfr_prec_t precision)
    {
        mpc_init2(value_, precision);
        mpc_set_ui(value_, 0, default_rounding());
    }

    mpc_t value_;
};

} // namespace mpfrxx

namespace gmpfrxx_mkII {
namespace detail {

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

inline mpfr_prec_t mpc_expression_precision(const object_leaf<mpfrxx::mpc_class>& expr)
{
    return expr.get().precision();
}

inline mpfr_prec_t mpc_expression_precision(const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return expr.get().precision();
}

inline mpfr_prec_t mpc_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mpfr_prec_t mpc_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

template <typename T, typename Result>
mpfr_prec_t mpc_expression_precision(const scalar_leaf<T, Result>&)
{
    return mpfrxx::default_precision_bits();
}

template <typename Op, typename Expr, typename Result>
mpfr_prec_t mpc_expression_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpc_expression_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mpfr_prec_t mpc_expression_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return std::max(mpc_expression_precision(expr.lhs()), mpc_expression_precision(expr.rhs()));
}

inline void mpc_evaluate(mpc_t dest, const object_leaf<mpfrxx::mpc_class>& expr, mpfr_prec_t, mpc_rnd_t rnd)
{
    mpc_set(dest, expr.get().mpc_data(), rnd);
}

inline void mpc_evaluate(mpc_t dest, const object_leaf<mpfrxx::mpfr_class>& expr, mpfr_prec_t, mpc_rnd_t rnd)
{
    mpc_set_fr(dest, expr.get().mpfr_data(), rnd);
}

inline void mpc_evaluate(mpc_t dest, const object_leaf<gmpxx::mpz_class>& expr, mpfr_prec_t, mpc_rnd_t rnd)
{
    mpc_set_z(dest, expr.get().mpz_data(), rnd);
}

inline void mpc_evaluate(mpc_t dest, const object_leaf<gmpxx::mpq_class>& expr, mpfr_prec_t, mpc_rnd_t rnd)
{
    mpc_set_q(dest, expr.get().mpq_data(), rnd);
}

template <typename T, typename Result>
void mpc_evaluate(mpc_t dest, const scalar_leaf<T, Result>& expr, mpfr_prec_t, mpc_rnd_t rnd)
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
void mpc_evaluate(mpc_t dest, const unary_expr<pos_op, Expr, Result>& expr, mpfr_prec_t eval_precision, mpc_rnd_t rnd)
{
    mpc_evaluate(dest, expr.expr(), eval_precision, rnd);
}

template <typename Expr, typename Result>
void mpc_evaluate(mpc_t dest, const unary_expr<neg_op, Expr, Result>& expr, mpfr_prec_t eval_precision, mpc_rnd_t rnd)
{
    mpc_evaluate(dest, expr.expr(), eval_precision, rnd);
    mpc_neg(dest, dest, rnd);
}

template <typename Expr>
void mpc_evaluate_to_temporary(mpc_t temp, const Expr& expr, mpfr_prec_t eval_precision, mpc_rnd_t rnd)
{
    mpc_init2(temp, eval_precision);
    mpc_evaluate(temp, expr, eval_precision, rnd);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpc_evaluate(mpc_t dest, const binary_expr<Op, Lhs, Rhs, Result>& expr, mpfr_prec_t eval_precision, mpc_rnd_t rnd)
{
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
    const mpfr_prec_t precision = gmpfrxx_mkII::detail::mpc_expression_precision(expr);
    mpc_init2(value_, precision);
    gmpfrxx_mkII::detail::mpc_evaluate(value_, expr, precision, default_rounding());
}

template <typename Expr, typename>
mpc_class& mpc_class::operator=(const Expr& expr)
{
    const mpfr_prec_t precision = this->precision();
    gmpfrxx_mkII::detail::mpc_evaluate(value_, expr, precision, default_rounding());
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP
