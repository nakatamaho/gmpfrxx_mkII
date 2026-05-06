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

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/eval_context.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <ios>
#include <istream>
#include <limits>
#include <locale>
#include <ostream>
#include <stdexcept>
#include <string>
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
        const auto context = gmpfrxx_mkII::detail::current_eval_context(other.precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, other.value_, context.rounding_mode);
    }

    mpfr_class(mpfr_class&& other) noexcept
    {
        mpfr_init2(value_, other.precision());
        mpfr_swap(value_, other.value_);
    }

    explicit mpfr_class(double value) : mpfr_class(value, default_precision()) {}

    mpfr_class(double value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_d(value_, value, context.rounding_mode);
    }

    explicit mpfr_class(const gmpxx::mpz_class& value) : mpfr_class(value, default_precision()) {}

    explicit mpfr_class(const gmpxx::mpq_class& value) : mpfr_class(value, default_precision()) {}

    mpfr_class(const gmpxx::mpz_class& value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_z(value_, value.mpz_data(), context.rounding_mode);
    }

    mpfr_class(const gmpxx::mpq_class& value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_q(value_, value.mpq_data(), context.rounding_mode);
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    explicit mpfr_class(T value) : mpfr_class(value, default_precision())
    {
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    mpfr_class(T value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        set_integral(value);
    }

    mpfr_class(bool) = delete;
    mpfr_class(bool, mpfr_prec_t) = delete;

    explicit mpfr_class(const char* text) : mpfr_class(text, default_precision(), 10) {}

    explicit mpfr_class(const std::string& text) : mpfr_class(text.c_str(), default_precision(), 10) {}

    mpfr_class(const char* text, mpfr_prec_t precision, int base = 10)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        if (text == nullptr || mpfr_set_str(value_, text, base, context.rounding_mode) != 0) {
            mpfr_clear(value_);
            throw std::invalid_argument("invalid mpfr_class decimal string");
        }
    }

    mpfr_class(const std::string& text, mpfr_prec_t precision, int base = 10)
        : mpfr_class(text.c_str(), precision, base)
    {
    }

    explicit mpfr_class(mpfr_srcptr value)
    {
        const mpfr_prec_t precision = mpfr_get_prec(value);
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, value, context.rounding_mode);
    }

    mpfr_class(mpfr_srcptr value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, value, context.rounding_mode);
    }

    mpfr_class(const mpfr_class& other, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, other.value_, context.rounding_mode);
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
    mpfr_class(const Expr& expr);

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>,
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
            const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
            mpfr_set(value_, other.value_, context.rounding_mode);
        }
        return *this;
    }

    mpfr_class& operator=(mpfr_class&& other) noexcept
    {
        if (this != &other) {
            if (this->precision() == other.precision()) {
                mpfr_swap(value_, other.value_);
            } else {
                const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
                const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
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
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_z(value_, value.mpz_data(), context.rounding_mode);
        return *this;
    }

    mpfr_class& operator=(const gmpxx::mpq_class& value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
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
        if (set_str(text) != 0) {
            throw std::invalid_argument("invalid mpfr_class decimal string");
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
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
    mpfr_class& operator=(const Expr& expr);

    static mpfr_class with_precision(mpfr_prec_t precision, double value)
    {
        mpfr_class result = with_precision(precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
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

    void set_prec(mpfr_prec_t precision)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_prec_round(value_, precision, context.rounding_mode);
    }

    explicit operator bool() const noexcept
    {
        return mpfr_zero_p(value_) == 0;
    }

    void set(double value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
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
        if (set_str(text) != 0) {
            throw std::invalid_argument("invalid mpfr_class decimal string");
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
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        const int rc = mpfr_set_str(temp, text, base, context.rounding_mode);
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
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_ui(value_, 0, context.rounding_mode);
    }

    template <typename T>
    void set_integral(T value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        const gmpxx::mpz_class integer(value);
        mpfr_set_z(value_, integer.mpz_data(), context.rounding_mode);
    }

    mpfr_t value_;
};

inline void swap(mpfr_class& lhs, mpfr_class& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace mpfrxx

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

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

inline std::ostream& operator<<(std::ostream& out, const mpfr_class& value)
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

        std::string format = (out.flags() & std::ios_base::showpoint) ? "%#.*R" : "%.*R";
        format.push_back(conversion);

        std::streamsize stream_precision = out.precision();
        if (stream_precision < 0) {
            stream_precision = 6;
        }
        if (stream_precision > static_cast<std::streamsize>(std::numeric_limits<int>::max())) {
            stream_precision = std::numeric_limits<int>::max();
        }

        char* raw = nullptr;
        const int count = mpfr_asprintf(
            &raw, format.c_str(), static_cast<int>(stream_precision), value.mpfr_data());
        gmpfrxx_mkII::detail::mpfr_allocated_string formatted(raw);
        if (count < 0 || !formatted) {
            out.setstate(std::ios_base::badbit);
            return out;
        }

        std::string text(formatted.c_str());
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
                const std::size_t pos = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1 : 0;
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
        gmp_randclass& state,
        random_mpfr_distribution distribution,
        mpfr_prec_t requested_precision,
        bool has_requested_precision) noexcept
        : state_(&state),
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
    gmp_randclass* state_;
    random_mpfr_distribution distribution_;
    mpfr_prec_t requested_precision_;
    bool has_requested_precision_;
};

class gmp_randclass {
public:
    gmp_randclass()
    {
        gmp_randinit_default(state_);
    }

    explicit gmp_randclass(void (*randinit)(gmp_randstate_t))
    {
        randinit(state_);
    }

    gmp_randclass(int (*randinit)(gmp_randstate_t, mp_bitcnt_t), mp_bitcnt_t size)
    {
        if (randinit(state_, size) == 0) {
            throw std::length_error("gmp_randinit_lc_2exp_size failed");
        }
    }

    gmp_randclass(
        void (*randinit)(gmp_randstate_t, const mpz_t, unsigned long, mp_bitcnt_t),
        const gmpxx::mpz_class& a,
        unsigned long c,
        mp_bitcnt_t m2exp)
    {
        randinit(state_, a.mpz_data(), c, m2exp);
    }

    gmp_randclass(gmp_randalg_t alg, mp_bitcnt_t size)
    {
        if (alg == GMP_RAND_ALG_DEFAULT || alg == GMP_RAND_ALG_LC ||
            alg == static_cast<gmp_randalg_t>(0)) {
            if (gmp_randinit_lc_2exp_size(state_, size) == 0) {
                throw std::length_error("gmp_randinit_lc_2exp_size failed");
            }
        } else {
            throw std::invalid_argument("unsupported GMP random algorithm");
        }
    }

    ~gmp_randclass()
    {
        gmp_randclear(state_);
    }

    gmp_randclass(const gmp_randclass&) = delete;
    gmp_randclass& operator=(const gmp_randclass&) = delete;
    gmp_randclass(gmp_randclass&&) = delete;
    gmp_randclass& operator=(gmp_randclass&&) = delete;

    void seed(unsigned long value)
    {
        gmp_randseed_ui(state_, value);
    }

    void seed(const gmpxx::mpz_class& value)
    {
        gmp_randseed(state_, value.mpz_data());
    }

    gmpxx::mpz_class get_z_bits(mp_bitcnt_t bits)
    {
        gmpxx::mpz_class result;
        mpz_urandomb(result.mpz_data(), state_, bits);
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
        mpz_urandomm(result.mpz_data(), state_, limit.mpz_data());
        return result;
    }

    random_mpfr_expr get_fr() noexcept
    {
        return random_mpfr_expr(*this, random_mpfr_distribution::urandomb, 0, false);
    }

    random_mpfr_expr get_fr(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(*this, random_mpfr_distribution::urandomb, precision, true);
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
        return random_mpfr_expr(*this, random_mpfr_distribution::uniform, 0, false);
    }

    random_mpfr_expr get_fr_uniform(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(*this, random_mpfr_distribution::uniform, precision, true);
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
        return random_mpfr_expr(*this, random_mpfr_distribution::normal, 0, false);
    }

    random_mpfr_expr get_fr_normal(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(*this, random_mpfr_distribution::normal, precision, true);
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
        return random_mpfr_expr(*this, random_mpfr_distribution::exponential, 0, false);
    }

    random_mpfr_expr get_fr_exponential(mpfr_prec_t precision) noexcept
    {
        return random_mpfr_expr(*this, random_mpfr_distribution::exponential, precision, true);
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
    friend class random_mpfr_expr;

    gmp_randstate_t state_;
};

inline void random_mpfr_expr::generate(mpfr_t dest, mpfr_rnd_t rnd) const
{
    switch (distribution_) {
    case random_mpfr_distribution::urandomb:
        if (mpfr_urandomb(dest, state_->state_) != 0) {
            throw std::runtime_error("mpfr_urandomb failed");
        }
        break;
    case random_mpfr_distribution::uniform:
        mpfr_urandom(dest, state_->state_, rnd);
        break;
    case random_mpfr_distribution::normal:
        mpfr_nrandom(dest, state_->state_, rnd);
        break;
    case random_mpfr_distribution::exponential:
        mpfr_erandom(dest, state_->state_, rnd);
        break;
    }
}

} // namespace mpfrxx

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

inline object_leaf<mpfrxx::mpfr_class> make_mpfr_operand(const mpfrxx::mpfr_class& value)
{
    return object_leaf<mpfrxx::mpfr_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpfr_operand(const gmpxx::mpz_class& value)
{
    return object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpfr_operand(const gmpxx::mpq_class& value)
{
    return object_leaf<gmpxx::mpq_class>(value);
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

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<gmpxx::mpq_class>&)
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
    const object_leaf<gmpxx::mpq_class>& expr,
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

inline bool mpfr_expression_references(const mpfr_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpfr_expression_references(const mpfr_t, const object_leaf<gmpxx::mpq_class>&)
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
    mpfr_init2(temp, eval_precision);
    mpfr_evaluate(temp, expr, eval_precision, rnd);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        mpz_t exact;
        mpz_init(exact);
        mpz_evaluate(exact, expr);
        mpfr_set_z(dest, exact, rnd);
        mpz_clear(exact);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        mpq_t exact;
        mpq_init(exact);
        mpq_evaluate(exact, expr);
        mpfr_set_q(dest, exact, rnd);
        mpq_clear(exact);
        return;
    }

    if (mpfr_expression_references(dest, expr)) {
        mpfr_t lhs;
        mpfr_t rhs;
        mpfr_evaluate_to_temporary(lhs, expr.lhs(), eval_precision, rnd);
        mpfr_evaluate_to_temporary(rhs, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, lhs, rhs, rnd);
        mpfr_clear(rhs);
        mpfr_clear(lhs);
        return;
    }

    if constexpr (std::is_same_v<Lhs, object_leaf<mpfrxx::mpfr_class>> &&
                  std::is_same_v<Rhs, object_leaf<mpfrxx::mpfr_class>>) {
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
    } else if constexpr (std::is_same_v<Rhs, object_leaf<mpfrxx::mpfr_class>>) {
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, dest, expr.rhs().get().mpfr_data(), rnd);
    } else if constexpr (std::is_same_v<Lhs, object_leaf<mpfrxx::mpfr_class>> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpfr_evaluate(dest, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), dest, rnd);
    } else {
        mpfr_t rhs;
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpfr_evaluate_to_temporary(rhs, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, dest, rhs, rnd);
        mpfr_clear(rhs);
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

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

template <typename Expr, typename>
mpfr_class::mpfr_class(const Expr& expr)
{
    const mpfr_prec_t precision = gmpfrxx_mkII::detail::mpfr_expression_precision(expr);
    mpfr_init2(value_, precision);
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
}

template <typename Expr, typename, typename>
mpfr_class::mpfr_class(const Expr& expr, mpfr_prec_t precision)
{
    mpfr_init2(value_, precision);
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
}

template <typename Expr, typename>
mpfr_class& mpfr_class::operator=(const Expr& expr)
{
    const mpfr_prec_t precision = this->precision();
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

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

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline int cmp(Lhs&& lhs, Rhs&& rhs)
{
    auto left = gmpfrxx_mkII::detail::make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = gmpfrxx_mkII::detail::make_mpfr_operand(std::forward<Rhs>(rhs));
    mpfr_prec_t precision = std::max(
        gmpfrxx_mkII::detail::mpfr_expression_precision(left),
        gmpfrxx_mkII::detail::mpfr_expression_precision(right));
    if (precision == 0) {
        precision = mpfr_class::default_precision();
    }

    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);

    mpfr_t lhs_value;
    mpfr_t rhs_value;
    mpfr_init2(lhs_value, precision);
    mpfr_init2(rhs_value, precision);
    gmpfrxx_mkII::detail::mpfr_evaluate(lhs_value, left, precision, context.rounding_mode);
    gmpfrxx_mkII::detail::mpfr_evaluate(rhs_value, right, precision, context.rounding_mode);
    const int result = mpfr_cmp(lhs_value, rhs_value);
    mpfr_clear(rhs_value);
    mpfr_clear(lhs_value);
    return result;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator==(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) == 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator!=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) != 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator<(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) < 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator<=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) <= 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator>(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) > 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpfr_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator>=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) >= 0;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator+=(mpfr_class& lhs, Rhs&& rhs)
{
    lhs = lhs + std::forward<Rhs>(rhs);
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator-=(mpfr_class& lhs, Rhs&& rhs)
{
    lhs = lhs - std::forward<Rhs>(rhs);
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator*=(mpfr_class& lhs, Rhs&& rhs)
{
    lhs = lhs * std::forward<Rhs>(rhs);
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<Rhs>, int> = 0>
inline mpfr_class& operator/=(mpfr_class& lhs, Rhs&& rhs)
{
    lhs = lhs / std::forward<Rhs>(rhs);
    return lhs;
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

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
