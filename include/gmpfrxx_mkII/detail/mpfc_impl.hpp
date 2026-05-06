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

#ifndef GMPFRXX_MKII_DETAIL_MPFC_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPFC_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/mpf_impl.hpp>

#include <algorithm>
#include <istream>
#include <ostream>
#include <type_traits>
#include <utility>

#include <gmp.h>

namespace gmpxx {
class mpf_class;
} // namespace gmpxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename Scalar, typename>
void mpfc_assign_scalar(gmpxx::mpf_class& dest, Scalar value);

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

class mpfc_class {
public:
    mpfc_class()
        : real_(mpf_class::with_precision(default_mpf_precision_bits())),
          imag_(mpf_class::with_precision(default_mpf_precision_bits()))
    {
    }

    mpfc_class(const mpfc_class&) = default;
    mpfc_class(mpfc_class&&) noexcept = default;

    mpfc_class(const mpf_class& real_value, const mpf_class& imag_value)
        : real_(real_value), imag_(imag_value)
    {
    }

    explicit mpfc_class(const mpf_class& real_value)
        : real_(real_value), imag_(mpf_class::with_precision(real_value.precision()))
    {
    }

    template <
        typename Scalar,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>>>
    explicit mpfc_class(Scalar real_value)
        : real_(mpf_class::with_precision(default_mpf_precision_bits())),
          imag_(mpf_class::with_precision(default_mpf_precision_bits()))
    {
        gmpfrxx_mkII::detail::mpfc_assign_scalar(real_, real_value);
    }

    template <
        typename RealScalar,
        typename ImagScalar,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<RealScalar> &&
                                    gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<ImagScalar>>>
    mpfc_class(RealScalar real_value, ImagScalar imag_value)
        : real_(mpf_class::with_precision(default_mpf_precision_bits())),
          imag_(mpf_class::with_precision(default_mpf_precision_bits()))
    {
        gmpfrxx_mkII::detail::mpfc_assign_scalar(real_, real_value);
        gmpfrxx_mkII::detail::mpfc_assign_scalar(imag_, imag_value);
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfc_class>>>
    mpfc_class(const Expr& expr);

    ~mpfc_class() = default;

    mpfc_class& operator=(const mpfc_class&) = default;
    mpfc_class& operator=(mpfc_class&&) noexcept = default;

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfc_class>>>
    mpfc_class& operator=(const Expr& expr);

    mpfc_class& operator=(const mpf_class& real_value)
    {
        real_ = real_value;
        mpf_set_ui(imag_.mpf_data(), 0);
        return *this;
    }

    template <typename Scalar, typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>>>
    mpfc_class& operator=(Scalar real_value)
    {
        gmpfrxx_mkII::detail::mpfc_assign_scalar(real_, real_value);
        mpf_set_ui(imag_.mpf_data(), 0);
        return *this;
    }

    static mpfc_class with_precision(mp_bitcnt_t precision)
    {
        return mpfc_class(precision_tag{}, precision, precision);
    }

    static mpfc_class with_precision(mp_bitcnt_t real_precision, mp_bitcnt_t imag_precision)
    {
        return mpfc_class(precision_tag{}, real_precision, imag_precision);
    }

    static mpfc_class with_precision(mp_bitcnt_t precision, double real_value, double imag_value)
    {
        mpfc_class result = with_precision(precision);
        mpf_set_d(result.real_.mpf_data(), real_value);
        mpf_set_d(result.imag_.mpf_data(), imag_value);
        return result;
    }

    mpf_class& real() noexcept
    {
        return real_;
    }

    const mpf_class& real() const noexcept
    {
        return real_;
    }

    mpf_class& imag() noexcept
    {
        return imag_;
    }

    const mpf_class& imag() const noexcept
    {
        return imag_;
    }

    void real(const mpf_class& value)
    {
        real_ = value;
    }

    void imag(const mpf_class& value)
    {
        imag_ = value;
    }

    mp_bitcnt_t precision() const noexcept
    {
        return std::max(real_.precision(), imag_.precision());
    }

    mp_bitcnt_t get_prec() const noexcept
    {
        return precision();
    }

    mp_bitcnt_t real_precision() const noexcept
    {
        return real_.precision();
    }

    mp_bitcnt_t imag_precision() const noexcept
    {
        return imag_.precision();
    }

    void swap(mpfc_class& other) noexcept
    {
        real_.swap(other.real_);
        imag_.swap(other.imag_);
    }

private:
    struct precision_tag {};

    mpfc_class(precision_tag, mp_bitcnt_t real_precision, mp_bitcnt_t imag_precision)
        : real_(mpf_class::with_precision(real_precision)),
          imag_(mpf_class::with_precision(imag_precision))
    {
    }

    mpf_class real_;
    mpf_class imag_;
};

} // namespace gmpxx

