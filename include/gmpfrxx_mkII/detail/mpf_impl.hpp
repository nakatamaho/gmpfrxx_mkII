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

#ifndef GMPFRXX_MKII_DETAIL_MPF_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPF_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <gmp.h>

namespace gmpxx {

inline mp_bitcnt_t builtin_default_mpf_precision_bits() noexcept
{
    return 512;
}

inline bool parse_mpf_precision_bits(const char* text, mp_bitcnt_t& out) noexcept
{
    if (text == nullptr || *text == '\0' || *text == '-') {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value == 0) {
        return false;
    }
    if (value > static_cast<unsigned long long>(std::numeric_limits<mp_bitcnt_t>::max())) {
        return false;
    }
    out = static_cast<mp_bitcnt_t>(value);
    return true;
}

inline mp_bitcnt_t load_default_mpf_precision_bits_from_environment() noexcept
{
    mp_bitcnt_t precision = builtin_default_mpf_precision_bits();
    parse_mpf_precision_bits(std::getenv("MPFXX_DEFAULT_PREC_BITS"), precision);
    return precision;
}

inline mp_bitcnt_t& mutable_default_mpf_precision_bits()
{
    static mp_bitcnt_t precision = load_default_mpf_precision_bits_from_environment();
    return precision;
}

inline mp_bitcnt_t default_mpf_precision_bits()
{
    return mutable_default_mpf_precision_bits();
}

inline void set_default_mpf_precision_bits(mp_bitcnt_t precision)
{
    if (precision > 0) {
        mutable_default_mpf_precision_bits() = precision;
    }
}

inline void reload_default_mpf_precision_bits_from_environment()
{
    mutable_default_mpf_precision_bits() = load_default_mpf_precision_bits_from_environment();
}

class mpf_class {
public:
    mpf_class() : mpf_class(precision_tag{}, default_mpf_precision_bits()) {}

    mpf_class(const mpf_class& other)
    {
        mpf_init2(value_, other.precision());
        mpf_set(value_, other.value_);
    }

    mpf_class(mpf_class&& other) noexcept
    {
        mpf_init2(value_, other.precision());
        mpf_swap(value_, other.value_);
    }

    explicit mpf_class(double value) : mpf_class(precision_tag{}, default_mpf_precision_bits())
    {
        mpf_set_d(value_, value);
    }

    mpf_class(double value, mp_bitcnt_t precision) : mpf_class(precision_tag{}, precision)
    {
        mpf_set_d(value_, value);
    }

