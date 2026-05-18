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

#include <gmpfrxx_mkII/detail/common_type_macros.hpp>
#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/mpf_impl.hpp>

#include <algorithm>
#include <cmath>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <gmp.h>

namespace gmpxx {
class mpf_class;
class mpfc_class;
} // namespace gmpxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename Scalar, typename>
void mpfc_assign_scalar(gmpxx::mpf_class& dest, Scalar value);
gmpxx::mpf_class& mpfc_real_ref(gmpxx::mpfc_class& value) noexcept;
gmpxx::mpf_class& mpfc_imag_ref(gmpxx::mpfc_class& value) noexcept;

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
        imag_ = 0;
        return *this;
    }

    template <typename Scalar, typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>>>
    mpfc_class& operator=(Scalar real_value)
    {
        gmpfrxx_mkII::detail::mpfc_assign_scalar(real_, real_value);
        imag_ = 0;
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

    const mpf_class& real() const noexcept
    {
        return real_;
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
    friend mpf_class& ::gmpfrxx_mkII::detail::mpfc_real_ref(mpfc_class& value) noexcept;
    friend mpf_class& ::gmpfrxx_mkII::detail::mpfc_imag_ref(mpfc_class& value) noexcept;

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

GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(gmpxx::mpfc_class);

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

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>, int> = 0>
inline bool operator==(const mpfc_class& lhs, Scalar rhs)
{
    return lhs == mpf_class(rhs, lhs.real_precision());
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>, int> = 0>
inline bool operator==(Scalar lhs, const mpfc_class& rhs)
{
    return rhs == lhs;
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>, int> = 0>
inline bool operator!=(const mpfc_class& lhs, Scalar rhs)
{
    return !(lhs == rhs);
}

template <typename Scalar, std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<Scalar>, int> = 0>
inline bool operator!=(Scalar lhs, const mpfc_class& rhs)
{
    return !(lhs == rhs);
}

} // namespace gmpxx

namespace gmpfrxx_mkII {
namespace detail {

inline gmpxx::mpf_class& mpfc_real_ref(gmpxx::mpfc_class& value) noexcept
{
    return value.real_;
}

inline gmpxx::mpf_class& mpfc_imag_ref(gmpxx::mpfc_class& value) noexcept
{
    return value.imag_;
}

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

template <>
struct is_mpfc_expression_operand<gmpxx::mpz_class> : std::true_type {};

template <>
struct is_mpfc_expression_operand<gmpxx::mpq_class> : std::true_type {};

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
struct is_mpfc_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, gmpxx::mpz_class>>>
    : std::true_type {};

template <typename T>
struct is_mpfc_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, gmpxx::mpq_class>>>
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

inline borrowed_object_leaf<gmpxx::mpfc_class> make_mpfc_operand(const gmpxx::mpfc_class& value)
{
    return borrowed_object_leaf<gmpxx::mpfc_class>(value);
}

inline object_leaf<gmpxx::mpfc_class> make_mpfc_operand(gmpxx::mpfc_class&& value)
{
    return object_leaf<gmpxx::mpfc_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpf_class> make_mpfc_operand(const gmpxx::mpf_class& value)
{
    return borrowed_object_leaf<gmpxx::mpf_class>(value);
}

inline object_leaf<gmpxx::mpf_class> make_mpfc_operand(gmpxx::mpf_class&& value)
{
    return object_leaf<gmpxx::mpf_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpz_class> make_mpfc_operand(const gmpxx::mpz_class& value)
{
    return borrowed_object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpfc_operand(gmpxx::mpz_class&& value)
{
    return object_leaf<gmpxx::mpz_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpq_class> make_mpfc_operand(const gmpxx::mpq_class& value)
{
    return borrowed_object_leaf<gmpxx::mpq_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpfc_operand(gmpxx::mpq_class&& value)
{
    return object_leaf<gmpxx::mpq_class>(std::move(value));
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

inline mp_bitcnt_t mpfc_expression_real_precision(const borrowed_object_leaf<gmpxx::mpfc_class>& expr)
{
    return expr.get().real_precision();
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const object_leaf<gmpxx::mpfc_class>& expr)
{
    return expr.get().imag_precision();
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const borrowed_object_leaf<gmpxx::mpfc_class>& expr)
{
    return expr.get().imag_precision();
}

inline mp_bitcnt_t mpfc_expression_real_precision(const object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpfc_expression_real_precision(const borrowed_object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const borrowed_object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpfc_expression_real_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_real_precision(const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_real_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_real_precision(const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpfc_expression_imag_precision(const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

template <typename T, typename Result>
mp_bitcnt_t mpfc_expression_real_precision(const scalar_leaf<T, Result>&)
{
    return 0;
}

template <typename T, typename Result>
mp_bitcnt_t mpfc_expression_imag_precision(const scalar_leaf<T, Result>&)
{
    return 0;
}

template <typename T>
mp_bitcnt_t mpfc_expression_real_precision(const scalar_leaf<T, gmpxx::mpf_class>&)
{
    return 0;
}

template <typename T>
mp_bitcnt_t mpfc_expression_imag_precision(const scalar_leaf<T, gmpxx::mpf_class>&)
{
    return 0;
}

template <typename Expr>
mp_bitcnt_t mpfc_materialization_real_precision(const Expr& expr)
{
    mp_bitcnt_t precision = mpfc_expression_real_precision(expr);
    if (precision == 0) {
        precision = gmpxx::default_mpf_precision_bits();
    }
    return precision;
}

template <typename Expr>
mp_bitcnt_t mpfc_materialization_imag_precision(const Expr& expr)
{
    mp_bitcnt_t precision = mpfc_expression_imag_precision(expr);
    if (precision == 0) {
        precision = gmpxx::default_mpf_precision_bits();
    }
    return precision;
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
    mpfc_real_ref(dest) = expr.get().real();
    mpfc_imag_ref(dest) = expr.get().imag();
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const borrowed_object_leaf<gmpxx::mpfc_class>& expr)
{
    mpfc_real_ref(dest) = expr.get().real();
    mpfc_imag_ref(dest) = expr.get().imag();
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const object_leaf<gmpxx::mpf_class>& expr)
{
    mpfc_real_ref(dest) = expr.get();
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const borrowed_object_leaf<gmpxx::mpf_class>& expr)
{
    mpfc_real_ref(dest) = expr.get();
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const object_leaf<gmpxx::mpz_class>& expr)
{
    mpf_set_z(mpfc_real_ref(dest).mpf_data(), expr.get().mpz_data());
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const borrowed_object_leaf<gmpxx::mpz_class>& expr)
{
    mpf_set_z(mpfc_real_ref(dest).mpf_data(), expr.get().mpz_data());
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const object_leaf<gmpxx::mpq_class>& expr)
{
    mpf_set_q_at_precision(mpfc_real_ref(dest).mpf_data(),
                           expr.get().mpq_data(),
                           mpfc_real_ref(dest).precision());
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

inline void mpfc_evaluate(gmpxx::mpfc_class& dest, const borrowed_object_leaf<gmpxx::mpq_class>& expr)
{
    mpf_set_q_at_precision(mpfc_real_ref(dest).mpf_data(),
                           expr.get().mpq_data(),
                           mpfc_real_ref(dest).precision());
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

template <typename T, typename Result>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const scalar_leaf<T, Result>& expr)
{
    mpfc_assign_scalar(mpfc_real_ref(dest), expr.value());
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
}

template <typename T>
void mpfc_evaluate(gmpxx::mpfc_class& dest, const scalar_leaf<T, gmpxx::mpf_class>& expr)
{
    mpfc_assign_scalar(mpfc_real_ref(dest), expr.value());
    mpf_set_ui(mpfc_imag_ref(dest).mpf_data(), 0);
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
    mpf_neg(mpfc_real_ref(dest).mpf_data(), mpfc_real_ref(dest).mpf_data());
    mpf_neg(mpfc_imag_ref(dest).mpf_data(), mpfc_imag_ref(dest).mpf_data());
}

inline bool mpfc_expression_references(
    const gmpxx::mpfc_class& target,
    const object_leaf<gmpxx::mpfc_class>& expr)
{
    return &target == &expr.get();
}

inline bool mpfc_expression_references(
    const gmpxx::mpfc_class& target,
    const borrowed_object_leaf<gmpxx::mpfc_class>& expr)
{
    return &target == &expr.get();
}

inline bool mpfc_expression_references(const gmpxx::mpfc_class&, const object_leaf<gmpxx::mpf_class>&)
{
    return false;
}

inline bool mpfc_expression_references(const gmpxx::mpfc_class&, const borrowed_object_leaf<gmpxx::mpf_class>&)
{
    return false;
}

inline bool mpfc_expression_references(const gmpxx::mpfc_class&, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpfc_expression_references(const gmpxx::mpfc_class&, const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpfc_expression_references(const gmpxx::mpfc_class&, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

inline bool mpfc_expression_references(const gmpxx::mpfc_class&, const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

template <typename T>
struct is_mpfc_class_leaf : std::false_type {};

template <>
struct is_mpfc_class_leaf<object_leaf<gmpxx::mpfc_class>> : std::true_type {};

template <>
struct is_mpfc_class_leaf<borrowed_object_leaf<gmpxx::mpfc_class>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpfc_class_leaf_v = is_mpfc_class_leaf<std::decay_t<T>>::value;

template <typename Expr>
inline constexpr bool mpfc_materialization_precision_is_nonzero_v = false;

template <typename Op, typename Lhs, typename Rhs>
inline constexpr bool mpfc_materialization_precision_is_nonzero_v<binary_expr<Op, Lhs, Rhs, gmpxx::mpfc_class>> =
    is_mpfc_class_leaf_v<Lhs> && is_mpfc_class_leaf_v<Rhs>;

template <typename Expr>
mp_bitcnt_t mpfc_constructor_materialization_real_precision(const Expr& expr)
{
    mp_bitcnt_t precision = mpfc_expression_real_precision(expr);
    if constexpr (!mpfc_materialization_precision_is_nonzero_v<std::decay_t<Expr>>) {
        if (precision == 0) {
            precision = gmpxx::default_mpf_precision_bits();
        }
    }
    return precision;
}

template <typename Expr>
mp_bitcnt_t mpfc_constructor_materialization_imag_precision(const Expr& expr)
{
    mp_bitcnt_t precision = mpfc_expression_imag_precision(expr);
    if constexpr (!mpfc_materialization_precision_is_nonzero_v<std::decay_t<Expr>>) {
        if (precision == 0) {
            precision = gmpxx::default_mpf_precision_bits();
        }
    }
    return precision;
}

template <typename T, typename Result>
bool mpfc_expression_references(const gmpxx::mpfc_class&, const scalar_leaf<T, Result>&)
{
    return false;
}

template <typename Op, typename Expr, typename Result>
bool mpfc_expression_references(const gmpxx::mpfc_class& target, const unary_expr<Op, Expr, Result>& expr)
{
    return mpfc_expression_references(target, expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
bool mpfc_expression_references(const gmpxx::mpfc_class& target, const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return mpfc_expression_references(target, expr.lhs()) ||
           mpfc_expression_references(target, expr.rhs());
}

inline int mpfc_compare_abs_for_division(mpf_srcptr lhs, mpf_srcptr rhs)
{
    const int lhs_sign = mpf_sgn(lhs);
    const int rhs_sign = mpf_sgn(rhs);
    if (lhs_sign == 0 || rhs_sign == 0) {
        if (lhs_sign == rhs_sign) {
            return 0;
        }
        return lhs_sign == 0 ? -1 : 1;
    }

    long lhs_exponent = 0;
    long rhs_exponent = 0;
    const double lhs_mantissa = std::fabs(mpf_get_d_2exp(&lhs_exponent, lhs));
    const double rhs_mantissa = std::fabs(mpf_get_d_2exp(&rhs_exponent, rhs));
    if (lhs_exponent < rhs_exponent) {
        return -1;
    }
    if (lhs_exponent > rhs_exponent) {
        return 1;
    }
    if (lhs_mantissa < rhs_mantissa) {
        return -1;
    }
    if (lhs_mantissa > rhs_mantissa) {
        return 1;
    }
    return 0;
}

inline void mpfc_divide_smith(
    mpf_t real_dest,
    mpf_t imag_dest,
    const gmpxx::mpfc_class& lhs,
    const gmpxx::mpfc_class& rhs,
    mp_bitcnt_t precision)
{
    mpf_t ratio;
    mpf_t scale;
    mpf_init2(ratio, precision);
    mpf_init2(scale, precision);

    if (mpfc_compare_abs_for_division(rhs.imag().mpf_data(), rhs.real().mpf_data()) <= 0) {
        mpf_div(ratio, rhs.imag().mpf_data(), rhs.real().mpf_data());

        mpf_mul(scale, rhs.imag().mpf_data(), ratio);
        mpf_add(scale, rhs.real().mpf_data(), scale);
        mpf_ui_div(scale, 1, scale);

        mpf_mul(real_dest, lhs.imag().mpf_data(), ratio);
        mpf_add(real_dest, lhs.real().mpf_data(), real_dest);
        mpf_mul(real_dest, real_dest, scale);

        mpf_mul(imag_dest, lhs.real().mpf_data(), ratio);
        mpf_sub(imag_dest, lhs.imag().mpf_data(), imag_dest);
        mpf_mul(imag_dest, imag_dest, scale);
    } else {
        mpf_div(ratio, rhs.real().mpf_data(), rhs.imag().mpf_data());

        mpf_mul(scale, rhs.real().mpf_data(), ratio);
        mpf_add(scale, rhs.imag().mpf_data(), scale);
        mpf_ui_div(scale, 1, scale);

        mpf_mul(real_dest, lhs.real().mpf_data(), ratio);
        mpf_add(real_dest, lhs.imag().mpf_data(), real_dest);
        mpf_mul(real_dest, real_dest, scale);

        mpf_mul(imag_dest, lhs.imag().mpf_data(), ratio);
        mpf_sub(imag_dest, imag_dest, lhs.real().mpf_data());
        mpf_mul(imag_dest, imag_dest, scale);
    }

    mpf_clear(scale);
    mpf_clear(ratio);
}

template <typename Op>
void mpfc_apply_binary(gmpxx::mpfc_class& dest, const gmpxx::mpfc_class& lhs, const gmpxx::mpfc_class& rhs)
{
    if constexpr (std::is_same_v<Op, add_op>) {
        mpf_add(mpfc_real_ref(dest).mpf_data(), lhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_add(mpfc_imag_ref(dest).mpf_data(), lhs.imag().mpf_data(), rhs.imag().mpf_data());
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpf_sub(mpfc_real_ref(dest).mpf_data(), lhs.real().mpf_data(), rhs.real().mpf_data());
        mpf_sub(mpfc_imag_ref(dest).mpf_data(), lhs.imag().mpf_data(), rhs.imag().mpf_data());
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        if (&dest != &lhs && &dest != &rhs) {
            mpf_t temp;
            mpf_init2(temp, dest.precision());

            mpf_mul(mpfc_real_ref(dest).mpf_data(), lhs.real().mpf_data(), rhs.real().mpf_data());
            mpf_mul(temp, lhs.imag().mpf_data(), rhs.imag().mpf_data());
            mpf_sub(mpfc_real_ref(dest).mpf_data(), mpfc_real_ref(dest).mpf_data(), temp);

            mpf_mul(mpfc_imag_ref(dest).mpf_data(), lhs.real().mpf_data(), rhs.imag().mpf_data());
            mpf_mul(temp, lhs.imag().mpf_data(), rhs.real().mpf_data());
            mpf_add(mpfc_imag_ref(dest).mpf_data(), mpfc_imag_ref(dest).mpf_data(), temp);

            mpf_clear(temp);
            return;
        }

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

        mpf_set(mpfc_real_ref(dest).mpf_data(), real_part);
        mpf_set(mpfc_imag_ref(dest).mpf_data(), imag_part);
        mpf_clear(temp);
        mpf_clear(imag_part);
        mpf_clear(real_part);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        if (mpf_sgn(rhs.real().mpf_data()) == 0 &&
            mpf_sgn(rhs.imag().mpf_data()) == 0) {
            throw std::domain_error("mpfc division by zero");
        }
        if (&dest != &lhs && &dest != &rhs) {
            mpfc_divide_smith(mpfc_real_ref(dest).mpf_data(),
                              mpfc_imag_ref(dest).mpf_data(),
                              lhs,
                              rhs,
                              dest.precision());
            return;
        }

        mpf_t real_part;
        mpf_t imag_part;
        const mp_bitcnt_t precision = dest.precision();
        mpf_init2(real_part, precision);
        mpf_init2(imag_part, precision);
        mpfc_divide_smith(real_part, imag_part, lhs, rhs, precision);

        mpf_set(mpfc_real_ref(dest).mpf_data(), real_part);
        mpf_set(mpfc_imag_ref(dest).mpf_data(), imag_part);
        mpf_clear(imag_part);
        mpf_clear(real_part);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPFC expression operation");
    }
}

template <typename Expr>
void mpfc_evaluate_to_temporary(
    gmpxx::mpfc_class& temp,
    const Expr& expr,
    mp_bitcnt_t real_precision,
    mp_bitcnt_t imag_precision)
{
    temp = gmpxx::mpfc_class::with_precision(real_precision, imag_precision);
    mpfc_evaluate(temp, expr);
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
    if (mpfc_expression_references(dest, expr)) {
        gmpxx::mpfc_class lhs;
        gmpxx::mpfc_class rhs;
        mpfc_evaluate_to_temporary(lhs, expr.lhs(), real_precision, imag_precision);
        mpfc_evaluate_to_temporary(rhs, expr.rhs(), real_precision, imag_precision);
        mpfc_apply_binary<Op>(dest, lhs, rhs);
    } else if constexpr (is_mpfc_class_leaf_v<Lhs> &&
                         is_mpfc_class_leaf_v<Rhs>) {
        mpfc_apply_binary<Op>(dest, expr.lhs().get(), expr.rhs().get());
    } else if constexpr (is_mpfc_class_leaf_v<Rhs>) {
        mpfc_evaluate(dest, expr.lhs());
        mpfc_apply_binary<Op>(dest, dest, expr.rhs().get());
    } else if constexpr (is_mpfc_class_leaf_v<Lhs> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpfc_evaluate(dest, expr.rhs());
        mpfc_apply_binary<Op>(dest, expr.lhs().get(), dest);
    } else {
        gmpxx::mpfc_class rhs;
        mpfc_evaluate(dest, expr.lhs());
        mpfc_evaluate_to_temporary(rhs, expr.rhs(), real_precision, imag_precision);
        mpfc_apply_binary<Op>(dest, dest, rhs);
    }
}

template <typename Op, typename Rhs>
void mpfc_compound_assign(gmpxx::mpfc_class& lhs, Rhs&& rhs)
{
    auto operand = make_mpfc_operand(std::forward<Rhs>(rhs));
    using operand_type = std::decay_t<decltype(operand)>;
    if constexpr (is_mpfc_class_leaf_v<operand_type>) {
        mpfc_apply_binary<Op>(lhs, lhs, operand.get());
    } else {
        const mp_bitcnt_t real_precision = mpf_get_prec(mpfc_real_ref(lhs).mpf_data());
        const mp_bitcnt_t imag_precision = mpf_get_prec(mpfc_imag_ref(lhs).mpf_data());
        gmpxx::mpfc_class value = gmpxx::mpfc_class::with_precision(real_precision, imag_precision);
        mpfc_evaluate(value, operand);
        mpfc_apply_binary<Op>(lhs, lhs, value);
    }
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
    : real_(mpf_class::with_precision(gmpfrxx_mkII::detail::mpfc_constructor_materialization_real_precision(expr))),
      imag_(mpf_class::with_precision(gmpfrxx_mkII::detail::mpfc_constructor_materialization_imag_precision(expr)))
{
    gmpfrxx_mkII::detail::mpfc_evaluate(*this, expr);
}

template <typename Expr, typename>
mpfc_class& mpfc_class::operator=(const Expr& expr)
{
    if (gmpfrxx_mkII::detail::mpfc_expression_references(*this, expr)) {
        mpfc_class temp = mpfc_class::with_precision(real_precision(), imag_precision());
        gmpfrxx_mkII::detail::mpfc_evaluate(temp, expr);
        swap(temp);
    } else {
        gmpfrxx_mkII::detail::mpfc_evaluate(*this, expr);
    }
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator+=(mpfc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfc_compound_assign<gmpfrxx_mkII::detail::add_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator-=(mpfc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfc_compound_assign<gmpfrxx_mkII::detail::sub_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator*=(mpfc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfc_compound_assign<gmpfrxx_mkII::detail::mul_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<Rhs>, int> = 0>
inline mpfc_class& operator/=(mpfc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfc_compound_assign<gmpfrxx_mkII::detail::div_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

namespace literals {

inline mpf_class parse_mpfc_imaginary_literal_mpf(const char* value, mp_bitcnt_t precision)
{
    if (value == nullptr) {
        return mpf_class(value, precision, 10);
    }

    std::string text(value);
    std::size_t pos = 0;
    if (pos < text.size() && (text[pos] == '+' || text[pos] == '-')) {
        ++pos;
    }

    if (pos + 2 < text.size() && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X')) {
        text.erase(pos, 2);
        return mpf_class(text.c_str(), precision, 16);
    }

    if (pos + 2 < text.size() && text[pos] == '0' && (text[pos + 1] == 'b' || text[pos + 1] == 'B')) {
        text.erase(pos, 2);
        return mpf_class(text.c_str(), precision, 2);
    }

    return mpf_class(value, precision, 10);
}

inline mpfc_class operator"" _mpfc_i(long double value)
{
    // Numeric user-defined literals receive an already-rounded long double
    // value rather than the source spelling.  Keep this path aligned with the
    // ordinary MPF floating constructor, which is double-based; use the string
    // literal form such as "0.1"_mpfc_i when decimal text should be parsed at
    // the MPF default precision.
    return mpfc_class(mpf_class::with_precision(default_mpf_precision_bits()),
                      mpf_class(static_cast<double>(value)));
}

inline mpfc_class operator"" _mpfc_i(const char* value, std::size_t)
{
    const mp_bitcnt_t precision = default_mpf_precision_bits();
    return mpfc_class(mpf_class::with_precision(precision),
                      parse_mpfc_imaginary_literal_mpf(value, precision));
}

} // namespace literals

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MPFC_IMPL_HPP