namespace std {

template <>
struct common_type<gmpxx::mpz_class, gmpxx::mpfc_class> {
    using type = gmpxx::mpfc_class;
};

template <>
struct common_type<gmpxx::mpfc_class, gmpxx::mpz_class> {
    using type = gmpxx::mpfc_class;
};

template <>
struct common_type<gmpxx::mpq_class, gmpxx::mpfc_class> {
    using type = gmpxx::mpfc_class;
};

template <>
struct common_type<gmpxx::mpfc_class, gmpxx::mpq_class> {
    using type = gmpxx::mpfc_class;
};

template <>
struct common_type<gmpxx::mpf_class, gmpxx::mpfc_class> {
    using type = gmpxx::mpfc_class;
};

template <>
struct common_type<gmpxx::mpfc_class, gmpxx::mpf_class> {
    using type = gmpxx::mpfc_class;
};

template <>
struct common_type<gmpxx::mpfc_class, gmpxx::mpfc_class> {
    using type = gmpxx::mpfc_class;
};

#define GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, SCALAR) \
    template <>                                                \
    struct common_type<CLASS, SCALAR> {                        \
        using type = CLASS;                                    \
    };                                                         \
    template <>                                                \
    struct common_type<SCALAR, CLASS> {                        \
        using type = CLASS;                                    \
    }

#define GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(CLASS)              \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, char);            \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, signed char);     \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned char);   \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, wchar_t);         \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, char16_t);        \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, char32_t);        \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, short);           \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned short);  \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, int);             \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned int);    \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, long);            \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned long);   \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, long long);       \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned long long); \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, float);           \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, double)

GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(gmpxx::mpfc_class);

#undef GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES
#undef GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE

} // namespace std