    mpf_class(bool) = delete;
    mpf_class(bool, mp_bitcnt_t) = delete;

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    explicit mpf_class(T value) : mpf_class(precision_tag{}, default_mpf_precision_bits())
    {
        set_integral(value);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    mpf_class(T value, mp_bitcnt_t precision) : mpf_class(precision_tag{}, precision)
    {
        set_integral(value);
    }

    explicit mpf_class(mpf_srcptr value)
    {
        mpf_init2(value_, mpf_get_prec(value));
        mpf_set(value_, value);
    }

    mpf_class(mpf_srcptr value, mp_bitcnt_t precision)
    {
        mpf_init2(value_, precision);
        mpf_set(value_, value);
    }

    mpf_class(const mpf_class& other, mp_bitcnt_t precision)
    {
        mpf_init2(value_, precision);
        mpf_set(value_, other.value_);
    }

    mpf_class(const char* value, mp_bitcnt_t precision, int base = 10)
    {
        mpf_init2(value_, precision);
        if (value == nullptr || mpf_set_str(value_, value, base) != 0) {
            mpf_clear(value_);
            throw std::invalid_argument("invalid mpf_class decimal string");
        }
    }

    mpf_class(const std::string& value, mp_bitcnt_t precision, int base = 10)
        : mpf_class(value.c_str(), precision, base)
    {
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
    mpf_class(const Expr& expr);

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
    mpf_class(const Expr& expr, mp_bitcnt_t precision);

    ~mpf_class()
    {
        mpf_clear(value_);
    }

    mpf_class& operator=(const mpf_class& other)
    {
        if (this != &other) {
            mpf_set(value_, other.value_);
        }
        return *this;
    }

    mpf_class& operator=(mpf_class&& other) noexcept
    {
        if (this != &other) {
            if (precision() == other.precision()) {
                mpf_swap(value_, other.value_);
            } else {
                mpf_set(value_, other.value_);
            }
        }
        return *this;
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
    mpf_class& operator=(const Expr& expr);

    mpf_class& operator=(double value)
    {
        mpf_set_d(value_, value);
        return *this;
    }

    mpf_class& operator=(bool) = delete;

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    mpf_class& operator=(T value)
    {
        set_integral(value);
        return *this;
    }

    mpf_class& operator=(const char* value)
    {
        set(value);
        return *this;
    }

    mpf_class& operator=(const std::string& value)
    {
        set(value);
        return *this;
    }

    static mpf_class with_precision(mp_bitcnt_t precision)
    {
        return mpf_class(precision_tag{}, precision);
    }

    static mpf_class with_precision(mp_bitcnt_t precision, double value)
    {
        mpf_class result = with_precision(precision);
        mpf_set_d(result.value_, value);
        return result;
    }

    mp_bitcnt_t precision() const noexcept
    {
        return mpf_get_prec(value_);
    }

    mp_bitcnt_t get_prec() const noexcept
    {
        return precision();
    }

    double to_double() const
    {
        return mpf_get_d(value_);
    }

    void set(double value)
    {
        mpf_set_d(value_, value);
    }

    void set(const char* value)
    {
        if (set_str(value) != 0) {
            throw std::invalid_argument("invalid mpf_class decimal string");
        }
    }

    void set(const std::string& value)
    {
        set(value.c_str());
    }

    std::string get_str() const
    {
        return to_string();
    }

    std::string get_str(mp_exp_t& exponent, int base = 10, std::size_t n_digits = 0) const
    {
        gmpfrxx_mkII::detail::gmp_allocated_string raw_digits(
            mpf_get_str(nullptr, &exponent, base, n_digits, value_));
        if (!raw_digits) {
            throw std::runtime_error("mpf_get_str failed");
        }
        return std::string(raw_digits.c_str());
    }

    int set_str(const char* value, int base = 10)
    {
        if (value == nullptr) {
            return -1;
        }

        mpf_t temp;
        mpf_init2(temp, precision());
        const int rc = mpf_set_str(temp, value, base);
        if (rc == 0) {
            mpf_set(value_, temp);
        }
        mpf_clear(temp);
        return rc;
    }

    int set_str(const std::string& value, int base = 10)
    {
        return set_str(value.c_str(), base);
    }

    std::string to_string(std::size_t n_digits = 0) const
    {
        mp_exp_t exponent = 0;
        std::string digits = get_str(exponent, 10, n_digits);

        bool negative = false;
        if (!digits.empty() && digits.front() == '-') {
            negative = true;
            digits.erase(digits.begin());
        }

        if (digits == "0") {
            return "0";
        }

        std::string result;
        if (negative) {
            result.push_back('-');
        }

        if (exponent <= 0) {
            result += "0.";
            result.append(static_cast<std::size_t>(-exponent), '0');
            result += digits;
        } else if (static_cast<std::size_t>(exponent) >= digits.size()) {
            result += digits;
            result.append(static_cast<std::size_t>(exponent) - digits.size(), '0');
        } else {
            const auto split = static_cast<std::size_t>(exponent);
            result += digits.substr(0, split);
            result.push_back('.');
            result += digits.substr(split);
        }

        return result;
    }

    const mpf_t& mpf_data() const noexcept
    {
        return value_;
    }

    mpf_t& mpf_data() noexcept
    {
        return value_;
    }

    const mpf_t& get_mpf_t() const noexcept
    {
        return value_;
    }

    mpf_t& get_mpf_t() noexcept
    {
        return value_;
    }

    void swap(mpf_class& other) noexcept
    {
        mpf_swap(value_, other.value_);
    }

private:
    struct precision_tag {};

    mpf_class(precision_tag, mp_bitcnt_t precision)
    {
        mpf_init2(value_, precision);
        mpf_set_ui(value_, 0);
    }

    template <typename T>
    void set_integral(T value)
    {
        const mpz_class integer(value);
        mpf_set_z(value_, integer.mpz_data());
    }

    mpf_t value_;
};

inline void swap(mpf_class& lhs, mpf_class& rhs) noexcept
{
    lhs.swap(rhs);
}

inline std::ostream& operator<<(std::ostream& out, const mpf_class& value)
{
    try {
        char conversion = 'g';
        const auto floatfield = out.flags() & std::ios_base::floatfield;
        if (floatfield == std::ios_base::fixed) {
            conversion = 'f';
        } else if (floatfield == std::ios_base::scientific) {
            conversion = 'e';
        }
        if (out.flags() & std::ios_base::uppercase) {
            conversion = static_cast<char>(conversion - 'a' + 'A');
        }

        std::string format = (out.flags() & std::ios_base::showpoint) ? "%#.*F" : "%.*F";
        format.push_back(conversion);

        std::streamsize stream_precision = out.precision();
        if (stream_precision < 0) {
            stream_precision = 6;
        }
        if (stream_precision > static_cast<std::streamsize>(std::numeric_limits<int>::max())) {
            stream_precision = std::numeric_limits<int>::max();
        }

        char* raw = nullptr;
        const int count = gmp_asprintf(
            &raw, format.c_str(), static_cast<int>(stream_precision), value.mpf_data());
        gmpfrxx_mkII::detail::gmp_allocated_string formatted(raw);
        if (count < 0 || !formatted) {
            out.setstate(std::ios_base::badbit);
            return out;
        }

        std::string text(formatted.c_str());
        if ((out.flags() & std::ios_base::showpos) && mpf_sgn(value.mpf_data()) >= 0) {
            text.insert(0, "+");
        }

        const std::streamsize width = out.width();
        if (static_cast<std::streamsize>(text.size()) < width) {
            const auto fill_count = static_cast<std::size_t>(width - static_cast<std::streamsize>(text.size()));
            if (out.flags() & std::ios_base::left) {
                text.append(fill_count, out.fill());
            } else if (out.flags() & std::ios_base::internal) {
                std::size_t pos = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1 : 0;
                text.insert(pos, fill_count, out.fill());
            } else {
                text.insert(0, fill_count, out.fill());
            }
        }

        out << text;
        out.width(0);
    } catch (...) {
        out.setstate(std::ios_base::badbit);
    }
    return out;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                         std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>,
                     int> = 0>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << mpf_class(expr);
}

inline std::istream& operator>>(std::istream& in, mpf_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    int base = gmpfrxx_mkII::detail::gmp_stream_input_base(in);
    if (base == 0) {
        base = 10;
    }

    std::string token;
    const bool parsed_token = gmpfrxx_mkII::detail::gmp_read_float_token(in, token, base);
    const std::string parse_token = gmpfrxx_mkII::detail::gmp_strip_leading_plus(std::move(token));

    mpf_class tmp = mpf_class::with_precision(value.precision());
    if (parsed_token && tmp.set_str(parse_token, base) == 0) {
        value.swap(tmp);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

} // namespace gmpxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename T>
struct is_supported_mpf_scalar
    : std::bool_constant<is_supported_expression_integral_v<T> ||
                         std::is_same_v<std::remove_cv_t<T>, float> ||
                         std::is_same_v<std::remove_cv_t<T>, double>> {};

template <typename T>
inline constexpr bool is_supported_mpf_scalar_v =
    is_supported_mpf_scalar<std::remove_cv_t<T>>::value;

template <typename T, typename = void>
struct normalized_mpf_scalar;

template <typename T>
struct normalized_mpf_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                 std::is_signed_v<std::remove_cv_t<T>>>> {
    using type = std::int64_t;
};

template <typename T>
struct normalized_mpf_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                 std::is_unsigned_v<std::remove_cv_t<T>>>> {
    using type = std::uint64_t;
};

template <>
struct normalized_mpf_scalar<float> {
    using type = double;
};

template <>
struct normalized_mpf_scalar<double> {
    using type = double;
};

template <typename T>
using normalized_mpf_scalar_t = typename normalized_mpf_scalar<std::remove_cv_t<T>>::type;

template <typename T, typename = void>
struct is_mpf_expression_operand : std::false_type {};

template <>
struct is_mpf_expression_operand<gmpxx::mpf_class> : std::true_type {};

template <>
struct is_mpf_expression_operand<gmpxx::mpz_class> : std::true_type {};

template <>
struct is_mpf_expression_operand<gmpxx::mpq_class> : std::true_type {};

template <typename T>
struct is_mpf_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, gmpxx::mpf_class>>>
    : std::true_type {};

template <typename T>
struct is_mpf_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> &&
                     (std::is_same_v<typename T::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename T::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
struct is_mpf_expression_operand<T, std::enable_if_t<is_supported_mpf_scalar_v<T>>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpf_expression_operand_v =
    is_mpf_expression_operand<std::decay_t<T>>::value;

template <typename T, typename = void>
struct is_mpf_object_or_node : std::false_type {};

template <typename T>
struct is_mpf_object_or_node<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, gmpxx::mpf_class>>> : std::true_type {};

template <typename T>
struct is_mpf_object_or_node<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     std::is_same_v<typename std::decay_t<T>::result_type, gmpxx::mpf_class>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpf_object_or_node_v = is_mpf_object_or_node<T>::value;

inline object_leaf<gmpxx::mpf_class> make_mpf_operand(const gmpxx::mpf_class& value)
{
    return object_leaf<gmpxx::mpf_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpf_operand(const gmpxx::mpz_class& value)
{
    return object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpf_operand(const gmpxx::mpq_class& value)
{
    return object_leaf<gmpxx::mpq_class>(value);
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_mpf_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

template <typename Scalar, typename = std::enable_if_t<is_supported_mpf_scalar_v<Scalar>>>
auto make_mpf_operand(Scalar value)
{
    using storage_type = normalized_mpf_scalar_t<Scalar>;
    return scalar_leaf<storage_type, gmpxx::mpf_class>(static_cast<storage_type>(value));
}

inline mp_bitcnt_t mpf_expression_precision(const object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpf_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpf_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

template <typename T, typename Result>
mp_bitcnt_t mpf_expression_precision(const scalar_leaf<T, Result>&)
{
    return gmpxx::default_mpf_precision_bits();
}

template <typename Op, typename Expr, typename Result>
mp_bitcnt_t mpf_expression_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpf_expression_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mp_bitcnt_t mpf_expression_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return std::max(mpf_expression_precision(expr.lhs()), mpf_expression_precision(expr.rhs()));
}

inline void mpf_evaluate(mpf_t dest, const object_leaf<gmpxx::mpf_class>& expr, mp_bitcnt_t)
{
    mpf_set(dest, expr.get().mpf_data());
}

inline void mpf_set_q_exact(mpf_t dest, const mpq_t value, mp_bitcnt_t eval_precision)
{
    mpf_t numerator;
    mpf_t denominator;
    mpf_init2(numerator, eval_precision);
    mpf_init2(denominator, eval_precision);
    mpf_set_z(numerator, mpq_numref(value));
    mpf_set_z(denominator, mpq_denref(value));
    mpf_div(dest, numerator, denominator);
    mpf_clear(denominator);
    mpf_clear(numerator);
}

inline void mpf_evaluate(mpf_t dest, const object_leaf<gmpxx::mpz_class>& expr, mp_bitcnt_t)
{
    mpf_set_z(dest, expr.get().mpz_data());
}

inline void mpf_evaluate(mpf_t dest, const object_leaf<gmpxx::mpq_class>& expr, mp_bitcnt_t eval_precision)
{
    mpf_set_q_exact(dest, expr.get().mpq_data(), eval_precision);
}

template <typename T, typename Result>
void mpf_evaluate(mpf_t dest, const scalar_leaf<T, Result>& expr, mp_bitcnt_t)
{
    if constexpr (std::is_same_v<T, double>) {
        mpf_set_d(dest, expr.value());
    } else if constexpr (std::is_same_v<T, std::int64_t> ||
                         std::is_same_v<T, std::uint64_t>) {
        const gmpxx::mpz_class integer(expr.value());
        mpf_set_z(dest, integer.mpz_data());
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPF scalar leaf");
    }
}

inline bool mpf_expression_references(const mpf_t target, const object_leaf<gmpxx::mpf_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpf_data()[0]);
}

inline bool mpf_expression_references(const mpf_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpf_expression_references(const mpf_t, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

template <typename T, typename Result>
bool mpf_expression_references(const mpf_t, const scalar_leaf<T, Result>&)
{
    return false;
}

template <typename Op, typename Expr, typename Result>
bool mpf_expression_references(const mpf_t target, const unary_expr<Op, Expr, Result>& expr)
{
    return mpf_expression_references(target, expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
bool mpf_expression_references(const mpf_t target, const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return mpf_expression_references(target, expr.lhs()) ||
           mpf_expression_references(target, expr.rhs());
}

template <typename Op>
void mpf_apply_binary(mpf_t dest, const mpf_t lhs, const mpf_t rhs)
{
    if constexpr (std::is_same_v<Op, add_op>) {
        mpf_add(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpf_sub(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpf_mul(dest, lhs, rhs);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpf_div(dest, lhs, rhs);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPF expression operation");
    }
}

template <typename Expr, typename Result>
void mpf_evaluate(mpf_t dest, const unary_expr<pos_op, Expr, Result>& expr, mp_bitcnt_t eval_precision)
{
    mpf_evaluate(dest, expr.expr(), eval_precision);
}

template <typename Expr, typename Result>
void mpf_evaluate(mpf_t dest, const unary_expr<neg_op, Expr, Result>& expr, mp_bitcnt_t eval_precision)
{
    mpf_evaluate(dest, expr.expr(), eval_precision);
    mpf_neg(dest, dest);
}

template <typename Expr>
void mpf_evaluate_to_temporary(mpf_t temp, const Expr& expr, mp_bitcnt_t eval_precision)
{
    mpf_init2(temp, eval_precision);
    mpf_evaluate(temp, expr, eval_precision);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpf_evaluate(mpf_t dest, const binary_expr<Op, Lhs, Rhs, Result>& expr, mp_bitcnt_t eval_precision)
{
    if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        mpz_t exact;
        mpz_init(exact);
        mpz_evaluate(exact, expr);
        mpf_set_z(dest, exact);
        mpz_clear(exact);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        mpq_t exact;
        mpq_init(exact);
        mpq_evaluate(exact, expr);
        mpf_set_q_exact(dest, exact, eval_precision);
        mpq_clear(exact);
        return;
    }

    if (mpf_expression_references(dest, expr)) {
        mpf_t lhs;
        mpf_t rhs;
        mpf_evaluate_to_temporary(lhs, expr.lhs(), eval_precision);
        mpf_evaluate_to_temporary(rhs, expr.rhs(), eval_precision);
        mpf_apply_binary<Op>(dest, lhs, rhs);
        mpf_clear(rhs);
        mpf_clear(lhs);
        return;
    }

    if constexpr (std::is_same_v<Lhs, object_leaf<gmpxx::mpf_class>> &&
                  std::is_same_v<Rhs, object_leaf<gmpxx::mpf_class>>) {
        mpf_apply_binary<Op>(dest, expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
    } else if constexpr (std::is_same_v<Rhs, object_leaf<gmpxx::mpf_class>>) {
        mpf_evaluate(dest, expr.lhs(), eval_precision);
        mpf_apply_binary<Op>(dest, dest, expr.rhs().get().mpf_data());
    } else if constexpr (std::is_same_v<Lhs, object_leaf<gmpxx::mpf_class>> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpf_evaluate(dest, expr.rhs(), eval_precision);
        mpf_apply_binary<Op>(dest, expr.lhs().get().mpf_data(), dest);
    } else {
        mpf_t rhs;
        mpf_evaluate(dest, expr.lhs(), eval_precision);
        mpf_evaluate_to_temporary(rhs, expr.rhs(), eval_precision);
        mpf_apply_binary<Op>(dest, dest, rhs);
        mpf_clear(rhs);
    }
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpf_expression_operand_v<Lhs> &&
                                        is_mpf_expression_operand_v<Rhs> &&
                                        (is_mpf_object_or_node_v<Lhs> ||
                                         is_mpf_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = make_mpf_operand(std::forward<Rhs>(rhs));
    return binary_expr<add_op, decltype(left), decltype(right), gmpxx::mpf_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpf_expression_operand_v<Lhs> &&
                                        is_mpf_expression_operand_v<Rhs> &&
                                        (is_mpf_object_or_node_v<Lhs> ||
                                         is_mpf_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = make_mpf_operand(std::forward<Rhs>(rhs));
    return binary_expr<sub_op, decltype(left), decltype(right), gmpxx::mpf_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpf_expression_operand_v<Lhs> &&
                                        is_mpf_expression_operand_v<Rhs> &&
                                        (is_mpf_object_or_node_v<Lhs> ||
                                         is_mpf_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = make_mpf_operand(std::forward<Rhs>(rhs));
    return binary_expr<mul_op, decltype(left), decltype(right), gmpxx::mpf_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpf_expression_operand_v<Lhs> &&
                                        is_mpf_expression_operand_v<Rhs> &&
                                        (is_mpf_object_or_node_v<Lhs> ||
                                         is_mpf_object_or_node_v<Rhs>),
                                    long> = 0>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = make_mpf_operand(std::forward<Rhs>(rhs));
    return binary_expr<div_op, decltype(left), decltype(right), gmpxx::mpf_class>(
        std::move(left), std::move(right));
}

template <typename Expr, std::enable_if_t<is_mpf_expression_operand_v<Expr>, long> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpf_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), gmpxx::mpf_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpf_expression_operand_v<Expr>, long> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpf_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), gmpxx::mpf_class>(std::move(operand));
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

template <typename Expr, typename>
mpf_class::mpf_class(const Expr& expr)
{
    const mp_bitcnt_t precision = gmpfrxx_mkII::detail::mpf_expression_precision(expr);
    mpf_init2(value_, precision);
    gmpfrxx_mkII::detail::mpf_evaluate(value_, expr, precision);
}

template <typename Expr, typename>
mpf_class::mpf_class(const Expr& expr, mp_bitcnt_t precision)
{
    mpf_init2(value_, precision);
    gmpfrxx_mkII::detail::mpf_evaluate(value_, expr, precision);
}

template <typename Expr, typename>
mpf_class& mpf_class::operator=(const Expr& expr)
{
    const mp_bitcnt_t precision = this->precision();
    gmpfrxx_mkII::detail::mpf_evaluate(value_, expr, precision);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MPF_IMPL_HPP
