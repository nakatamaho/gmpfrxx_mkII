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

#ifndef GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP

#include <gmpfrxx_mkII/detail/common_type_macros.hpp>
#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/eval_context.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <istream>
#include <limits>
#include <locale>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <mpfr.h>

namespace gmpfrxx_mkII {
namespace detail {

inline std::string mpfr_trim_surrounding_whitespace(const char* text)
{
    if (text == nullptr) {
        return {};
    }

    const char* first = text;
    while (*first != '\0' && std::isspace(static_cast<unsigned char>(*first)) != 0) {
        ++first;
    }

    const char* last = first;
    while (*last != '\0') {
        ++last;
    }
    while (last != first && std::isspace(static_cast<unsigned char>(*(last - 1))) != 0) {
        --last;
    }

    return std::string(first, last);
}

class scoped_mpfr_init {
public:
    scoped_mpfr_init(mpfr_t value, mpfr_prec_t precision)
        : value_(value)
    {
        require_valid_mpfr_precision(precision);
        mpfr_init2(value_, precision);
    }

    scoped_mpfr_init(const scoped_mpfr_init&) = delete;
    scoped_mpfr_init& operator=(const scoped_mpfr_init&) = delete;

    ~scoped_mpfr_init()
    {
        if (active_) {
            mpfr_clear(value_);
        }
    }

    void release() noexcept
    {
        active_ = false;
    }

private:
    mpfr_ptr value_;
    bool active_{true};
};

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

struct evaluation_context {
    mpfr_prec_t precision;
    mpfr_rnd_t rounding_mode;
};

class mpfr_class {
public:
    mpfr_class() : mpfr_class(precision_tag{}, default_precision()) {}

    mpfr_class(const mpfr_class& other)
    {
        const mpfr_prec_t precision = other.precision();
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set(value_, other.value_, context.rounding_mode);
        init_guard.release();
    }

    mpfr_class(mpfr_class&& other) noexcept
    {
        mpfr_init2(value_, other.precision());
        mpfr_swap(value_, other.value_);
    }

    mpfr_class(double value) : mpfr_class(value, default_precision()) {}

    mpfr_class(double value, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set_d(value_, value, context.rounding_mode);
        init_guard.release();
    }

    mpfr_class(const gmpxx::mpz_class& value) : mpfr_class(value, default_precision()) {}

    mpfr_class(const gmpxx::mpq_class& value) : mpfr_class(value, default_precision()) {}

    mpfr_class(const gmpxx::mpz_class& value, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set_z(value_, value.mpz_data(), context.rounding_mode);
        init_guard.release();
    }