namespace gmpxx {

inline std::ostream& operator<<(std::ostream& out, const mpfc_class& value)
{
    out.width(0);
    out << '(' << value.real() << ',' << value.imag() << ')';
    return out;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                         std::is_same_v<typename std::decay_t<Expr>::result_type, mpfc_class>,
                     int> = 0>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << mpfc_class(expr);
}

inline std::istream& operator>>(std::istream& in, mpfc_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    mpf_class real = mpf_class::with_precision(value.real_precision());
    mpf_class imag = mpf_class::with_precision(value.imag_precision());
    char open = '\0';
    char comma = '\0';
    char close = '\0';

    if ((in >> open) && open == '(' &&
        (in >> real) &&
        (in >> comma) && comma == ',' &&
        (in >> imag) &&
        (in >> close) && close == ')') {
        value.real(real);
        value.imag(imag);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

inline void swap(mpfc_class& lhs, mpfc_class& rhs) noexcept
{
    lhs.swap(rhs);
}

inline bool operator==(const mpfc_class& lhs, const mpfc_class& rhs)
{
    return mpf_cmp(lhs.real().mpf_data(), rhs.real().mpf_data()) == 0 &&
           mpf_cmp(lhs.imag().mpf_data(), rhs.imag().mpf_data()) == 0;
}

inline bool operator!=(const mpfc_class& lhs, const mpfc_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpfc_class& lhs, const mpf_class& rhs)
{
    return mpf_cmp(lhs.real().mpf_data(), rhs.mpf_data()) == 0 &&
           mpf_sgn(lhs.imag().mpf_data()) == 0;
}

inline bool operator==(const mpf_class& lhs, const mpfc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpfc_class& lhs, const mpf_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const mpf_class& lhs, const mpfc_class& rhs)
{
    return !(lhs == rhs);
}

} // namespace gmpxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename Scalar, typename = std::enable_if_t<is_supported_mpf_scalar_v<Scalar>>>
void mpfc_assign_scalar(gmpxx::mpf_class& dest, Scalar value)
{
    auto leaf = make_mpf_operand(value);
    mpf_evaluate(dest.mpf_data(), leaf, dest.precision());
}

template <typename T, typename = void>
struct is_mpfc_expression_operand : std::false_type {};

template <>
struct is_mpfc_expression_operand<gmpxx::mpfc_class> : std::true_type {};

template <>
struct is_mpfc_expression_operand<gmpxx::mpf_class> : std::true_type {};

template <typename T>
struct is_mpfc_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, gmpxx::mpfc_class>>>
    : std::true_type {};

template <typename T>
struct is_mpfc_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, gmpxx::mpf_class>>>
    : std::true_type {};

template <typename T>
struct is_mpfc_expression_operand<T, std::enable_if_t<is_supported_mpf_scalar_v<T>>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpfc_expression_operand_v =
    is_mpfc_expression_operand<std::decay_t<T>>::value;

template <typename T, typename = void>
struct is_mpfc_object_or_node : std::false_type {};

template <typename T>
struct is_mpfc_object_or_node<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, gmpxx::mpfc_class>>> : std::true_type {};

template <typename T>
struct is_mpfc_object_or_node<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     std::is_same_v<typename std::decay_t<T>::result_type, gmpxx::mpfc_class>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpfc_object_or_node_v = is_mpfc_object_or_node<T>::value;

inline object_leaf<gmpxx::mpfc_class> make_mpfc_operand(const gmpxx::mpfc_class& value)
{
    return object_leaf<gmpxx::mpfc_class>(value);
}

inline object_leaf<gmpxx::mpf_class> make_mpfc_operand(const gmpxx::mpf_class& value)
{
    return object_leaf<gmpxx::mpf_class>(value);
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_mpfc_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

template <typename Scalar, typename = std::enable_if_t<is_supported_mpf_scalar_v<Scalar>>>
auto make_mpfc_operand(Scalar value)
{
    using storage_type = normalized_mpf_scalar_t<Scalar>;
    return scalar_leaf<storage_type, gmpxx::mpfc_class>(static_cast<storage_type>(value));
}

inline mp_bitcnt_t mpfc_expression_real_precision(const object_leaf<gmpxx::mpfc_class>& expr)
{
    return expr.get().real_precision();
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const object_leaf<gmpxx::mpfc_class>& expr)
{
    return expr.get().imag_precision();
}

inline mp_bitcnt_t mpfc_expression_real_precision(const object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

template <typename T, typename Result>
mp_bitcnt_t mpfc_expression_real_precision(const scalar_leaf<T, Result>&)
{
    return gmpxx::default_mpf_precision_bits();
}

template <typename T, typename Result>
mp_bitcnt_t mpfc_expression_imag_precision(const scalar_leaf<T, Result>&)
{
    return gmpxx::default_mpf_precision_bits();
}

template <typename T>
mp_bitcnt_t mpfc_expression_real_precision(const scalar_leaf<T, gmpxx::mpf_class>&)
{
    return gmpxx::default_mpf_precision_bits();
}

template <typename T>
mp_bitcnt_t mpfc_expression_imag_precision(const scalar_leaf<T, gmpxx::mpf_class>&)
{
    return gmpxx::default_mpf_precision_bits();
}

template <typename Op, typename Expr, typename Result>
mp_bitcnt_t mpfc_expression_real_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpfc_expression_real_precision(expr.expr());
}

template <typename Op, typename Expr, typename Result>
mp_bitcnt_t mpfc_expression_imag_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpfc_expression_imag_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mp_bitcnt_t mpfc_expression_real_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return std::max(mpfc_expression_real_precision(expr.lhs()),
                    mpfc_expression_real_precision(expr.rhs()));
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mp_bitcnt_t mpfc_expression_imag_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return std::max(mpfc_expression_imag_precision(expr.lhs()),
                    mpfc_expression_imag_precision(expr.rhs()));
}

inline mp_bitcnt_t mpfc_expression_precision_max(mp_bitcnt_t real_precision, mp_bitcnt_t imag_precision)
{
    return std::max(real_precision, imag_precision);
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const object_leaf<gmpxx::mpfc_class>& expr)
{
    dest.real() = expr.get().real();
    dest.imag() = expr.get().imag();
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const object_leaf<gmpxx::mpf_class>& expr)
{
    dest.real() = expr.get();
    mpf_set_ui(dest.imag().mpf_data(), 0);
}

template <typename T, typename Result>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const scalar_leaf<T, Result>& expr)
{
    mpfc_assign_scalar(dest.real(), expr.value());
    mpf_set_ui(dest.imag().mpf_data(), 0);
}

template <typename T>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const scalar_leaf<T, gmpxx::mpf_class>& expr)
{
    mpfc_assign_scalar(dest.real(), expr.value());
    mpf_set_ui(dest.imag().mpf_data(), 0);
}

template <typename Expr, typename Result>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const unary_expr<pos_op, Expr, Result>& expr)
{
    mpfc_evaluate(dest, expr.expr());
}

template <typename Expr, typename Result>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const unary_expr<neg_op, Expr, Result>& expr)
{
    mpfc_evaluate(dest, expr.expr());
    mpf_neg(dest.real().mpf_data(), dest.real().mpf_data());
    mpf_neg(dest.imag().mpf_data(), dest.imag().mpf_data());
}

template <typename Op>
void mpfc_apply_binary(gmpxx::mpfc_class& dest, const gmpxx::mpfc_class& lhs, const gmpxx::mpfc_class& rhs)
{
    if constexpr (std::is_same_v<Op, add_op>) {
        mpf_add(dest.real().mpf_data(), lhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_add(dest.imag().mpf_data(), lhs.imag().mpf_data(), rhs.imag().mpf_data());
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpf_sub(dest.real().mpf_data(), lhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_sub(dest.imag().mpf_data(), lhs.imag().mpf_data(), rhs.imag().mpf_data());
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpf_t real_part;
        mpf_t imag_part;
        mpf_t temp;
        const mp_bitcnt_t precision = dest.precision();
        mpf_init2(real_part, precision);
        mpf_init2(imag_part, precision);
        mpf_init2(temp, precision);

        mpf_mul(real_part, lhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_mul(temp, lhs.imag().mpf_data(), rhs.imag().mpf_data());
        mpf_sub(real_part, real_part, temp);

        mpf_mul(imag_part, lhs.real().mpf_data(), rhs.imag().mpf_data());
        mpf_mul(temp, lhs.imag().mpf_data(), rhs.real().mpf_data());
        mpf_add(imag_part, imag_part, temp);

        mpf_set(dest.real().mpf_data(), real_part);
        mpf_set(dest.imag().mpf_data(), imag_part);
        mpf_clear(temp);
        mpf_clear(imag_part);
        mpf_clear(real_part);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpf_t denominator;
        mpf_t real_part;
        mpf_t imag_part;
        mpf_t temp;
        const mp_bitcnt_t precision = dest.precision();
        mpf_init2(denominator, precision);
        mpf_init2(real_part, precision);
        mpf_init2(imag_part, precision);
        mpf_init2(temp, precision);

        mpf_mul(denominator, rhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_mul(temp, rhs.imag().mpf_data(), rhs.imag().mpf_data());
        mpf_add(denominator, denominator, temp);

        mpf_mul(real_part, lhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_mul(temp, lhs.imag().mpf_data(), rhs.imag().mpf_data());
        mpf_add(real_part, real_part, temp);
        mpf_div(real_part, real_part, denominator);

        mpf_mul(imag_part, lhs.imag().mpf_data(), rhs.real().mpf_data());
        mpf_mul(temp, lhs.real().mpf_data(), rhs.imag().mpf_data());
        mpf_sub(imag_part, imag_part, temp);
        mpf_div(imag_part, imag_part, denominator);

        mpf_set(dest.real().mpf_data(), real_part);
        mpf_set(dest.imag().mpf_data(), imag_part);
        mpf_clear(temp);
        mpf_clear(imag_part);
        mpf_clear(real_part);
        mpf_clear(denominator);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPFC expression operation");
    }
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    const mp_bitcnt_t real_precision =
        mpfc_expression_precision_max(mpfc_expression_real_precision(expr.lhs()),
                                      mpfc_expression_real_precision(expr.rhs()));
    const mp_bitcnt_t imag_precision =
        mpfc_expression_precision_max(mpfc_expression_imag_precision(expr.lhs()),
                                      mpfc_expression_imag_precision(expr.rhs()));
    gmpxx::mpfc_class lhs = gmpxx::mpfc_class::with_precision(real_precision, imag_precision);
    gmpxx::mpfc_class rhs = gmpxx::mpfc_class::with_precision(real_precision, imag_precision);
    mpfc_evaluate(lhs, expr.lhs());
    mpfc_evaluate(rhs, expr.rhs());
    mpfc_apply_binary<Op>(dest, lhs, rhs);
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfc_expression_operand_v<Lhs> &&
                                        is_mpfc_expression_operand_v<Rhs> &&
                                        (is_mpfc_object_or_node_v<Lhs> ||
                                         is_mpfc_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfc_operand(std::forward<Rhs>(rhs));
    return binary_expr<add_op, decltype(left), decltype(right), gmpxx::mpfc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfc_expression_operand_v<Lhs> &&
                                        is_mpfc_expression_operand_v<Rhs> &&
                                        (is_mpfc_object_or_node_v<Lhs> ||
                                         is_mpfc_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfc_operand(std::forward<Rhs>(rhs));
    return binary_expr<sub_op, decltype(left), decltype(right), gmpxx::mpfc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfc_expression_operand_v<Lhs> &&
                                        is_mpfc_expression_operand_v<Rhs> &&
                                        (is_mpfc_object_or_node_v<Lhs> ||
                                         is_mpfc_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfc_operand(std::forward<Rhs>(rhs));
    return binary_expr<mul_op, decltype(left), decltype(right), gmpxx::mpfc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfc_expression_operand_v<Lhs> &&
                                        is_mpfc_expression_operand_v<Rhs> &&
                                        (is_mpfc_object_or_node_v<Lhs> ||
                                         is_mpfc_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfc_operand(std::forward<Rhs>(rhs));
    return binary_expr<div_op, decltype(left), decltype(right), gmpxx::mpfc_class>(
        std::move(left), std::move(right));
}

template <typename Expr, std::enable_if_t<is_mpfc_expression_operand_v<Expr> && is_mpfc_object_or_node_v<Expr>, long> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpfc_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), gmpxx::mpfc_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpfc_expression_operand_v<Expr> && is_mpfc_object_or_node_v<Expr>, long> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpfc_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), gmpxx::mpfc_class>(std::move(operand));
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

template <typename Expr, typename>
mpfc_class::mpfc_class(const Expr& expr)
    : real_(mpf_class::with_precision(gmpfrxx_mkII::detail::mpfc_expression_real_precision(expr))),
      imag_(mpf_class::with_precision(gmpfrxx_mkII::detail::mpfc_expression_imag_precision(expr)))
{
    gmpfrxx_mkII::detail::mpfc_evaluate(*this, expr);
}

template <typename Expr, typename>
mpfc_class& mpfc_class::operator=(const Expr& expr)
{
    mpfc_class temp = mpfc_class::with_precision(real_precision(), imag_precision());
    gmpfrxx_mkII::detail::mpfc_evaluate(temp, expr);
    swap(temp);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator+=(mpfc_class& lhs, Rhs&& rhs)
{
    lhs = lhs + std::forward<Rhs>(rhs);
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator-=(mpfc_class& lhs, Rhs&& rhs)
{
    lhs = lhs - std::forward<Rhs>(rhs);
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator*=(mpfc_class& lhs, Rhs&& rhs)
{
    lhs = lhs * std::forward<Rhs>(rhs);
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator/=(mpfc_class& lhs, Rhs&& rhs)
{
    lhs = lhs / std::forward<Rhs>(rhs);
    return lhs;
}

namespace literals {

inline mpfc_class operator"" _mpfc_i(long double value)
{
    return mpfc_class(mpf_class::with_precision(default_mpf_precision_bits()),
                      mpf_class(static_cast<double>(value)));
}

inline mpfc_class operator"" _mpfc_i(const char* value, std::size_t)
{
    const mp_bitcnt_t precision = default_mpf_precision_bits();
    return mpfc_class(mpf_class::with_precision(precision),
                      mpf_class(value, precision, 10));
}

} // namespace literals

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MPFC_IMPL_HPP