    mpfr_class(const gmpxx::mpq_class& value, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set_q(value_, value.mpq_data(), context.rounding_mode);
        init_guard.release();
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    mpfr_class(T value) : mpfr_class(value, default_precision())
    {
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    mpfr_class(T value, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        set_integral(value);
        init_guard.release();
    }

    mpfr_class(bool) = delete;
    mpfr_class(bool, mpfr_prec_t) = delete;

    explicit mpfr_class(const char* text) : mpfr_class(text, default_precision(), 0) {}

    explicit mpfr_class(const std::string& text) : mpfr_class(text.c_str(), default_precision(), 0) {}

    mpfr_class(const char* text, mpfr_prec_t precision, int base = 0)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const std::string parse_text = gmpfrxx_mkII::detail::mpfr_trim_surrounding_whitespace(text);
        if (text == nullptr || mpfr_set_str(value_, parse_text.c_str(), base, context.rounding_mode) != 0) {
            throw std::invalid_argument("invalid mpfr_class string");
        }
        init_guard.release();
    }

    mpfr_class(const std::string& text, mpfr_prec_t precision, int base = 0)
        : mpfr_class(text.c_str(), precision, base)
    {
    }

    explicit mpfr_class(mpfr_srcptr value)
    {
        const mpfr_prec_t precision = mpfr_get_prec(value);
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set(value_, value, context.rounding_mode);
        init_guard.release();
    }

    mpfr_class(mpfr_srcptr value, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set(value_, value, context.rounding_mode);
        init_guard.release();
    }

    mpfr_class(const mpfr_class& other, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set(value_, other.value_, context.rounding_mode);
        init_guard.release();
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    (std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class> ||
                                     std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpz_class> ||
                                     std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpq_class>)>>
    mpfr_class(const Expr& expr);

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    (std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class> ||
                                     std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpz_class> ||
                                     std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpq_class>)>,
        typename = void>
    mpfr_class(const Expr& expr, mpfr_prec_t precision);

    ~mpfr_class()
    {
        mpfr_clear(value_);
    }

    mpfr_class& operator=(const mpfr_class& other)
    {
        if (this != &other) {
            const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
            mpfr_set(value_, other.value_, context.rounding_mode);
        }
        return *this;
    }

    mpfr_class& operator=(mpfr_class&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if constexpr (gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath) {
            assert((this->precision() == other.precision()) &&
                   "mpfr_class move assignment precision mismatch under "
                   "GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH");
            mpfr_swap(value_, other.value_);
        } else {
            if (this->precision() == other.precision()) {
                mpfr_swap(value_, other.value_);
            } else {
                const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
                mpfr_set(value_, other.value_, context.rounding_mode);
            }
        }
        return *this;
    }

    mpfr_class& operator=(double value)
    {
        set(value);
        return *this;
    }

    mpfr_class& operator=(const gmpxx::mpz_class& value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        mpfr_set_z(value_, value.mpz_data(), context.rounding_mode);
        return *this;
    }

    mpfr_class& operator=(const gmpxx::mpq_class& value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        mpfr_set_q(value_, value.mpq_data(), context.rounding_mode);
        return *this;
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    mpfr_class& operator=(T value)
    {
        set_integral(value);
        return *this;
    }

    mpfr_class& operator=(bool) = delete;

    mpfr_class& operator=(const char* text)
    {
        if (set_str(text, 0) != 0) {
            throw std::invalid_argument("invalid mpfr_class string");
        }
        return *this;
    }

    mpfr_class& operator=(const std::string& text)
    {
        return *this = text.c_str();
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    (std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class> ||
                                     std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpz_class> ||
                                     std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpq_class>)>>
    mpfr_class& operator=(const Expr& expr);

    static mpfr_class with_precision(mpfr_prec_t precision, double value)
    {
        mpfr_class result = with_precision(precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set_d(result.value_, value, context.rounding_mode);
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

    mpfr_prec_t get_prec() const noexcept
    {
        return precision();
    }

    double to_double() const
    {
        return mpfr_get_d(value_, default_rounding());
    }

    double get_d() const
    {
        return to_double();
    }

    signed long get_si() const
    {
        return mpfr_get_si(value_, default_rounding());
    }

    unsigned long get_ui() const
    {
        return mpfr_get_ui(value_, default_rounding());
    }

    std::uint64_t get_u64() const
    {
        gmpfrxx_mkII::detail::scoped_mpz_t integer;
        mpfr_get_z(integer.get(), value_, default_rounding());
        return gmpfrxx_mkII::detail::mpz_get_uint64_checked(integer.get());
    }

    std::int64_t get_i64() const
    {
        gmpfrxx_mkII::detail::scoped_mpz_t integer;
        mpfr_get_z(integer.get(), value_, default_rounding());
        return gmpfrxx_mkII::detail::mpz_get_int64_checked(integer.get());
    }

    bool fits_sint_p() const
    {
        return mpfr_fits_sint_p(value_, default_rounding()) != 0;
    }

    bool fits_slong_p() const
    {
        return mpfr_fits_slong_p(value_, default_rounding()) != 0;
    }

    bool fits_sshort_p() const
    {
        return mpfr_fits_sshort_p(value_, default_rounding()) != 0;
    }

    bool fits_uint_p() const
    {
        return mpfr_fits_uint_p(value_, default_rounding()) != 0;
    }

    bool fits_ulong_p() const
    {
        return mpfr_fits_ulong_p(value_, default_rounding()) != 0;
    }

    bool fits_ushort_p() const
    {
        return mpfr_fits_ushort_p(value_, default_rounding()) != 0;
    }

    void set_prec(mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_prec_round(value_, precision, context.rounding_mode);
    }

    explicit operator bool() const noexcept
    {
        return mpfr_zero_p(value_) == 0;
    }

    explicit operator gmpxx::mpz_class() const
    {
        gmpxx::mpz_class result;
        mpfr_get_z(result.mpz_data(), value_, default_rounding());
        return result;
    }

    explicit operator gmpxx::mpq_class() const
    {
        if (mpfr_nan_p(value_) != 0 || mpfr_inf_p(value_) != 0) {
            throw std::domain_error("cannot convert NaN or Inf to mpq_class");
        }
        gmpxx::mpq_class result;
        mpfr_get_q(result.mpq_data(), value_);
        result.canonicalize();
        return result;
    }

    void set(double value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        mpfr_set_d(value_, value, context.rounding_mode);
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    void set(T value)
    {
        set_integral(value);
    }

    void set(const char* text)
    {
        if (set_str(text, 0) != 0) {
            throw std::invalid_argument("invalid mpfr_class string");
        }
    }

    void set(const std::string& text)
    {
        set(text.c_str());
    }

    int set_str(const char* text, int base = 10)
    {
        if (text == nullptr) {
            return -1;
        }

        mpfr_t temp;
        mpfr_init2(temp, precision());
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const std::string parse_text = gmpfrxx_mkII::detail::mpfr_trim_surrounding_whitespace(text);
        const int rc = mpfr_set_str(temp, parse_text.c_str(), base, context.rounding_mode);
        if (rc == 0) {
            mpfr_set(value_, temp, context.rounding_mode);
        }
        mpfr_clear(temp);
        return rc;
    }

    int set_str(const std::string& text, int base = 10)
    {
        return set_str(text.c_str(), base);
    }

    std::string get_str() const
    {
        return to_string();
    }

    std::string get_str(mpfr_exp_t& exponent, int base = 10, std::size_t n_digits = 0) const
    {
        char* raw = mpfr_get_str(nullptr, &exponent, base, n_digits, value_, default_rounding());
        if (raw == nullptr) {
            throw std::runtime_error("mpfr_get_str failed");
        }
        std::string result(raw);
        mpfr_free_str(raw);
        return result;
    }

    std::string to_string(std::size_t n_digits = 0) const
    {
        if (mpfr_nan_p(value_)) {
            return "nan";
        }
        if (mpfr_inf_p(value_)) {
            return mpfr_sgn(value_) < 0 ? "-inf" : "inf";
        }

        mpfr_exp_t exponent = 0;
        std::string digits = get_str(exponent, 10, n_digits);

        bool negative = false;
        if (!digits.empty() && digits.front() == '-') {
            negative = true;
            digits.erase(digits.begin());
        }
        if (n_digits == 0) {
            while (digits.size() > 1 && digits.back() == '0') {
                digits.pop_back();
            }
        }

        if (digits == "0") {
            return negative ? "-0" : "0";
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

    const mpfr_t& mpfr_data() const noexcept
    {
        return value_;
    }

    mpfr_t& mpfr_data() noexcept
    {
        return value_;
    }

    const mpfr_t& get_mpfr_t() const noexcept
    {
        return value_;
    }

    mpfr_t& get_mpfr_t() noexcept
    {
        return value_;
    }

    void swap(mpfr_class& other) noexcept
    {
        mpfr_swap(value_, other.value_);
    }

    static mpfr_prec_t default_precision() noexcept
    {
        return default_precision_bits();
    }

    static mpfr_rnd_t default_rounding() noexcept
    {
        return default_rounding_mode();
    }

private:
    struct precision_tag {};

    mpfr_class(precision_tag, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::scoped_mpfr_init init_guard(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        mpfr_set_ui(value_, 0, context.rounding_mode);
        init_guard.release();
    }

    template <typename T>
    void set_integral(T value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        using value_type = std::remove_cv_t<T>;
        if constexpr (std::is_signed_v<value_type> &&
                      std::numeric_limits<value_type>::digits <= std::numeric_limits<long>::digits) {
            mpfr_set_si(value_, static_cast<long>(value), context.rounding_mode);
        } else if constexpr (std::is_unsigned_v<value_type> &&
                             std::numeric_limits<value_type>::digits <= std::numeric_limits<unsigned long>::digits) {
            mpfr_set_ui(value_, static_cast<unsigned long>(value), context.rounding_mode);
        } else {
            const gmpxx::mpz_class integer(value);
            mpfr_set_z(value_, integer.mpz_data(), context.rounding_mode);
        }
    }

    mpfr_t value_;
};

inline void swap(mpfr_class& lhs, mpfr_class& rhs) noexcept
{
    lhs.swap(rhs);
}

class mpfr_context_ref;

} // namespace mpfrxx

namespace std {

template <>
struct common_type<gmpxx::mpz_class, mpfrxx::mpfr_class> {
    using type = mpfrxx::mpfr_class;
};

template <>
struct common_type<mpfrxx::mpfr_class, gmpxx::mpz_class> {
    using type = mpfrxx::mpfr_class;
};

template <>
struct common_type<gmpxx::mpq_class, mpfrxx::mpfr_class> {
    using type = mpfrxx::mpfr_class;
};

template <>
struct common_type<mpfrxx::mpfr_class, gmpxx::mpq_class> {
    using type = mpfrxx::mpfr_class;
};

template <>
struct common_type<mpfrxx::mpfr_class, mpfrxx::mpfr_class> {
    using type = mpfrxx::mpfr_class;
};

GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(mpfrxx::mpfr_class);

} // namespace std

namespace gmpfrxx_mkII {
namespace detail {

class mpfr_allocated_string {
public:
    explicit mpfr_allocated_string(char* value) noexcept : value_(value) {}

    ~mpfr_allocated_string()
    {
        reset();
    }

    mpfr_allocated_string(const mpfr_allocated_string&) = delete;
    mpfr_allocated_string& operator=(const mpfr_allocated_string&) = delete;

    mpfr_allocated_string(mpfr_allocated_string&& other) noexcept : value_(other.value_)
    {
        other.value_ = nullptr;
    }

    mpfr_allocated_string& operator=(mpfr_allocated_string&& other) noexcept
    {
        if (this != &other) {
            reset();
            value_ = other.value_;
            other.value_ = nullptr;
        }
        return *this;
    }

    const char* c_str() const noexcept
    {
        return value_;
    }

    explicit operator bool() const noexcept
    {
        return value_ != nullptr;
    }

private:
    void reset() noexcept
    {
        if (value_ != nullptr) {
            mpfr_free_str(value_);
            value_ = nullptr;
        }
    }

    char* value_;
};

inline int mpfr_stream_input_base(const std::ios_base& stream) noexcept
{
    const auto basefield = stream.flags() & std::ios_base::basefield;
    if (basefield == std::ios_base::hex) {
        return 16;
    }
    if (basefield == std::ios_base::oct) {
        return 8;
    }
    return 10;
}

inline int mpfr_digit_value(char c) noexcept
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    return -1;
}

inline bool mpfr_is_digit_for_base(char c, int base) noexcept
{
    const int value = mpfr_digit_value(c);
    return value >= 0 && value < base;
}

inline bool mpfr_stream_peek_char(std::istream& in, char& c)
{
    const auto next = in.rdbuf()->sgetc();
    if (next == std::char_traits<char>::eof()) {
        in.setstate(std::ios_base::eofbit);
        return false;
    }
    c = static_cast<char>(next);
    return true;
}

inline void mpfr_stream_get_char(std::istream& in, std::string& token)
{
    const auto next = in.rdbuf()->sbumpc();
    if (next == std::char_traits<char>::eof()) {
        in.setstate(std::ios_base::eofbit | std::ios_base::failbit);
        return;
    }
    token.push_back(static_cast<char>(next));
}

inline void mpfr_read_optional_sign(std::istream& in, std::string& token)
{
    char c = '\0';
    if (mpfr_stream_peek_char(in, c) && (c == '+' || c == '-')) {
        mpfr_stream_get_char(in, token);
    }
}

inline bool mpfr_read_digits_for_base(std::istream& in, std::string& token, int base)
{
    bool saw_digit = false;
    char c = '\0';
    while (mpfr_stream_peek_char(in, c) && mpfr_is_digit_for_base(c, base)) {
        mpfr_stream_get_char(in, token);
        saw_digit = true;
    }
    return saw_digit;
}

inline char mpfr_stream_decimal_point(const std::ios_base& stream)
{
    return std::use_facet<std::numpunct<char>>(stream.getloc()).decimal_point();
}

inline bool mpfr_read_float_token(std::istream& in, std::string& token, int base)
{
    token.clear();
    mpfr_read_optional_sign(in, token);
    const char decimal_point = mpfr_stream_decimal_point(in);

    bool saw_mantissa_digit = false;
    saw_mantissa_digit |= mpfr_read_digits_for_base(in, token, base);

    char c = '\0';
    if (mpfr_stream_peek_char(in, c) && c == decimal_point) {
        in.rdbuf()->sbumpc();
        token.push_back('.');
        saw_mantissa_digit |= mpfr_read_digits_for_base(in, token, base);
    }

    if (!saw_mantissa_digit) {
        return false;
    }

    const char exponent_marker = base == 10 ? 'e' : '@';
    if (!mpfr_stream_peek_char(in, c) ||
        (c != exponent_marker && !(base == 10 && c == 'E'))) {
        return true;
    }

    mpfr_stream_get_char(in, token);
    mpfr_read_optional_sign(in, token);

    bool saw_exponent_digit = false;
    while (mpfr_stream_peek_char(in, c) && std::isdigit(static_cast<unsigned char>(c))) {
        mpfr_stream_get_char(in, token);
        saw_exponent_digit = true;
    }
    return saw_exponent_digit;
}

inline std::string mpfr_strip_leading_plus(std::string token)
{
    if (!token.empty() && token.front() == '+') {
        token.erase(token.begin());
    }
    return token;
}

inline std::string mpfr_get_str_abs(mpfr_srcptr value, mpfr_exp_t& exponent, int base, std::size_t digits)
{
    mpfr_t magnitude;
    mpfr_init2(magnitude, mpfr_get_prec(value));
    mpfr_abs(magnitude, value, ::mpfrxx::mpfr_class::default_rounding());
    gmpfrxx_mkII::detail::mpfr_allocated_string raw(
        mpfr_get_str(nullptr, &exponent, base, digits, magnitude, ::mpfrxx::mpfr_class::default_rounding()));
    mpfr_clear(magnitude);
    if (!raw) {
        throw std::bad_alloc();
    }
    return std::string(raw.c_str());
}

inline void mpfr_uppercase_if_requested(std::string& text, std::ios_base::fmtflags flags)
{
    if (!(flags & std::ios_base::uppercase)) {
        return;
    }
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
}

inline void add_mpfr_base_prefix(std::string& text, mpfr_srcptr value, int base, std::ios_base::fmtflags flags)
{
    if (!(flags & std::ios_base::showbase)) {
        return;
    }
    if (base == 16) {
        text.insert(0, "0x");
    } else if (base == 8 && mpfr_sgn(value) != 0) {
        text.insert(0, "0");
    }
}

inline bool mpfr_formatted_has_nonzero_digit(const std::string& text)
{
    return std::any_of(text.begin(), text.end(), [](unsigned char c) {
        return std::isdigit(c) && c != '0';
    });
}

inline void add_mpfr_sign(std::string& text, mpfr_srcptr value)
{
    if (mpfr_sgn(value) < 0) {
        text.insert(0, "-");
    }
}

inline mpfr_exp_t mpfr_integral_digits_in_base(mpfr_srcptr value, int base)
{
    if (mpfr_sgn(value) == 0) {
        return 1;
    }

    mpfr_exp_t exponent = 0;
    mpfr_get_d_2exp(&exponent, value, ::mpfrxx::mpfr_class::default_rounding());
    if (exponent <= 0) {
        return 1;
    }

    switch (base) {
    case 16:
        return static_cast<mpfr_exp_t>((exponent + 3) / 4);
    case 8:
        return static_cast<mpfr_exp_t>((exponent + 2) / 3);
    case 10:
        return static_cast<mpfr_exp_t>(std::floor(exponent / std::log2(10.0)) + 1);
    case 2:
        return exponent;
    default:
        return 0;
    }
}

inline std::size_t effective_mpfr_base_precision(std::streamsize precision)
{
    return static_cast<std::size_t>(precision == 0 ? 6 : precision);
}

inline std::string mpfr_to_base_string_default(
    mpfr_srcptr value,
    int base,
    std::ios_base::fmtflags flags,
    std::streamsize precision)
{
    if (mpfr_sgn(value) == 0) {
        std::string text = (flags & std::ios_base::showpoint) ? "0." : "0";
        add_mpfr_base_prefix(text, value, base, flags);
        mpfr_uppercase_if_requested(text, flags);
        return text;
    }

    const std::size_t effective_precision = effective_mpfr_base_precision(precision);
    mpfr_exp_t exponent = 0;
    std::string digits = mpfr_get_str_abs(value, exponent, base, effective_precision);
    if (digits.empty()) {
        digits = "0";
    }

    std::string text;
    if (exponent <= 0) {
        text = "0.";
        text.append(static_cast<std::size_t>(-exponent), '0');
        text += digits;
    } else if (static_cast<std::size_t>(exponent) > digits.size()) {
        text = digits.substr(0, 1);
        if (digits.size() > 1 || (flags & std::ios_base::showpoint)) {
            text += ".";
            if (digits.size() > 1) {
                text += digits.substr(1);
            }
        }
        const mpfr_exp_t adjusted_exponent = exponent - 1;
        text += (base == 16) ? "@" : "e";
        text += adjusted_exponent >= 0 ? "+" : "-";
        const mpfr_exp_t absolute_exponent = adjusted_exponent >= 0 ? adjusted_exponent : -adjusted_exponent;
        if (absolute_exponent < 10) {
            text += "0";
        }
        text += std::to_string(absolute_exponent);
    } else {
        text = digits.substr(0, static_cast<std::size_t>(exponent));
        if (exponent < static_cast<mpfr_exp_t>(digits.size())) {
            text += ".";
            text += digits.substr(static_cast<std::size_t>(exponent));
        }
    }

    if ((flags & std::ios_base::showpoint) && text.find('.') == std::string::npos) {
        text += ".";
    }
    if (!(flags & std::ios_base::showpoint)) {
        const std::size_t point = text.find('.');
        if (point != std::string::npos) {
            while (!text.empty() && text.back() == '0') {
                text.pop_back();
            }
        }
    }
    if (!(flags & std::ios_base::showpoint) && !text.empty() && text.back() == '.') {
        text.pop_back();
    }
    if (base != 8 || !(flags & std::ios_base::showbase) || mpfr_formatted_has_nonzero_digit(text)) {
        add_mpfr_base_prefix(text, value, base, flags);
    }
    add_mpfr_sign(text, value);
    mpfr_uppercase_if_requested(text, flags);
    return text;
}

inline std::string mpfr_to_base_string_fixed(
    mpfr_srcptr value,
    int base,
    std::ios_base::fmtflags flags,
    std::streamsize precision)
{
    const std::size_t effective_precision = effective_mpfr_base_precision(precision);
    mpfr_exp_t exponent = 0;
    const std::size_t digits_wanted =
        static_cast<std::size_t>(mpfr_integral_digits_in_base(value, base)) + effective_precision + 1;
    std::string digits = mpfr_get_str_abs(value, exponent, base, digits_wanted);
    if (digits.empty()) {
        digits = "0";
    }

    std::string text;
    if (exponent <= 0) {
        text = "0";
        if (effective_precision > 0 || (flags & std::ios_base::showpoint)) {
            text += ".";
            std::string fraction(static_cast<std::size_t>(-exponent), '0');
            fraction += digits;
            fraction.resize(effective_precision, '0');
            text += fraction;
        }
    } else {
        if (static_cast<std::size_t>(exponent) >= digits.size()) {
            text = digits;
            text.append(static_cast<std::size_t>(exponent) - digits.size(), '0');
        } else {
            text = digits.substr(0, static_cast<std::size_t>(exponent));
        }
        if (effective_precision > 0 || (flags & std::ios_base::showpoint)) {
            text += ".";
            std::string fraction;
            if (static_cast<std::size_t>(exponent) < digits.size()) {
                fraction = digits.substr(static_cast<std::size_t>(exponent));
            }
            fraction.resize(effective_precision, '0');
            text += fraction;
        }
    }

    if (base != 8 || !(flags & std::ios_base::showbase) || mpfr_formatted_has_nonzero_digit(text)) {
        add_mpfr_base_prefix(text, value, base, flags);
    }
    add_mpfr_sign(text, value);
    mpfr_uppercase_if_requested(text, flags);
    return text;
}

inline std::string mpfr_to_base_string_scientific(
    mpfr_srcptr value,
    int base,
    std::ios_base::fmtflags flags,
    std::streamsize precision)
{
    const std::size_t effective_precision = effective_mpfr_base_precision(precision);
    mpfr_exp_t exponent = 0;
    std::string digits = mpfr_get_str_abs(value, exponent, base, effective_precision + 1);
    if (digits.empty()) {
        digits = "0";
    }

    std::string text;
    text += digits[0];
    text += ".";
    if (digits.size() > 1) {
        text += digits.substr(1, effective_precision);
    }
    if (text.size() < effective_precision + 2) {
        text.append(effective_precision + 2 - text.size(), '0');
    }

    const mpfr_exp_t adjusted_exponent = mpfr_sgn(value) == 0 ? 0 : exponent - 1;
    text += (base == 16) ? "@" : "e";
    text += adjusted_exponent >= 0 ? "+" : "-";
    const mpfr_exp_t absolute_exponent = adjusted_exponent >= 0 ? adjusted_exponent : -adjusted_exponent;
    if (absolute_exponent < 10) {
        text += "0";
    }
    text += std::to_string(absolute_exponent);

    add_mpfr_base_prefix(text, value, base, flags);
    add_mpfr_sign(text, value);
    mpfr_uppercase_if_requested(text, flags);
    return text;
}

inline std::string mpfr_stream_text(mpfr_srcptr value, const std::ios_base& out)
{
    const auto flags = out.flags();
    const auto basefield = flags & std::ios_base::basefield;
    if (basefield == std::ios_base::hex || basefield == std::ios_base::oct) {
        const int base = basefield == std::ios_base::hex ? 16 : 8;
        const auto floatfield = flags & std::ios_base::floatfield;
        if (floatfield == std::ios_base::fixed) {
            return mpfr_to_base_string_fixed(value, base, flags, out.precision());
        }
        if (floatfield == std::ios_base::scientific) {
            return mpfr_to_base_string_scientific(value, base, flags, out.precision());
        }
        return mpfr_to_base_string_default(value, base, flags, out.precision());
    }

    char conversion = 'g';
    const auto floatfield = flags & std::ios_base::floatfield;
    if (floatfield == std::ios_base::fixed) {
        conversion = 'f';
    } else if (floatfield == std::ios_base::scientific) {
        conversion = 'e';
    }
    if (flags & std::ios_base::uppercase) {
        conversion = static_cast<char>(conversion - 'a' + 'A');
    }

    std::string format = (flags & std::ios_base::showpoint) ? "%#.*R" : "%.*R";
    format.push_back(conversion);

    std::streamsize stream_precision = out.precision();
    if (stream_precision < 0) {
        stream_precision = 6;
    }
    if (stream_precision == 0 && (flags & std::ios_base::floatfield) != std::ios_base::fixed) {
        stream_precision = 6;
    }
    if (stream_precision > static_cast<std::streamsize>(std::numeric_limits<int>::max())) {
        stream_precision = std::numeric_limits<int>::max();
    }

    char* raw = nullptr;
    const int count = mpfr_asprintf(
        &raw, format.c_str(), static_cast<int>(stream_precision), value);
    gmpfrxx_mkII::detail::mpfr_allocated_string formatted(raw);
    if (count < 0 || !formatted) {
        return {};
    }
    return std::string(formatted.c_str());
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

inline std::ostream& operator<<(std::ostream& out, const mpfr_class& value)
{
    try {
        std::string text = gmpfrxx_mkII::detail::mpfr_stream_text(value.mpfr_data(), out);
        if (text.empty()) {
            out.setstate(std::ios_base::badbit);
            return out;
        }

        const char decimal_point = gmpfrxx_mkII::detail::mpfr_stream_decimal_point(out);
        if (decimal_point != '.') {
            const std::size_t point = text.find('.');
            if (point != std::string::npos) {
                text[point] = decimal_point;
            }
        }
        if ((out.flags() & std::ios_base::showpos) && mpfr_sgn(value.mpfr_data()) >= 0) {
            text.insert(0, "+");
        }

        const std::streamsize width = out.width();
        if (static_cast<std::streamsize>(text.size()) < width) {
            const auto fill_count = static_cast<std::size_t>(width - static_cast<std::streamsize>(text.size()));
            if (out.flags() & std::ios_base::left) {
                text.append(fill_count, out.fill());
            } else if (out.flags() & std::ios_base::internal) {
                std::size_t pos = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1 : 0;
                if (text.compare(pos, 2, "0x") == 0 || text.compare(pos, 2, "0X") == 0) {
                    pos += 2;
                } else if (text.compare(pos, 1, "0") == 0 &&
                           (out.flags() & std::ios_base::basefield) == std::ios_base::oct &&
                           (out.flags() & std::ios_base::showbase)) {
                    pos += 1;
                }
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
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << mpfr_class(expr);
}

inline std::istream& operator>>(std::istream& in, mpfr_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    const int base = gmpfrxx_mkII::detail::mpfr_stream_input_base(in);
    std::string token;
    const bool parsed_token = gmpfrxx_mkII::detail::mpfr_read_float_token(in, token, base);
    const std::string parse_token = gmpfrxx_mkII::detail::mpfr_strip_leading_plus(std::move(token));

    mpfr_class tmp = mpfr_class::with_precision(value.precision());
    if (parsed_token && tmp.set_str(parse_token, base) == 0) {
        value.swap(tmp);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

class gmp_randclass;

enum class random_mpfr_distribution {
    urandomb,
    uniform,
    normal,
    exponential
};

class random_mpfr_expr {
public:
    using result_type = mpfr_class;

    random_mpfr_expr(
        std::shared_ptr<gmpfrxx_mkII::detail::gmp_randstate_holder> state,
        random_mpfr_distribution distribution,
        mpfr_prec_t requested_precision,
        bool has_requested_precision) noexcept
        : state_(std::move(state)),
          distribution_(distribution),
          requested_precision_(requested_precision),
          has_requested_precision_(has_requested_precision)
    {
    }

    mpfr_prec_t materialization_precision() const noexcept
    {
        return has_requested_precision_ ? requested_precision_ : mpfr_class::default_precision();
    }

    void generate(mpfr_t dest, mpfr_rnd_t rnd) const;

private:
    std::shared_ptr<gmpfrxx_mkII::detail::gmp_randstate_holder> state_;
    random_mpfr_distribution distribution_;
    mpfr_prec_t requested_precision_;
    bool has_requested_precision_;
};

class gmp_randclass {
public:
    gmp_randclass()
        : state_(std::make_shared<gmpfrxx_mkII::detail::gmp_randstate_holder>())
    {
        gmp_randinit_default(state_->state);
        state_->mark_initialized();
    }

    explicit gmp_randclass(void (*randinit)(gmp_randstate_t))
        : state_(std::make_shared<gmpfrxx_mkII::detail::gmp_randstate_holder>())
    {
        randinit(state_->state);
        state_->mark_initialized();
    }

    gmp_randclass(int (*randinit)(gmp_randstate_t, mp_bitcnt_t), mp_bitcnt_t size)
        : state_(std::make_shared<gmpfrxx_mkII::detail::gmp_randstate_holder>())
    {
        if (randinit(state_->state, size) == 0) {
            throw std::length_error("gmp_randinit_lc_2exp_size failed");
        }
        state_->mark_initialized();
    }

    gmp_randclass(
        void (*randinit)(gmp_randstate_t, const mpz_t, unsigned long, mp_bitcnt_t),
        const gmpxx::mpz_class& a,
        unsigned long c,
        mp_bitcnt_t m2exp)
        : state_(std::make_shared<gmpfrxx_mkII::detail::gmp_randstate_holder>())
    {
        randinit(state_->state, a.mpz_data(), c, m2exp);
        state_->mark_initialized();
    }

    gmp_randclass(gmp_randalg_t alg, mp_bitcnt_t size)
        : state_(std::make_shared<gmpfrxx_mkII::detail::gmp_randstate_holder>())
    {
        if (alg == GMP_RAND_ALG_DEFAULT || alg == GMP_RAND_ALG_LC ||
            alg == static_cast<gmp_randalg_t>(0)) {
            if (gmp_randinit_lc_2exp_size(state_->state, size) == 0) {
                throw std::length_error("gmp_randinit_lc_2exp_size failed");
            }
            state_->mark_initialized();
        } else {
            throw std::invalid_argument("unsupported GMP random algorithm");
        }
    }

    ~gmp_randclass() = default;

    gmp_randclass(const gmp_randclass&) = delete;
    gmp_randclass& operator=(const gmp_randclass&) = delete;
    gmp_randclass(gmp_randclass&&) = delete;
    gmp_randclass& operator=(gmp_randclass&&) = delete;

    void seed(unsigned long value)
    {
        gmp_randseed_ui(state_->state, value);
    }

    void seed(const gmpxx::mpz_class& value)
    {
        gmp_randseed(state_->state, value.mpz_data());
    }

    gmpxx::mpz_class get_z_bits(mp_bitcnt_t bits)
    {
        gmpxx::mpz_class result;
        mpz_urandomb(result.mpz_data(), state_->state, bits);
        return result;
    }

    gmpxx::mpz_class get_z_bits(const gmpxx::mpz_class& bits)
    {
        if (mpz_sgn(bits.mpz_data()) < 0) {
            throw std::invalid_argument("random bit count must be non-negative");
        }
        if (mpz_fits_ulong_p(bits.mpz_data()) == 0) {
            throw std::length_error("random bit count does not fit mp_bitcnt_t");
        }
        return get_z_bits(static_cast<mp_bitcnt_t>(mpz_get_ui(bits.mpz_data())));
    }

    gmpxx::mpz_class get_z_range(const gmpxx::mpz_class& limit)
    {
        if (mpz_sgn(limit.mpz_data()) <= 0) {
            throw std::invalid_argument("random range limit must be positive");
        }
        gmpxx::mpz_class result;
        mpz_urandomm(result.mpz_data(), state_->state, limit.mpz_data());
        return result;
    }

    random_mpfr_expr get_fr() noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::urandomb, 0, false);
    }

    random_mpfr_expr get_fr(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::urandomb, precision, true);
    }

    random_mpfr_expr get_fr(const mpfr_class& prototype) noexcept
    {
        return get_fr(prototype.precision());
    }

    random_mpfr_expr get_fr_urandomb() noexcept
    {
        return get_fr();
    }

    random_mpfr_expr get_fr_urandomb(mpfr_prec_t precision) noexcept
    {
        return get_fr(precision);
    }

    random_mpfr_expr get_fr_uniform() noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::uniform, 0, false);
    }

    random_mpfr_expr get_fr_uniform(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::uniform, precision, true);
    }

    random_mpfr_expr get_fr_urandom() noexcept
    {
        return get_fr_uniform();
    }

    random_mpfr_expr get_fr_urandom(mpfr_prec_t precision) noexcept
    {
        return get_fr_uniform(precision);
    }

    random_mpfr_expr get_fr_normal() noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::normal, 0, false);
    }

    random_mpfr_expr get_fr_normal(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::normal, precision, true);
    }

    random_mpfr_expr get_fr_nrandom() noexcept
    {
        return get_fr_normal();
    }

    random_mpfr_expr get_fr_nrandom(mpfr_prec_t precision) noexcept
    {
        return get_fr_normal(precision);
    }

    random_mpfr_expr get_fr_exponential() noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::exponential, 0, false);
    }

    random_mpfr_expr get_fr_exponential(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(state_, random_mpfr_distribution::exponential, precision, true);
    }

    random_mpfr_expr get_fr_erandom() noexcept
    {
        return get_fr_exponential();
    }

    random_mpfr_expr get_fr_erandom(mpfr_prec_t precision) noexcept
    {
        return get_fr_exponential(precision);
    }

private:
    std::shared_ptr<gmpfrxx_mkII::detail::gmp_randstate_holder> state_;
};

inline void random_mpfr_expr::generate(mpfr_t dest, mpfr_rnd_t rnd) const
{
    switch (distribution_) {
    case random_mpfr_distribution::urandomb:
        if (mpfr_urandomb(dest, state_->state) != 0) {
            throw std::runtime_error("mpfr_urandomb failed");
        }
        break;
    case random_mpfr_distribution::uniform:
        mpfr_urandom(dest, state_->state, rnd);
        break;
    case random_mpfr_distribution::normal:
        mpfr_nrandom(dest, state_->state, rnd);
        break;
    case random_mpfr_distribution::exponential:
        mpfr_erandom(dest, state_->state, rnd);
        break;
    }
}

} // namespace mpfrxx

inline std::ostream& operator<<(std::ostream& out, mpfr_srcptr value)
{
    return out << mpfrxx::mpfr_class(value);
}

inline std::istream& operator>>(std::istream& in, mpfr_ptr value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    const int base = gmpfrxx_mkII::detail::mpfr_stream_input_base(in);
    std::string token;
    const bool parsed_token = gmpfrxx_mkII::detail::mpfr_read_float_token(in, token, base);
    const std::string parse_token = gmpfrxx_mkII::detail::mpfr_strip_leading_plus(std::move(token));

    mpfrxx::mpfr_class tmp = mpfrxx::mpfr_class::with_precision(mpfr_get_prec(value));
    if (parsed_token && tmp.set_str(parse_token, base) == 0) {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(mpfr_get_prec(value));
        mpfr_set(value, tmp.mpfr_data(), context.rounding_mode);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

namespace gmpfrxx_mkII {
namespace detail {

template <>
struct is_expression_node<mpfrxx::random_mpfr_expr> : std::true_type {};

template <typename T, typename = void>
struct is_mpfr_expression_operand : std::false_type {};

template <typename T>
struct is_supported_mpfr_scalar
    : std::bool_constant<is_supported_expression_integral_v<T> ||
                         std::is_same_v<std::remove_cv_t<T>, float> ||
                         std::is_same_v<std::remove_cv_t<T>, double>> {};

template <typename T>
inline constexpr bool is_supported_mpfr_scalar_v =
    is_supported_mpfr_scalar<std::remove_cv_t<T>>::value;

template <typename T, typename = void>
struct normalized_mpfr_scalar;

template <typename T>
struct normalized_mpfr_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                  std::is_signed_v<std::remove_cv_t<T>>>> {
    using type = std::int64_t;
};

template <typename T>
struct normalized_mpfr_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                  std::is_unsigned_v<std::remove_cv_t<T>>>> {
    using type = std::uint64_t;
};

template <>
struct normalized_mpfr_scalar<float> {
    using type = double;
};

template <>
struct normalized_mpfr_scalar<double> {
    using type = double;
};

template <typename T>
using normalized_mpfr_scalar_t = typename normalized_mpfr_scalar<std::remove_cv_t<T>>::type;

template <>
struct is_mpfr_expression_operand<mpfrxx::mpfr_class> : std::true_type {};

template <>
struct is_mpfr_expression_operand<gmpxx::mpz_class> : std::true_type {};

template <>
struct is_mpfr_expression_operand<gmpxx::mpq_class> : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, mpfrxx::mpfr_class>>>
    : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> &&
                     (std::is_same_v<typename T::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename T::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<T, std::enable_if_t<is_supported_mpfr_scalar_v<T>>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_expression_operand_v =
    is_mpfr_expression_operand<std::decay_t<T>>::value;

template <typename T, typename = void>
struct is_mpfr_object_or_node : std::false_type {};

template <typename T>
struct is_mpfr_object_or_node<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, mpfrxx::mpfr_class>>> : std::true_type {};

template <typename T>
struct is_mpfr_object_or_node<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     std::is_same_v<typename std::decay_t<T>::result_type, mpfrxx::mpfr_class>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_object_or_node_v = is_mpfr_object_or_node<T>::value;

inline borrowed_object_leaf<mpfrxx::mpfr_class> make_mpfr_operand(const mpfrxx::mpfr_class& value)
{
    return borrowed_object_leaf<mpfrxx::mpfr_class>(value);
}

inline object_leaf<mpfrxx::mpfr_class> make_mpfr_operand(mpfrxx::mpfr_class&& value)
{
    return object_leaf<mpfrxx::mpfr_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpz_class> make_mpfr_operand(const gmpxx::mpz_class& value)
{
    return borrowed_object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpfr_operand(gmpxx::mpz_class&& value)
{
    return object_leaf<gmpxx::mpz_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpq_class> make_mpfr_operand(const gmpxx::mpq_class& value)
{
    return borrowed_object_leaf<gmpxx::mpq_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpfr_operand(gmpxx::mpq_class&& value)
{
    return object_leaf<gmpxx::mpq_class>(std::move(value));
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_mpfr_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

template <typename Scalar, typename = std::enable_if_t<is_supported_mpfr_scalar_v<Scalar>>>
auto make_mpfr_operand(Scalar value)
{
    using storage_type = normalized_mpfr_scalar_t<Scalar>;
    return scalar_leaf<storage_type, mpfrxx::mpfr_class>(static_cast<storage_type>(value));
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return expr.get().precision();
}

inline mpfr_prec_t mpfr_expression_precision(const borrowed_object_leaf<mpfrxx::mpfr_class>& expr)
{
    return expr.get().precision();
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mpfr_prec_t mpfr_expression_precision(const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mpfr_prec_t mpfr_expression_precision(const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mpfr_prec_t mpfr_expression_precision(const mpfrxx::random_mpfr_expr& expr)
{
    return expr.materialization_precision();
}

template <typename T, typename Result>
mpfr_prec_t mpfr_expression_precision(const scalar_leaf<T, Result>&)
{
    return 0;
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

template <typename T>
struct mpfr_expression_contains_random : std::false_type {};

template <>
struct mpfr_expression_contains_random<mpfrxx::random_mpfr_expr> : std::true_type {};

template <typename Op, typename Expr, typename Result>
struct mpfr_expression_contains_random<unary_expr<Op, Expr, Result>>
    : mpfr_expression_contains_random<std::decay_t<Expr>> {};

template <typename Op, typename Lhs, typename Rhs, typename Result>
struct mpfr_expression_contains_random<binary_expr<Op, Lhs, Rhs, Result>>
    : std::bool_constant<
          mpfr_expression_contains_random<std::decay_t<Lhs>>::value ||
          mpfr_expression_contains_random<std::decay_t<Rhs>>::value> {};

template <typename Expr>
inline constexpr bool mpfr_expression_contains_random_v =
    mpfr_expression_contains_random<std::decay_t<Expr>>::value;

template <typename Expr>
mpfr_prec_t mpfr_materialization_precision(const Expr& expr)
{
    return mpfr_expression_precision(expr);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<mpfrxx::mpfr_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set(dest, expr.get().mpfr_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const borrowed_object_leaf<mpfrxx::mpfr_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set(dest, expr.get().mpfr_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<gmpxx::mpz_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set_z(dest, expr.get().mpz_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const borrowed_object_leaf<gmpxx::mpz_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set_z(dest, expr.get().mpz_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<gmpxx::mpq_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set_q(dest, expr.get().mpq_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const borrowed_object_leaf<gmpxx::mpq_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set_q(dest, expr.get().mpq_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const mpfrxx::random_mpfr_expr& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    expr.generate(dest, rnd);
}

template <typename T, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const scalar_leaf<T, Result>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<T, double>) {
        mpfr_set_d(dest, expr.value(), rnd);
    } else if constexpr (std::is_same_v<T, std::int64_t>) {
        if constexpr (std::numeric_limits<long>::digits >= 63) {
            mpfr_set_si(dest, static_cast<long>(expr.value()), rnd);
        } else {
            const gmpxx::mpz_class integer(expr.value());
            mpfr_set_z(dest, integer.mpz_data(), rnd);
        }
    } else if constexpr (std::is_same_v<T, std::uint64_t>) {
        if constexpr (std::numeric_limits<unsigned long>::digits >= 64) {
            mpfr_set_ui(dest, static_cast<unsigned long>(expr.value()), rnd);
        } else {
            const gmpxx::mpz_class integer(expr.value());
            mpfr_set_z(dest, integer.mpz_data(), rnd);
        }
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPFR scalar leaf");
    }
}

inline bool mpfr_expression_references(
    const mpfr_t target,
    const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpfr_data()[0]);
}

inline bool mpfr_expression_references(
    const mpfr_t target,
    const borrowed_object_leaf<mpfrxx::mpfr_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpfr_data()[0]);
}

inline bool mpfr_expression_references(const mpfr_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpfr_expression_references(const mpfr_t, const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpfr_expression_references(const mpfr_t, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

inline bool mpfr_expression_references(const mpfr_t, const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

inline bool mpfr_expression_references(const mpfr_t, const mpfrxx::random_mpfr_expr&)
{
    return false;
}

template <typename T, typename Result>
bool mpfr_expression_references(const mpfr_t, const scalar_leaf<T, Result>&)
{
    return false;
}

template <typename Op, typename Expr, typename Result>
bool mpfr_expression_references(
    const mpfr_t target,
    const unary_expr<Op, Expr, Result>& expr)
{
    return mpfr_expression_references(target, expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
bool mpfr_expression_references(
    const mpfr_t target,
    const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return mpfr_expression_references(target, expr.lhs()) ||
           mpfr_expression_references(target, expr.rhs());
}

inline unsigned long mpfr_shift_count_from_mpfr(const mpfr_t value)
{
    if (mpfr_nan_p(value) || mpfr_inf_p(value) || mpfr_integer_p(value) == 0) {
        throw std::overflow_error("shift count must be an integer");
    }

    mpz_t shift_count;
    mpz_init(shift_count);
    mpfr_get_z(shift_count, value, MPFR_RNDZ);

    unsigned long bits = 0;
    try {
        bits = zq_shift_count_from_mpz(shift_count);
    } catch (...) {
        mpz_clear(shift_count);
        throw;
    }
    mpz_clear(shift_count);
    return bits;
}

template <typename Op>
void mpfr_apply_binary(mpfr_t dest, const mpfr_t lhs, const mpfr_t rhs, mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<Op, add_op>) {
        mpfr_add(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpfr_sub(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpfr_mul(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpfr_div(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, shl_op>) {
        mpfr_mul_2ui(dest, lhs, mpfr_shift_count_from_mpfr(rhs), rnd);
    } else if constexpr (std::is_same_v<Op, shr_op>) {
        mpfr_div_2ui(dest, lhs, mpfr_shift_count_from_mpfr(rhs), rnd);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPFR expression operation");
    }
}

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

template <typename Expr>
void mpfr_evaluate_to_temporary(
    mpfr_t temp,
    const Expr& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(eval_precision);
    mpfr_init2(temp, eval_precision);
    try {
        mpfr_evaluate(temp, expr, eval_precision, rnd);
    } catch (...) {
        mpfr_clear(temp);
        throw;
    }
}

class scoped_mpfr_temporary {
public:
    explicit scoped_mpfr_temporary(mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
        mpfr_init2(value_, precision);
    }

    scoped_mpfr_temporary(const scoped_mpfr_temporary&) = delete;
    scoped_mpfr_temporary& operator=(const scoped_mpfr_temporary&) = delete;

    ~scoped_mpfr_temporary()
    {
        mpfr_clear(value_);
    }

    mpfr_ptr get() noexcept { return value_; }
    mpfr_srcptr get() const noexcept { return value_; }

private:
    mpfr_t value_;
};

class mpfr_thread_scratch_pool {
public:
    static constexpr std::size_t slot_count = 16;
    static constexpr mpfr_prec_t max_retained_precision = mpfr_prec_t{1} << 20;

    struct slot {
        mpfr_t value;
        bool initialized = false;
        bool in_use = false;
        mpfr_prec_t allocated_precision = 0;
        mpfr_prec_t active_precision = 0;
    };

    ~mpfr_thread_scratch_pool()
    {
        for (auto& item : slots_) {
            if (item.initialized) {
                restore_allocated_precision(item);
                mpfr_clear(item.value);
            }
        }
    }

    slot* acquire(mpfr_prec_t precision)
    {
        if (precision > max_retained_precision) {
            return nullptr;
        }

        for (auto& item : slots_) {
            if (!item.in_use) {
                prepare(item, precision);
                item.in_use = true;
                return &item;
            }
        }
        return nullptr;
    }

    static void release(slot& item) noexcept
    {
        // Fixed-precision fast paths reuse the slot at the requested active
        // precision. Restoring the allocation precision on every release would
        // put mpfr_set_prec_raw back into the hot loop.
        item.in_use = false;
    }

private:
    static void restore_allocated_precision(slot& item) noexcept
    {
        if (item.initialized && item.active_precision != item.allocated_precision) {
            mpfr_set_prec_raw(item.value, item.allocated_precision);
            item.active_precision = item.allocated_precision;
        }
    }

    static void prepare(slot& item, mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
        if (!item.initialized) {
            mpfr_init2(item.value, precision);
            item.initialized = true;
            item.allocated_precision = mpfr_get_prec(item.value);
            item.active_precision = item.allocated_precision;
        } else if (item.allocated_precision < precision) {
            restore_allocated_precision(item);
            mpfr_set_prec(item.value, precision);
            item.allocated_precision = mpfr_get_prec(item.value);
            item.active_precision = item.allocated_precision;
        }

        if (item.active_precision != precision) {
            mpfr_set_prec_raw(item.value, precision);
            item.active_precision = precision;
        }
    }

    slot slots_[slot_count]{};
};

inline mpfr_thread_scratch_pool& mpfr_current_thread_scratch_pool()
{
    thread_local mpfr_thread_scratch_pool pool;
    return pool;
}

class mpfr_thread_scratch {
public:
    explicit mpfr_thread_scratch(mpfr_prec_t precision)
        : slot_(mpfr_current_thread_scratch_pool().acquire(precision))
    {
        if (slot_ == nullptr) {
            gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
            mpfr_init2(fallback_, precision);
            using_fallback_ = true;
        }
    }

    mpfr_thread_scratch(const mpfr_thread_scratch&) = delete;
    mpfr_thread_scratch& operator=(const mpfr_thread_scratch&) = delete;

    ~mpfr_thread_scratch()
    {
        if (using_fallback_) {
            mpfr_clear(fallback_);
        } else {
            mpfr_thread_scratch_pool::release(*slot_);
        }
    }

    mpfr_ptr get() noexcept
    {
        return using_fallback_ ? fallback_ : slot_->value;
    }

private:
    mpfr_thread_scratch_pool::slot* slot_ = nullptr;
    bool using_fallback_ = false;
    mpfr_t fallback_;
};

template <typename T>
struct is_mpfr_object_leaf : std::false_type {};

template <>
struct is_mpfr_object_leaf<object_leaf<mpfrxx::mpfr_class>> : std::true_type {};

template <>
struct is_mpfr_object_leaf<borrowed_object_leaf<mpfrxx::mpfr_class>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_object_leaf_v = is_mpfr_object_leaf<std::decay_t<T>>::value;

template <typename T>
struct is_mpfr_mul_direct_expr : std::false_type {};

template <typename Lhs, typename Rhs>
struct is_mpfr_mul_direct_expr<binary_expr<mul_op, Lhs, Rhs, mpfrxx::mpfr_class>>
    : std::bool_constant<is_mpfr_object_leaf_v<Lhs> && is_mpfr_object_leaf_v<Rhs>> {};

template <typename T>
inline constexpr bool is_mpfr_mul_direct_expr_v =
    is_mpfr_mul_direct_expr<std::decay_t<T>>::value;

template <typename Expr>
inline constexpr bool mpfr_materialization_precision_is_nonzero_v = false;

template <typename Op, typename Lhs, typename Rhs>
inline constexpr bool mpfr_materialization_precision_is_nonzero_v<binary_expr<Op, Lhs, Rhs, mpfrxx::mpfr_class>> =
    is_mpfr_object_leaf_v<Lhs> && is_mpfr_object_leaf_v<Rhs>;

template <typename Expr>
mpfr_prec_t mpfr_constructor_materialization_precision(const Expr& expr)
{
    mpfr_prec_t precision = mpfr_materialization_precision(expr);
    if constexpr (!mpfr_materialization_precision_is_nonzero_v<std::decay_t<Expr>>) {
        if (precision == 0) {
            precision = mpfrxx::default_precision_bits();
        }
    }
    return precision;
}

inline mpfr_rnd_t mpfr_dual_rounding_for_negated_result(mpfr_rnd_t rnd) noexcept
{
    switch (rnd) {
    case MPFR_RNDU:
        return MPFR_RNDD;
    case MPFR_RNDD:
        return MPFR_RNDU;
    default:
        return rnd;
    }
}

template <typename Lhs, typename Rhs>
void mpfr_compound_fma_apply(
    mpfr_t dest,
    const binary_expr<mul_op, Lhs, Rhs, mpfrxx::mpfr_class>& expr,
    mpfr_rnd_t rnd)
{
    mpfr_fma(dest, expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), dest, rnd);
}

template <typename Lhs, typename Rhs>
void mpfr_compound_submul_fma_apply(
    mpfr_t dest,
    const binary_expr<mul_op, Lhs, Rhs, mpfrxx::mpfr_class>& expr,
    mpfr_rnd_t rnd)
{
    mpfr_fms(
        dest,
        expr.lhs().get().mpfr_data(),
        expr.rhs().get().mpfr_data(),
        dest,
        mpfr_dual_rounding_for_negated_result(rnd));
    mpfr_neg(dest, dest, MPFR_RNDN);
}

template <typename Lhs, typename Rhs>
GMPFRXX_MKII_ALWAYS_INLINE void mpfr_compound_mul_apply(
    mpfr_t dest,
    const binary_expr<mul_op, Lhs, Rhs, mpfrxx::mpfr_class>& expr,
    mpfr_prec_t precision,
    mpfr_rnd_t rnd)
{
    if constexpr (build_options::assume_fixed_precision_fastpath) {
        mpfr_thread_scratch product(precision);
        mpfr_mul(product.get(), expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
        mpfr_add(dest, dest, product.get(), rnd);
    } else {
        scoped_mpfr_temporary product(precision);
        mpfr_mul(product.get(), expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
        mpfr_add(dest, dest, product.get(), rnd);
    }
}

template <typename Lhs, typename Rhs>
GMPFRXX_MKII_ALWAYS_INLINE void mpfr_compound_submul_apply(
    mpfr_t dest,
    const binary_expr<mul_op, Lhs, Rhs, mpfrxx::mpfr_class>& expr,
    mpfr_prec_t precision,
    mpfr_rnd_t rnd)
{
    if constexpr (build_options::assume_fixed_precision_fastpath) {
        mpfr_thread_scratch product(precision);
        mpfr_mul(product.get(), expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
        mpfr_sub(dest, dest, product.get(), rnd);
    } else {
        scoped_mpfr_temporary product(precision);
        mpfr_mul(product.get(), expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
        mpfr_sub(dest, dest, product.get(), rnd);
    }
}

template <typename Lhs, typename Rhs>
void mpfr_fmma_direct_apply(
    mpfr_t dest,
    const binary_expr<add_op, Lhs, Rhs, mpfrxx::mpfr_class>& expr,
    mpfr_rnd_t rnd)
{
    mpfr_fmma(dest,
              expr.lhs().lhs().get().mpfr_data(),
              expr.lhs().rhs().get().mpfr_data(),
              expr.rhs().lhs().get().mpfr_data(),
              expr.rhs().rhs().get().mpfr_data(),
              rnd);
}

template <typename Lhs, typename Rhs>
void mpfr_fmms_direct_apply(
    mpfr_t dest,
    const binary_expr<sub_op, Lhs, Rhs, mpfrxx::mpfr_class>& expr,
    mpfr_rnd_t rnd)
{
    mpfr_fmms(dest,
              expr.lhs().lhs().get().mpfr_data(),
              expr.lhs().rhs().get().mpfr_data(),
              expr.rhs().lhs().get().mpfr_data(),
              expr.rhs().rhs().get().mpfr_data(),
              rnd);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        scoped_mpz_t exact;
        mpz_evaluate(exact.get(), expr);
        mpfr_set_z(dest, exact.get(), rnd);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        scoped_mpq_t exact;
        mpq_evaluate(exact.get(), expr);
        mpfr_set_q(dest, exact.get(), rnd);
        return;
    }

    if constexpr (std::is_same_v<Op, shl_op> || std::is_same_v<Op, shr_op>) {
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        unsigned long shift_count = 0;
        if constexpr (is_zq_shift_scalar_leaf_v<Rhs>) {
            shift_count = zq_shift_count_from_scalar(expr.rhs().value());
        } else {
            scoped_mpz_t bits_value;
            mpz_evaluate(bits_value.get(), expr.rhs());
            shift_count = zq_shift_count_from_mpz(bits_value.get());
        }
        if constexpr (std::is_same_v<Op, shl_op>) {
            mpfr_mul_2ui(dest, dest, shift_count, rnd);
        } else {
            mpfr_div_2ui(dest, dest, shift_count, rnd);
        }
        return;
    }

    if (mpfr_expression_references(dest, expr)) {
        scoped_mpfr_temporary lhs(eval_precision);
        scoped_mpfr_temporary rhs(eval_precision);
        mpfr_evaluate(lhs.get(), expr.lhs(), eval_precision, rnd);
        mpfr_evaluate(rhs.get(), expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, lhs.get(), rhs.get(), rnd);
        return;
    }

    if constexpr (
        build_options::enable_mpfr_fma &&
        is_mpfr_mul_direct_expr_v<Lhs> &&
        is_mpfr_mul_direct_expr_v<Rhs> &&
        (std::is_same_v<Op, add_op> || std::is_same_v<Op, sub_op>)) {
        if constexpr (std::is_same_v<Op, add_op>) {
            mpfr_fmma_direct_apply(dest, expr, rnd);
        } else {
            mpfr_fmms_direct_apply(dest, expr, rnd);
        }
        return;
    }

    if constexpr (is_mpfr_object_leaf_v<Lhs> &&
                  is_mpfr_object_leaf_v<Rhs>) {
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
    } else if constexpr (is_mpfr_object_leaf_v<Rhs>) {
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, dest, expr.rhs().get().mpfr_data(), rnd);
    } else if constexpr (is_mpfr_object_leaf_v<Lhs> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpfr_evaluate(dest, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), dest, rnd);
    } else {
        scoped_mpfr_temporary rhs(eval_precision);
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpfr_evaluate(rhs.get(), expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, dest, rhs.get(), rnd);
    }
}

template <typename Expr>
bool mpfr_try_assign_direct_leaf_binary(mpfr_t, const Expr&, mpfr_rnd_t)
{
    return false;
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
bool mpfr_try_assign_direct_leaf_binary(
    mpfr_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<Result, mpfrxx::mpfr_class> &&
                  is_mpfr_object_leaf_v<Lhs> &&
                  is_mpfr_object_leaf_v<Rhs> &&
                  (std::is_same_v<Op, add_op> ||
                   std::is_same_v<Op, sub_op> ||
                   std::is_same_v<Op, mul_op> ||
                   std::is_same_v<Op, div_op>)) {
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
        return true;
    } else {
        return false;
    }
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
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
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
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
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
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
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
                                    int> = 0>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<div_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Expr, std::enable_if_t<is_mpfr_expression_operand_v<Expr> &&
                                              is_mpfr_object_or_node_v<Expr>,
                                          int> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpfr_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), mpfrxx::mpfr_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpfr_expression_operand_v<Expr> &&
                                              is_mpfr_object_or_node_v<Expr>,
                                          int> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpfr_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), mpfrxx::mpfr_class>(std::move(operand));
}

template <typename Lhs, typename Bits, std::enable_if_t<
                                    is_mpfr_object_or_node_v<Lhs> &&
                                        is_supported_expression_integral_v<std::decay_t<Bits>>,
                                    int> = 0>
auto operator<<(Lhs&& lhs, Bits bits)
{
    if constexpr (std::is_signed_v<std::decay_t<Bits>>) {
        if (bits < 0) {
            throw std::invalid_argument("negative shift count");
        }
    }
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = scalar_leaf<std::uint64_t, gmpxx::mpz_class>(static_cast<std::uint64_t>(bits));
    return binary_expr<shl_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Bits, std::enable_if_t<
                                    is_mpfr_object_or_node_v<Lhs> &&
                                        is_supported_expression_integral_v<std::decay_t<Bits>>,
                                    int> = 0>
auto operator>>(Lhs&& lhs, Bits bits)
{
    if constexpr (std::is_signed_v<std::decay_t<Bits>>) {
        if (bits < 0) {
            throw std::invalid_argument("negative shift count");
        }
    }
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = scalar_leaf<std::uint64_t, gmpxx::mpz_class>(static_cast<std::uint64_t>(bits));
    return binary_expr<shr_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Op, typename Rhs>
void mpfr_compound_assign_with_context(
    mpfrxx::mpfr_class& lhs,
    Rhs&& rhs,
    eval_context context)
{
    auto operand = make_mpfr_operand(std::forward<Rhs>(rhs));
    using operand_type = std::decay_t<decltype(operand)>;

    const mpfr_prec_t precision = context.precision_bits;

    if constexpr (is_mpfr_object_leaf_v<operand_type>) {
        mpfr_apply_binary<Op>(
            lhs.mpfr_data(),
            lhs.mpfr_data(),
            operand.get().mpfr_data(),
            context.rounding_mode);
    } else if constexpr (
        is_mpfr_mul_direct_expr_v<operand_type> &&
        (std::is_same_v<Op, add_op> || std::is_same_v<Op, sub_op>)) {
        if constexpr (build_options::enable_mpfr_fma) {
            if constexpr (std::is_same_v<Op, add_op>) {
                mpfr_compound_fma_apply(
                    lhs.mpfr_data(),
                    operand,
                    context.rounding_mode);
            } else {
                mpfr_compound_submul_fma_apply(
                    lhs.mpfr_data(),
                    operand,
                    context.rounding_mode);
            }
        } else {
            if constexpr (std::is_same_v<Op, add_op>) {
                mpfr_compound_mul_apply(
                    lhs.mpfr_data(),
                    operand,
                    precision,
                    context.rounding_mode);
            } else {
                mpfr_compound_submul_apply(
                    lhs.mpfr_data(),
                    operand,
                    precision,
                    context.rounding_mode);
            }
        }
    } else {
        scoped_mpfr_temporary value(precision);
        mpfr_evaluate(value.get(), operand, precision, context.rounding_mode);
        mpfr_apply_binary<Op>(lhs.mpfr_data(), lhs.mpfr_data(), value.get(), context.rounding_mode);
    }
}

template <typename Op, typename Rhs>
void mpfr_compound_assign(mpfrxx::mpfr_class& lhs, Rhs&& rhs)
{
    const mpfr_prec_t precision = mpfr_get_prec(lhs.mpfr_data());
    const auto context = current_eval_context(precision);
    mpfr_compound_assign_with_context<Op>(lhs, std::forward<Rhs>(rhs), context);
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

template <typename Expr, typename>
mpfr_class::mpfr_class(const Expr& expr)
{
    const mpfr_prec_t precision = gmpfrxx_mkII::detail::mpfr_constructor_materialization_precision(expr);
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
    mpfr_init2(value_, precision);
    try {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
    } catch (...) {
        mpfr_clear(value_);
        throw;
    }
}

template <typename Expr, typename, typename>
mpfr_class::mpfr_class(const Expr& expr, mpfr_prec_t precision)
{
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
    mpfr_init2(value_, precision);
    try {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
    } catch (...) {
        mpfr_clear(value_);
        throw;
    }
}

template <typename Expr, typename>
mpfr_class& mpfr_class::operator=(const Expr& expr)
{
    const mpfr_prec_t precision = this->precision();
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    if (gmpfrxx_mkII::detail::mpfr_try_assign_direct_leaf_binary(value_, expr, context.rounding_mode)) {
        return *this;
    }
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
    return *this;
}

class mpfr_context_ref {
public:
    mpfr_context_ref(mpfr_class& value, evaluation_context context)
        : value_(&value),
          context_(context),
          precision_(context.precision),
          rounding_mode_(context.rounding_mode)
    {
        check_precision();
    }

    mpfr_context_ref(mpfr_class& value, mpfr_prec_t precision, mpfr_rnd_t rounding_mode)
        : value_(&value),
          context_{precision, rounding_mode},
          precision_(precision),
          rounding_mode_(rounding_mode)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
        check_precision();
    }

private:
    void check_precision() const
    {
        if constexpr (!gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath) {
            if (precision_ != value_->precision()) {
                throw std::invalid_argument("mpfr evaluation context precision must match target precision");
            }
        }
    }

public:
    mpfr_context_ref(const mpfr_context_ref&) = default;
    mpfr_context_ref& operator=(const mpfr_context_ref&) = default;

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
    mpfr_context_ref& operator=(Rhs&& rhs)
    {
        auto operand = gmpfrxx_mkII::detail::make_mpfr_operand(std::forward<Rhs>(rhs));
        const auto context = detail_context();
        if (gmpfrxx_mkII::detail::mpfr_try_assign_direct_leaf_binary(
                value_->mpfr_data(),
                operand,
                context.rounding_mode)) {
            return *this;
        }
        gmpfrxx_mkII::detail::mpfr_evaluate(
            value_->mpfr_data(),
            operand,
            context.precision_bits,
            context.rounding_mode);
        return *this;
    }

    mpfr_class& value() const noexcept
    {
        return *value_;
    }

    const evaluation_context& context() const noexcept
    {
        return context_;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
    mpfr_context_ref& operator+=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpfr_compound_assign_with_context<gmpfrxx_mkII::detail::add_op>(
            *value_,
            std::forward<Rhs>(rhs),
            detail_context());
        return *this;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
    mpfr_context_ref& operator-=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpfr_compound_assign_with_context<gmpfrxx_mkII::detail::sub_op>(
            *value_,
            std::forward<Rhs>(rhs),
            detail_context());
        return *this;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
    mpfr_context_ref& operator*=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpfr_compound_assign_with_context<gmpfrxx_mkII::detail::mul_op>(
            *value_,
            std::forward<Rhs>(rhs),
            detail_context());
        return *this;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
    mpfr_context_ref& operator/=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpfr_compound_assign_with_context<gmpfrxx_mkII::detail::div_op>(
            *value_,
            std::forward<Rhs>(rhs),
            detail_context());
        return *this;
    }

private:
    gmpfrxx_mkII::detail::eval_context detail_context() const noexcept
    {
        return gmpfrxx_mkII::detail::eval_context{
            precision_,
            rounding_mode_,
        };
    }

    mpfr_class* value_;
    evaluation_context context_;
    mpfr_prec_t precision_;
    mpfr_rnd_t rounding_mode_;
};

inline mpfr_context_ref with_context(mpfr_class& value, evaluation_context context)
{
    return mpfr_context_ref(value, context);
}

inline mpfr_context_ref with_context(mpfr_class& value, mpfr_prec_t precision, mpfr_rnd_t rounding_mode)
{
    return mpfr_context_ref(value, precision, rounding_mode);
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;
using ::gmpfrxx_mkII::detail::operator<<;
using ::gmpfrxx_mkII::detail::operator>>;

template <typename T>
struct is_mpfr_comparison_non_scalar
    : std::bool_constant<
          std::is_same_v<std::decay_t<T>, mpfr_class> ||
          std::is_same_v<std::decay_t<T>, gmpxx::mpz_class> ||
          std::is_same_v<std::decay_t<T>, gmpxx::mpq_class> ||
          gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<T>>> {};

template <typename Lhs, typename Rhs>
struct is_mpfr_comparison_pair
    : std::bool_constant<
          gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
          gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
          !gmpfrxx_mkII::detail::is_zq_comparison_pair_v<Lhs, Rhs> &&
          (is_mpfr_comparison_non_scalar<Lhs>::value ||
           is_mpfr_comparison_non_scalar<Rhs>::value)> {};

class scoped_mpfr_t {
public:
    explicit scoped_mpfr_t(mpfr_prec_t precision)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision);
        mpfr_init2(value_, precision);
    }

    scoped_mpfr_t(const scoped_mpfr_t&) = delete;
    scoped_mpfr_t& operator=(const scoped_mpfr_t&) = delete;

    ~scoped_mpfr_t()
    {
        mpfr_clear(value_);
    }

    mpfr_ptr get() noexcept { return value_; }
    mpfr_srcptr get() const noexcept { return value_; }

private:
    mpfr_t value_;
};

struct mpfr_comparison_result {
    int order;
    bool has_nan;
};

template <typename T>
struct is_mpfr_class_comparison_leaf : std::false_type {};

template <>
struct is_mpfr_class_comparison_leaf<gmpfrxx_mkII::detail::object_leaf<mpfrxx::mpfr_class>> : std::true_type {};

template <>
struct is_mpfr_class_comparison_leaf<gmpfrxx_mkII::detail::borrowed_object_leaf<mpfrxx::mpfr_class>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_class_comparison_leaf_v =
    is_mpfr_class_comparison_leaf<std::decay_t<T>>::value;

template <typename T>
struct is_mpfr_exact_comparison_leaf : std::false_type {};

template <>
struct is_mpfr_exact_comparison_leaf<gmpfrxx_mkII::detail::object_leaf<gmpxx::mpz_class>> : std::true_type {};

template <>
struct is_mpfr_exact_comparison_leaf<gmpfrxx_mkII::detail::borrowed_object_leaf<gmpxx::mpz_class>>
    : std::true_type {};

template <>
struct is_mpfr_exact_comparison_leaf<gmpfrxx_mkII::detail::object_leaf<gmpxx::mpq_class>> : std::true_type {};

template <>
struct is_mpfr_exact_comparison_leaf<gmpfrxx_mkII::detail::borrowed_object_leaf<gmpxx::mpq_class>>
    : std::true_type {};

template <typename T, typename Result>
struct is_mpfr_exact_comparison_leaf<gmpfrxx_mkII::detail::scalar_leaf<T, Result>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_exact_comparison_leaf_v =
    is_mpfr_exact_comparison_leaf<std::decay_t<T>>::value;

inline int mpfr_reverse_comparison_order(int order) noexcept
{
    if (order < 0) {
        return 1;
    }
    if (order > 0) {
        return -1;
    }
    return 0;
}

inline mpfr_srcptr mpfr_comparison_leaf_ptr(
    const gmpfrxx_mkII::detail::object_leaf<mpfrxx::mpfr_class>& expr) noexcept
{
    return expr.get().mpfr_data();
}

inline mpfr_srcptr mpfr_comparison_leaf_ptr(
    const gmpfrxx_mkII::detail::borrowed_object_leaf<mpfrxx::mpfr_class>& expr) noexcept
{
    return expr.get().mpfr_data();
}

inline mpfr_comparison_result mpfr_compare_mpfr_to_exact_leaf(
    mpfr_srcptr lhs,
    const gmpfrxx_mkII::detail::object_leaf<gmpxx::mpz_class>& rhs)
{
    if (mpfr_nan_p(lhs) != 0) {
        return {0, true};
    }
    return {mpfr_cmp_z(lhs, rhs.get().mpz_data()), false};
}

inline mpfr_comparison_result mpfr_compare_mpfr_to_exact_leaf(
    mpfr_srcptr lhs,
    const gmpfrxx_mkII::detail::borrowed_object_leaf<gmpxx::mpz_class>& rhs)
{
    if (mpfr_nan_p(lhs) != 0) {
        return {0, true};
    }
    return {mpfr_cmp_z(lhs, rhs.get().mpz_data()), false};
}

inline mpfr_comparison_result mpfr_compare_mpfr_to_exact_leaf(
    mpfr_srcptr lhs,
    const gmpfrxx_mkII::detail::object_leaf<gmpxx::mpq_class>& rhs)
{
    if (mpfr_nan_p(lhs) != 0) {
        return {0, true};
    }
    return {mpfr_cmp_q(lhs, rhs.get().mpq_data()), false};
}

inline mpfr_comparison_result mpfr_compare_mpfr_to_exact_leaf(
    mpfr_srcptr lhs,
    const gmpfrxx_mkII::detail::borrowed_object_leaf<gmpxx::mpq_class>& rhs)
{
    if (mpfr_nan_p(lhs) != 0) {
        return {0, true};
    }
    return {mpfr_cmp_q(lhs, rhs.get().mpq_data()), false};
}

inline int mpfr_cmp_int64_exact(mpfr_srcptr lhs, std::int64_t rhs)
{
    if constexpr (std::numeric_limits<long>::digits >= 63) {
        return mpfr_cmp_si(lhs, static_cast<long>(rhs));
    } else {
        const gmpxx::mpz_class integer(rhs);
        return mpfr_cmp_z(lhs, integer.mpz_data());
    }
}

inline int mpfr_cmp_uint64_exact(mpfr_srcptr lhs, std::uint64_t rhs)
{
    if constexpr (std::numeric_limits<unsigned long>::digits >= 64) {
        return mpfr_cmp_ui(lhs, static_cast<unsigned long>(rhs));
    } else {
        const gmpxx::mpz_class integer(rhs);
        return mpfr_cmp_z(lhs, integer.mpz_data());
    }
}

template <typename T, typename Result>
mpfr_comparison_result mpfr_compare_mpfr_to_exact_leaf(
    mpfr_srcptr lhs,
    const gmpfrxx_mkII::detail::scalar_leaf<T, Result>& rhs)
{
    if (mpfr_nan_p(lhs) != 0) {
        return {0, true};
    }
    if constexpr (std::is_same_v<T, double>) {
        if (std::isnan(rhs.value())) {
            return {0, true};
        }
        return {mpfr_cmp_d(lhs, rhs.value()), false};
    } else if constexpr (std::is_same_v<T, std::int64_t>) {
        return {mpfr_cmp_int64_exact(lhs, rhs.value()), false};
    } else if constexpr (std::is_same_v<T, std::uint64_t>) {
        return {mpfr_cmp_uint64_exact(lhs, rhs.value()), false};
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPFR comparison scalar leaf");
    }
}

template <typename LhsLeaf, typename RhsLeaf>
bool mpfr_try_exact_leaf_compare(const LhsLeaf& lhs, const RhsLeaf& rhs, mpfr_comparison_result& result)
{
    if constexpr (is_mpfr_class_comparison_leaf_v<LhsLeaf> && is_mpfr_class_comparison_leaf_v<RhsLeaf>) {
        const mpfr_srcptr left = mpfr_comparison_leaf_ptr(lhs);
        const mpfr_srcptr right = mpfr_comparison_leaf_ptr(rhs);
        if (mpfr_nan_p(left) != 0 || mpfr_nan_p(right) != 0) {
            result = {0, true};
        } else {
            result = {mpfr_cmp(left, right), false};
        }
        return true;
    } else if constexpr (is_mpfr_class_comparison_leaf_v<LhsLeaf> && is_mpfr_exact_comparison_leaf_v<RhsLeaf>) {
        result = mpfr_compare_mpfr_to_exact_leaf(mpfr_comparison_leaf_ptr(lhs), rhs);
        return true;
    } else if constexpr (is_mpfr_exact_comparison_leaf_v<LhsLeaf> && is_mpfr_class_comparison_leaf_v<RhsLeaf>) {
        result = mpfr_compare_mpfr_to_exact_leaf(mpfr_comparison_leaf_ptr(rhs), lhs);
        result.order = mpfr_reverse_comparison_order(result.order);
        return true;
    } else {
        return false;
    }
}

template <typename Expr, typename ExactLeaf>
mpfr_comparison_result mpfr_compare_expression_to_exact_leaf(const Expr& expr, const ExactLeaf& exact)
{
    mpfr_prec_t precision = gmpfrxx_mkII::detail::mpfr_expression_precision(expr);
    if (precision == 0) {
        precision = mpfr_class::default_precision();
    }

    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    scoped_mpfr_t value(precision);
    gmpfrxx_mkII::detail::mpfr_evaluate(value.get(), expr, precision, context.rounding_mode);
    return mpfr_compare_mpfr_to_exact_leaf(value.get(), exact);
}

template <typename LhsExpr, typename RhsExpr>
bool mpfr_try_expression_exact_compare(const LhsExpr& lhs, const RhsExpr& rhs, mpfr_comparison_result& result)
{
    if constexpr (!is_mpfr_exact_comparison_leaf_v<LhsExpr> && is_mpfr_exact_comparison_leaf_v<RhsExpr>) {
        result = mpfr_compare_expression_to_exact_leaf(lhs, rhs);
        return true;
    } else if constexpr (is_mpfr_exact_comparison_leaf_v<LhsExpr> && !is_mpfr_exact_comparison_leaf_v<RhsExpr>) {
        result = mpfr_compare_expression_to_exact_leaf(rhs, lhs);
        result.order = mpfr_reverse_comparison_order(result.order);
        return true;
    } else {
        return false;
    }
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline mpfr_comparison_result cmp_with_nan_status(Lhs&& lhs, Rhs&& rhs)
{
    auto left = gmpfrxx_mkII::detail::make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = gmpfrxx_mkII::detail::make_mpfr_operand(std::forward<Rhs>(rhs));

    mpfr_comparison_result exact_result{0, false};
    if (mpfr_try_exact_leaf_compare(left, right, exact_result)) {
        return exact_result;
    }
    if (mpfr_try_expression_exact_compare(left, right, exact_result)) {
        return exact_result;
    }

    mpfr_prec_t precision = std::max(
        gmpfrxx_mkII::detail::mpfr_expression_precision(left),
        gmpfrxx_mkII::detail::mpfr_expression_precision(right));
    if (precision == 0) {
        precision = mpfr_class::default_precision();
    }

    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);

    scoped_mpfr_t lhs_value(precision);
    scoped_mpfr_t rhs_value(precision);
    gmpfrxx_mkII::detail::mpfr_evaluate(lhs_value.get(), left, precision, context.rounding_mode);
    gmpfrxx_mkII::detail::mpfr_evaluate(rhs_value.get(), right, precision, context.rounding_mode);
    if (mpfr_nan_p(lhs_value.get()) != 0 || mpfr_nan_p(rhs_value.get()) != 0) {
        return {0, true};
    }
    return {mpfr_cmp(lhs_value.get(), rhs_value.get()), false};
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline int cmp(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    if (result.has_nan) {
        throw std::domain_error("unordered comparison with NaN");
    }
    return result.order;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator==(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    return !result.has_nan && result.order == 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator!=(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    return result.has_nan || result.order != 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator<(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    return !result.has_nan && result.order < 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator<=(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    return !result.has_nan && result.order <= 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator>(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    return !result.has_nan && result.order > 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator>=(Lhs&& lhs, Rhs&& rhs)
{
    const auto result = cmp_with_nan_status(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    return !result.has_nan && result.order >= 0;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpz_class& operator+=(gmpxx::mpz_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpz_class>(mpfr_class(mpfr_class(lhs) + std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpz_class& operator-=(gmpxx::mpz_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpz_class>(mpfr_class(mpfr_class(lhs) - std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpz_class& operator*=(gmpxx::mpz_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpz_class>(mpfr_class(mpfr_class(lhs) * std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpz_class& operator/=(gmpxx::mpz_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpz_class>(mpfr_class(mpfr_class(lhs) / std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpq_class& operator+=(gmpxx::mpq_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpq_class>(mpfr_class(mpfr_class(lhs) + std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpq_class& operator-=(gmpxx::mpq_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpq_class>(mpfr_class(mpfr_class(lhs) - std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpq_class& operator*=(gmpxx::mpq_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpq_class>(mpfr_class(mpfr_class(lhs) * std::forward<Rhs>(rhs)));
    return lhs;
}

template <
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs>,
                     int> = 0>
inline gmpxx::mpq_class& operator/=(gmpxx::mpq_class& lhs, Rhs&& rhs)
{
    lhs = static_cast<gmpxx::mpq_class>(mpfr_class(mpfr_class(lhs) / std::forward<Rhs>(rhs)));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator+=(mpfr_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfr_compound_assign<gmpfrxx_mkII::detail::add_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator-=(mpfr_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfr_compound_assign<gmpfrxx_mkII::detail::sub_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator*=(mpfr_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfr_compound_assign<gmpfrxx_mkII::detail::mul_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator/=(mpfr_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpfr_compound_assign<gmpfrxx_mkII::detail::div_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

inline mpfr_class& operator<<=(mpfr_class& value, unsigned long bits)
{
    const auto context = gmpfrxx_mkII::detail::current_eval_context(value.precision());
    mpfr_mul_2ui(value.mpfr_data(), value.mpfr_data(), bits, context.rounding_mode);
    return value;
}

inline mpfr_class& operator>>=(mpfr_class& value, unsigned long bits)
{
    const auto context = gmpfrxx_mkII::detail::current_eval_context(value.precision());
    mpfr_div_2ui(value.mpfr_data(), value.mpfr_data(), bits, context.rounding_mode);
    return value;
}

inline mpfr_class& operator++(mpfr_class& value)
{
    value += 1;
    return value;
}

inline mpfr_class operator++(mpfr_class& value, int)
{
    mpfr_class old(value);
    ++value;
    return old;
}

inline mpfr_class operator++(mpfr_class&& value)
{
    ++value;
    return std::move(value);
}

inline mpfr_class operator++(mpfr_class&& value, int)
{
    mpfr_class old(value);
    ++value;
    return old;
}

inline mpfr_class& operator--(mpfr_class& value)
{
    value -= 1;
    return value;
}

inline mpfr_class operator--(mpfr_class& value, int)
{
    mpfr_class old(value);
    --value;
    return old;
}

inline mpfr_class operator--(mpfr_class&& value)
{
    --value;
    return std::move(value);
}

inline mpfr_class operator--(mpfr_class&& value, int)
{
    mpfr_class old(value);
    --value;
    return old;
}

namespace detail {

template <typename Expr>
inline mpfr_class materialize_mpfr_math_operand(const Expr& expr)
{
    if constexpr (std::is_same_v<std::decay_t<Expr>, mpfr_class>) {
        return expr;
    } else {
        return mpfr_class(expr);
    }
}

template <typename Expr>
inline mpfr_class materialize_mpfr_unary_value(const Expr& expr)
{
    return materialize_mpfr_math_operand(expr);
}

template <typename Lhs, typename Rhs>
inline std::pair<mpfr_class, mpfr_class> materialize_mpfr_binary_values(const Lhs& lhs, const Rhs& rhs)
{
    mpfr_class left = materialize_mpfr_math_operand(lhs);
    mpfr_class right = materialize_mpfr_math_operand(rhs);
    const mpfr_prec_t precision = std::max(left.precision(), right.precision());
    if (left.precision() != precision) {
        left = mpfr_class(left, precision);
    }
    if (right.precision() != precision) {
        right = mpfr_class(right, precision);
    }
    return {std::move(left), std::move(right)};
}

template <typename Expr, typename Function>
inline mpfr_class unary_mpfr_math(const Expr& expr, Function function)
{
    const mpfr_class operand = materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    function(result.mpfr_data(), operand.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

template <typename Lhs, typename Rhs, typename Function>
inline mpfr_class binary_mpfr_math(const Lhs& lhs, const Rhs& rhs, Function function)
{
    const mpfr_class left = materialize_mpfr_math_operand(lhs);
    const mpfr_class right = materialize_mpfr_math_operand(rhs);
    const mpfr_prec_t precision = std::max(left.precision(), right.precision());
    mpfr_class result = mpfr_class::with_precision(precision);
    function(result.mpfr_data(), left.mpfr_data(), right.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

template <typename A, typename B, typename C, typename Function>
inline mpfr_class ternary_mpfr_math(const A& a, const B& b, const C& c, Function function)
{
    const mpfr_class first = materialize_mpfr_math_operand(a);
    const mpfr_class second = materialize_mpfr_math_operand(b);
    const mpfr_class third = materialize_mpfr_math_operand(c);
    const mpfr_prec_t precision = std::max({first.precision(), second.precision(), third.precision()});
    mpfr_class result = mpfr_class::with_precision(precision);
    function(result.mpfr_data(),
             first.mpfr_data(),
             second.mpfr_data(),
             third.mpfr_data(),
             mpfr_class::default_rounding());
    return result;
}

template <typename A, typename B, typename C, typename D, typename Function>
inline mpfr_class quaternary_mpfr_math(const A& a, const B& b, const C& c, const D& d, Function function)
{
    const mpfr_class first = materialize_mpfr_math_operand(a);
    const mpfr_class second = materialize_mpfr_math_operand(b);
    const mpfr_class third = materialize_mpfr_math_operand(c);
    const mpfr_class fourth = materialize_mpfr_math_operand(d);
    const mpfr_prec_t precision = std::max({first.precision(),
                                            second.precision(),
                                            third.precision(),
                                            fourth.precision()});
    mpfr_class result = mpfr_class::with_precision(precision);
    function(result.mpfr_data(),
             first.mpfr_data(),
             second.mpfr_data(),
             third.mpfr_data(),
             fourth.mpfr_data(),
             mpfr_class::default_rounding());
    return result;
}

} // namespace detail

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class sqrt(const Expr& expr)
{
    return detail::unary_mpfr_math(expr, [](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) {
        mpfr_sqrt(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class sqr(const Expr& expr)
{
    return detail::unary_mpfr_math(expr, [](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) {
        mpfr_sqr(rop, op, rnd);
    });
}

inline mpfr_class sqrt_ui(unsigned long value, mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_sqrt_ui(result.mpfr_data(), value, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class sqrt_ui(unsigned long value)
{
    return sqrt_ui(value, mpfr_class::default_precision());
}

inline mpfr_class const_pi(mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_const_pi(result.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

inline mpfr_class const_pi()
{
    return const_pi(mpfr_class::default_precision());
}

inline mpfr_class const_log2(mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_const_log2(result.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

inline mpfr_class const_log2()
{
    return const_log2(mpfr_class::default_precision());
}

inline mpfr_class const_e(mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_set_ui(result.mpfr_data(), 1ul, mpfr_class::default_rounding());
    mpfr_exp(result.mpfr_data(), result.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

inline mpfr_class const_e()
{
    return const_e(mpfr_class::default_precision());
}

inline mpfr_class const_log10(mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_log_ui(result.mpfr_data(), 10ul, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class const_log10()
{
    return const_log10(mpfr_class::default_precision());
}

inline mpfr_class pi_over_two(mpfr_prec_t precision)
{
    mpfr_class result = const_pi(precision);
    mpfr_div_2ui(result.mpfr_data(), result.mpfr_data(), 1ul, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class pi_over_two()
{
    return pi_over_two(mpfr_class::default_precision());
}

inline mpfr_class const_euler(mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_const_euler(result.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

inline mpfr_class const_euler()
{
    return const_euler(mpfr_class::default_precision());
}

inline mpfr_class const_catalan(mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_const_catalan(result.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

inline mpfr_class const_catalan()
{
    return const_catalan(mpfr_class::default_precision());
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class rec_sqrt(const Expr& expr)
{
    return detail::unary_mpfr_math(expr, [](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) {
        mpfr_rec_sqrt(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class neg(const Expr& expr)
{
    return detail::unary_mpfr_math(expr, [](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) {
        mpfr_neg(rop, op, rnd);
    });
}

#define GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>, \
                         int> = 0> \
    inline mpfr_class name(const Expr& expr) \
    { \
        return detail::unary_mpfr_math(expr, [](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) { \
            mpfr_##name(rop, op, rnd); \
        }); \
    }

GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(abs)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(sin)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(cos)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(tan)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(asin)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(acos)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(atan)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(sinh)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(cosh)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(tanh)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(asinh)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(acosh)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(atanh)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(sec)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(csc)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(cot)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(sech)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(csch)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(coth)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(sinpi)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(cospi)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(tanpi)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(asinpi)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(acospi)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(atanpi)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(cbrt)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(log)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(log2)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(log10)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(log1p)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(log2p1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(log10p1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(exp)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(exp2)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(exp10)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(expm1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(exp2m1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(exp10m1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(eint)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(li2)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(erf)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(erfc)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(gamma)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(lngamma)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(digamma)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(zeta)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(j0)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(j1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(y0)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(y1)
GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION(ai)

#undef GMPFRXX_MKII_DEFINE_MPFR_UNARY_FUNCTION

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class reciprocal_gamma(const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class gamma_value = mpfr_class::with_precision(operand.precision());
    mpfr_gamma(gamma_value.mpfr_data(), operand.mpfr_data(), mpfr_class::default_rounding());
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_ui_div(result.mpfr_data(), 1ul, gamma_value.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

#define GMPFRXX_MKII_DEFINE_MPFR_ROUND_TO_INTEGER_FUNCTION(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>, \
                         int> = 0> \
    inline mpfr_class name(const Expr& expr) \
    { \
        return detail::unary_mpfr_math(expr, [](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t) { \
            mpfr_##name(rop, op); \
        }); \
    }

GMPFRXX_MKII_DEFINE_MPFR_ROUND_TO_INTEGER_FUNCTION(ceil)
GMPFRXX_MKII_DEFINE_MPFR_ROUND_TO_INTEGER_FUNCTION(floor)
GMPFRXX_MKII_DEFINE_MPFR_ROUND_TO_INTEGER_FUNCTION(trunc)

#undef GMPFRXX_MKII_DEFINE_MPFR_ROUND_TO_INTEGER_FUNCTION

#define GMPFRXX_MKII_DEFINE_MPFR_SCALE_UI_FUNCTION(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>, \
                         int> = 0> \
    inline mpfr_class name(const Expr& expr, unsigned long exponent) \
    { \
        return detail::unary_mpfr_math(expr, [exponent](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) { \
            mpfr_##name(rop, op, exponent, rnd); \
        }); \
    }

GMPFRXX_MKII_DEFINE_MPFR_SCALE_UI_FUNCTION(mul_2ui)
GMPFRXX_MKII_DEFINE_MPFR_SCALE_UI_FUNCTION(div_2ui)

#undef GMPFRXX_MKII_DEFINE_MPFR_SCALE_UI_FUNCTION

#define GMPFRXX_MKII_DEFINE_MPFR_SCALE_SI_FUNCTION(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>, \
                         int> = 0> \
    inline mpfr_class name(const Expr& expr, long exponent) \
    { \
        return detail::unary_mpfr_math(expr, [exponent](mpfr_t rop, mpfr_srcptr op, mpfr_rnd_t rnd) { \
            mpfr_##name(rop, op, exponent, rnd); \
        }); \
    }

GMPFRXX_MKII_DEFINE_MPFR_SCALE_SI_FUNCTION(mul_2si)
GMPFRXX_MKII_DEFINE_MPFR_SCALE_SI_FUNCTION(div_2si)

#undef GMPFRXX_MKII_DEFINE_MPFR_SCALE_SI_FUNCTION

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline std::pair<mpfr_class, int> lgamma(const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    int sign = 0;
    mpfr_lgamma(result.mpfr_data(), &sign, operand.mpfr_data(), mpfr_class::default_rounding());
    return {std::move(result), sign};
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline bool signbit(const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_unary_value(expr);
    return mpfr_signbit(operand.mpfr_data()) != 0;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline int sgn(const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_unary_value(expr);
    return mpfr_sgn(operand.mpfr_data());
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class setsign(const Expr& expr, int sign)
{
    const mpfr_class operand = detail::materialize_mpfr_unary_value(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_setsign(result.mpfr_data(), operand.mpfr_data(), sign, mpfr_class::default_rounding());
    return result;
}

template <
    typename Value,
    typename Sign,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Value> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Sign> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Value> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Sign>),
                     int> = 0>
inline mpfr_class copysign(const Value& value, const Sign& sign_source)
{
    const mpfr_class value_operand = detail::materialize_mpfr_math_operand(value);
    const mpfr_class sign_operand = detail::materialize_mpfr_math_operand(sign_source);
    mpfr_class result = mpfr_class::with_precision(value_operand.precision());
    mpfr_copysign(result.mpfr_data(),
                  value_operand.mpfr_data(),
                  sign_operand.mpfr_data(),
                  mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class nextabove(const Expr& expr)
{
    mpfr_class result = detail::materialize_mpfr_unary_value(expr);
    mpfr_nextabove(result.mpfr_data());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class nextbelow(const Expr& expr)
{
    mpfr_class result = detail::materialize_mpfr_unary_value(expr);
    mpfr_nextbelow(result.mpfr_data());
    return result;
}

template <
    typename Expr,
    typename Direction,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Direction> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Direction>),
                     int> = 0>
inline mpfr_class nexttoward(const Expr& expr, const Direction& direction)
{
    mpfr_class result = detail::materialize_mpfr_math_operand(expr);
    const mpfr_class target = detail::materialize_mpfr_math_operand(direction);
    mpfr_nexttoward(result.mpfr_data(), target.mpfr_data());
    return result;
}

template <
    typename Expr,
    typename Direction,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Direction> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Direction>),
                     int> = 0>
inline mpfr_class nextafter(const Expr& expr, const Direction& direction)
{
    return nexttoward(expr, direction);
}

#define GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>, \
                         int> = 0> \
    inline bool name(const Expr& expr) \
    { \
        const mpfr_class operand = detail::materialize_mpfr_unary_value(expr); \
        return mpfr_##name(operand.mpfr_data()) != 0; \
    }

GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(nan_p)
GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(inf_p)
GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(number_p)
GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(integer_p)
GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(zero_p)
GMPFRXX_MKII_DEFINE_MPFR_PREDICATE(regular_p)

#undef GMPFRXX_MKII_DEFINE_MPFR_PREDICATE

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline int cmpabs(const Lhs& lhs, const Rhs& rhs)
{
    const auto operands = detail::materialize_mpfr_binary_values(lhs, rhs);
    return mpfr_cmpabs(operands.first.mpfr_data(), operands.second.mpfr_data());
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline int cmpabs_ui(const Expr& expr, unsigned long value)
{
    const mpfr_class operand = detail::materialize_mpfr_unary_value(expr);
    return mpfr_cmpabs_ui(operand.mpfr_data(), value);
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class reldiff(const Lhs& lhs, const Rhs& rhs)
{
    const auto operands = detail::materialize_mpfr_binary_values(lhs, rhs);
    mpfr_class result = mpfr_class::with_precision(operands.first.precision());
    mpfr_reldiff(result.mpfr_data(),
                 operands.first.mpfr_data(),
                 operands.second.mpfr_data(),
                 mpfr_class::default_rounding());
    return result;
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline bool eq(const Lhs& lhs, const Rhs& rhs, unsigned long bits)
{
    const auto operands = detail::materialize_mpfr_binary_values(lhs, rhs);
    return mpfr_eq(operands.first.mpfr_data(), operands.second.mpfr_data(), bits) != 0;
}

#define GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(name) \
    template < \
        typename Lhs, \
        typename Rhs, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> && \
                             gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> && \
                             (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> || \
                              gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>), \
                         int> = 0> \
    inline bool name(const Lhs& lhs, const Rhs& rhs) \
    { \
        const auto operands = detail::materialize_mpfr_binary_values(lhs, rhs); \
        return mpfr_##name(operands.first.mpfr_data(), operands.second.mpfr_data()) != 0; \
    }

GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(greater_p)
GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(greaterequal_p)
GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(less_p)
GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(lessequal_p)
GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(lessgreater_p)
GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(equal_p)
GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE(unordered_p)

#undef GMPFRXX_MKII_DEFINE_MPFR_BINARY_PREDICATE

inline mpfr_class log_ui(unsigned long value, mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_log_ui(result.mpfr_data(), value, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class log_ui(unsigned long value)
{
    return log_ui(value, mpfr_class::default_precision());
}

inline mpfr_class zeta_ui(unsigned long value, mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_zeta_ui(result.mpfr_data(), value, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class zeta_ui(unsigned long value)
{
    return zeta_ui(value, mpfr_class::default_precision());
}

inline mpfr_class fac_ui(unsigned long value, mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_fac_ui(result.mpfr_data(), value, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class fac_ui(unsigned long value)
{
    return fac_ui(value, mpfr_class::default_precision());
}

inline mpfr_class ui_pow_ui(unsigned long base, unsigned long exponent, mpfr_prec_t precision)
{
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_ui_pow_ui(result.mpfr_data(), base, exponent, mpfr_class::default_rounding());
    return result;
}

inline mpfr_class ui_pow_ui(unsigned long base, unsigned long exponent)
{
    return ui_pow_ui(base, exponent, mpfr_class::default_precision());
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class pow_si(const Expr& expr, long exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_pow_si(result.mpfr_data(), operand.mpfr_data(), exponent, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class pow_ui(const Expr& expr, unsigned long exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_pow_ui(result.mpfr_data(), operand.mpfr_data(), exponent, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class pow_sj(const Expr& expr, intmax_t exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_pow_sj(result.mpfr_data(), operand.mpfr_data(), exponent, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class pow_uj(const Expr& expr, uintmax_t exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_pow_uj(result.mpfr_data(), operand.mpfr_data(), exponent, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class pow_z(const Expr& expr, const ::gmpxx::mpz_class& exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_pow_z(result.mpfr_data(), operand.mpfr_data(), exponent.mpz_data(), mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class ui_pow(unsigned long base, const Expr& exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(exponent);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_ui_pow(result.mpfr_data(), base, operand.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class compound_si(const Expr& expr, long exponent)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_compound_si(result.mpfr_data(), operand.mpfr_data(), exponent, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class rootn_ui(const Expr& expr, unsigned long root_index)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_rootn_ui(result.mpfr_data(), operand.mpfr_data(), root_index, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class rootn_si(const Expr& expr, long root_index)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_rootn_si(result.mpfr_data(), operand.mpfr_data(), root_index, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class jn(long order, const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_jn(result.mpfr_data(), order, operand.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class yn(long order, const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_yn(result.mpfr_data(), order, operand.mpfr_data(), mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class sinu(const Expr& expr, unsigned long unit)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_sinu(result.mpfr_data(), operand.mpfr_data(), unit, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class cosu(const Expr& expr, unsigned long unit)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_cosu(result.mpfr_data(), operand.mpfr_data(), unit, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class tanu(const Expr& expr, unsigned long unit)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_tanu(result.mpfr_data(), operand.mpfr_data(), unit, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class asinu(const Expr& expr, unsigned long unit)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_asinu(result.mpfr_data(), operand.mpfr_data(), unit, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class acosu(const Expr& expr, unsigned long unit)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_acosu(result.mpfr_data(), operand.mpfr_data(), unit, mpfr_class::default_rounding());
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class atanu(const Expr& expr, unsigned long unit)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.precision());
    mpfr_atanu(result.mpfr_data(), operand.mpfr_data(), unit, mpfr_class::default_rounding());
    return result;
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class agm(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_agm(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class pow(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_pow(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class powr(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_powr(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class min(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_min(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class max(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_max(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class dim(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_dim(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class hypot(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_hypot(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class remainder(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_remainder(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class fmod(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_fmod(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline std::pair<mpfr_class, long> remquo(const Lhs& lhs, const Rhs& rhs)
{
    const mpfr_class left = detail::materialize_mpfr_math_operand(lhs);
    const mpfr_class right = detail::materialize_mpfr_math_operand(rhs);
    const mpfr_prec_t precision = std::max(left.precision(), right.precision());
    mpfr_class result = mpfr_class::with_precision(precision);
    long quotient = 0;
    mpfr_remquo(result.mpfr_data(),
                &quotient,
                left.mpfr_data(),
                right.mpfr_data(),
                mpfr_class::default_rounding());
    return {std::move(result), quotient};
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class gamma_inc(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_gamma_inc(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Rhs>),
                     int> = 0>
inline mpfr_class beta(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpfr_math(lhs, rhs, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_beta(rop, op1, op2, rnd);
    });
}

template <
    typename Y,
    typename X,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Y> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<X> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Y> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<X>),
                     int> = 0>
inline mpfr_class atan2(const Y& y, const X& x)
{
    return detail::binary_mpfr_math(y, x, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_atan2(rop, op1, op2, rnd);
    });
}

template <
    typename Y,
    typename X,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Y> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<X> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Y> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<X>),
                     int> = 0>
inline mpfr_class atan2pi(const Y& y, const X& x)
{
    return detail::binary_mpfr_math(y, x, [](mpfr_t rop, mpfr_srcptr op1, mpfr_srcptr op2, mpfr_rnd_t rnd) {
        mpfr_atan2pi(rop, op1, op2, rnd);
    });
}

template <
    typename Y,
    typename X,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Y> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<X> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Y> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<X>),
                     int> = 0>
inline mpfr_class atan2u(const Y& y, const X& x, unsigned long unit)
{
    const mpfr_class left = detail::materialize_mpfr_math_operand(y);
    const mpfr_class right = detail::materialize_mpfr_math_operand(x);
    const mpfr_prec_t precision = std::max(left.precision(), right.precision());
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_atan2u(result.mpfr_data(),
                left.mpfr_data(),
                right.mpfr_data(),
                unit,
                mpfr_class::default_rounding());
    return result;
}

template <
    typename A,
    typename B,
    typename C,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<A> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<B> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<C> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<A> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<B> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<C>),
                     int> = 0>
inline mpfr_class fma(const A& a, const B& b, const C& c)
{
    return detail::ternary_mpfr_math(a, b, c, [](mpfr_t rop,
                                                 mpfr_srcptr op1,
                                                 mpfr_srcptr op2,
                                                 mpfr_srcptr op3,
                                                 mpfr_rnd_t rnd) {
        mpfr_fma(rop, op1, op2, op3, rnd);
    });
}

template <
    typename A,
    typename B,
    typename C,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<A> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<B> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<C> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<A> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<B> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<C>),
                     int> = 0>
inline mpfr_class fms(const A& a, const B& b, const C& c)
{
    return detail::ternary_mpfr_math(a, b, c, [](mpfr_t rop,
                                                 mpfr_srcptr op1,
                                                 mpfr_srcptr op2,
                                                 mpfr_srcptr op3,
                                                 mpfr_rnd_t rnd) {
        mpfr_fms(rop, op1, op2, op3, rnd);
    });
}

template <
    typename A,
    typename B,
    typename C,
    typename D,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<A> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<B> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<C> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<D> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<A> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<B> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<C> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<D>),
                     int> = 0>
inline mpfr_class fmma(const A& a, const B& b, const C& c, const D& d)
{
    return detail::quaternary_mpfr_math(a, b, c, d, [](mpfr_t rop,
                                                       mpfr_srcptr op1,
                                                       mpfr_srcptr op2,
                                                       mpfr_srcptr op3,
                                                       mpfr_srcptr op4,
                                                       mpfr_rnd_t rnd) {
        mpfr_fmma(rop, op1, op2, op3, op4, rnd);
    });
}

template <
    typename A,
    typename B,
    typename C,
    typename D,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<A> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<B> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<C> &&
                         gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<D> &&
                         (gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<A> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<B> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<C> ||
                          gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<D>),
                     int> = 0>
inline mpfr_class fmms(const A& a, const B& b, const C& c, const D& d)
{
    return detail::quaternary_mpfr_math(a, b, c, d, [](mpfr_t rop,
                                                       mpfr_srcptr op1,
                                                       mpfr_srcptr op2,
                                                       mpfr_srcptr op3,
                                                       mpfr_srcptr op4,
                                                       mpfr_rnd_t rnd) {
        mpfr_fmms(rop, op1, op2, op3, op4, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline std::pair<mpfr_class, mpfr_class> sin_cos(const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class sine = mpfr_class::with_precision(operand.precision());
    mpfr_class cosine = mpfr_class::with_precision(operand.precision());
    mpfr_sin_cos(sine.mpfr_data(), cosine.mpfr_data(), operand.mpfr_data(), mpfr_class::default_rounding());
    return {std::move(sine), std::move(cosine)};
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpfr_object_or_node_v<Expr>,
                     int> = 0>
inline std::pair<mpfr_class, mpfr_class> sinh_cosh(const Expr& expr)
{
    const mpfr_class operand = detail::materialize_mpfr_math_operand(expr);
    mpfr_class hyperbolic_sine = mpfr_class::with_precision(operand.precision());
    mpfr_class hyperbolic_cosine = mpfr_class::with_precision(operand.precision());
    mpfr_sinh_cosh(hyperbolic_sine.mpfr_data(),
                   hyperbolic_cosine.mpfr_data(),
                   operand.mpfr_data(),
                   mpfr_class::default_rounding());
    return {std::move(hyperbolic_sine), std::move(hyperbolic_cosine)};
}

namespace literals {

inline mpfr_class operator"" _mpfr(long double value)
{
    return mpfr_class(static_cast<double>(value));
}

inline mpfr_class operator"" _mpfr(const char* value)
{
    return mpfr_class(value, mpfr_class::default_precision(), 0);
}

inline mpfr_class operator"" _mpfr(const char* value, std::size_t)
{
    return mpfr_class(value, mpfr_class::default_precision(), 0);
}

} // namespace literals

using literals::operator"" _mpfr;

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
