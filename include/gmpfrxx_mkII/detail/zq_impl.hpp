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

#ifndef GMPFRXX_MKII_DETAIL_ZQ_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_ZQ_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>

#include <gmp.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <istream>
#include <locale>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace gmpfrxx_mkII {
namespace detail {

class gmp_allocated_string {
public:
    explicit gmp_allocated_string(char* value) noexcept : value_(value) {}

    ~gmp_allocated_string()
    {
        reset();
    }

    gmp_allocated_string(const gmp_allocated_string&) = delete;
    gmp_allocated_string& operator=(const gmp_allocated_string&) = delete;

    gmp_allocated_string(gmp_allocated_string&& other) noexcept : value_(other.value_)
    {
        other.value_ = nullptr;
    }

    gmp_allocated_string& operator=(gmp_allocated_string&& other) noexcept
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
        if (value_ == nullptr) {
            return;
        }
        void (*free_function)(void*, std::size_t) = nullptr;
        mp_get_memory_functions(nullptr, nullptr, &free_function);
        free_function(value_, std::strlen(value_) + 1);
        value_ = nullptr;
    }

    char* value_;
};

inline int gmp_stream_input_base(const std::ios_base& stream) noexcept
{
    const auto basefield = stream.flags() & std::ios_base::basefield;
    if (basefield == std::ios_base::hex) {
        return 16;
    }
    if (basefield == std::ios_base::oct) {
        return 8;
    }
    if (basefield == std::ios_base::dec) {
        return 10;
    }
    return 0;
}

inline int gmp_digit_value(char c) noexcept
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

inline bool gmp_is_digit_for_base(char c, int base) noexcept
{
    const int value = gmp_digit_value(c);
    return value >= 0 && value < base;
}

inline bool gmp_stream_peek_char(std::istream& in, char& c)
{
    const auto next = in.rdbuf()->sgetc();
    if (next == std::char_traits<char>::eof()) {
        in.setstate(std::ios_base::eofbit);
        return false;
    }
    c = static_cast<char>(next);
    return true;
}

inline void gmp_stream_get_char(std::istream& in, std::string& token)
{
    const auto next = in.rdbuf()->sbumpc();
    if (next == std::char_traits<char>::eof()) {
        in.setstate(std::ios_base::eofbit | std::ios_base::failbit);
        return;
    }
    token.push_back(static_cast<char>(next));
}

inline void gmp_read_optional_sign(std::istream& in, std::string& token)
{
    char c = '\0';
    if (gmp_stream_peek_char(in, c) && (c == '+' || c == '-')) {
        gmp_stream_get_char(in, token);
    }
}

inline bool gmp_read_digits_for_base(std::istream& in, std::string& token, int base)
{
    bool saw_digit = false;
    char c = '\0';
    while (gmp_stream_peek_char(in, c) && gmp_is_digit_for_base(c, base)) {
        gmp_stream_get_char(in, token);
        saw_digit = true;
    }
    return saw_digit;
}

inline bool gmp_read_integer_token(std::istream& in, std::string& token, int base)
{
    token.clear();
    gmp_read_optional_sign(in, token);

    if (base != 0) {
        return gmp_read_digits_for_base(in, token, base);
    }

    char c = '\0';
    if (!gmp_stream_peek_char(in, c) || !std::isdigit(static_cast<unsigned char>(c))) {
        return false;
    }

    if (c != '0') {
        return gmp_read_digits_for_base(in, token, 10);
    }

    gmp_stream_get_char(in, token);
    char prefix = '\0';
    if (gmp_stream_peek_char(in, prefix) && (prefix == 'x' || prefix == 'X')) {
        in.rdbuf()->sbumpc();
        char first_hex = '\0';
        if (gmp_stream_peek_char(in, first_hex) && gmp_is_digit_for_base(first_hex, 16)) {
            token.push_back(prefix);
            gmp_read_digits_for_base(in, token, 16);
            return true;
        }
        if (in.rdbuf()->sputbackc(prefix) == std::char_traits<char>::eof()) {
            in.setstate(std::ios_base::badbit);
        }
    }

    gmp_read_digits_for_base(in, token, 8);
    return true;
}

inline bool gmp_read_rational_token(std::istream& in, std::string& token, int base)
{
    token.clear();
    std::string numerator;
    if (!gmp_read_integer_token(in, numerator, base)) {
        return false;
    }
    token = numerator;

    char c = '\0';
    if (!gmp_stream_peek_char(in, c) || c != '/') {
        return true;
    }

    gmp_stream_get_char(in, token);
    std::string denominator;
    if (!gmp_read_integer_token(in, denominator, base)) {
        return false;
    }
    token += denominator;
    return true;
}

inline char gmp_stream_decimal_point(const std::ios_base& stream)
{
    return std::use_facet<std::numpunct<char>>(stream.getloc()).decimal_point();
}

inline bool gmp_read_float_token(std::istream& in, std::string& token, int base)
{
    token.clear();
    gmp_read_optional_sign(in, token);
    const char decimal_point = gmp_stream_decimal_point(in);

    bool saw_mantissa_digit = false;
    saw_mantissa_digit |= gmp_read_digits_for_base(in, token, base);

    char c = '\0';
    if (gmp_stream_peek_char(in, c) && c == decimal_point) {
        in.rdbuf()->sbumpc();
        token.push_back('.');
        saw_mantissa_digit |= gmp_read_digits_for_base(in, token, base);
    }

    if (!saw_mantissa_digit) {
        return false;
    }

    const char exponent_marker = base == 10 ? 'e' : '@';
    if (!gmp_stream_peek_char(in, c) ||
        (c != exponent_marker && !(base == 10 && c == 'E'))) {
        return true;
    }

    gmp_stream_get_char(in, token);
    gmp_read_optional_sign(in, token);

    bool saw_exponent_digit = false;
    while (gmp_stream_peek_char(in, c) && std::isdigit(static_cast<unsigned char>(c))) {
        gmp_stream_get_char(in, token);
        saw_exponent_digit = true;
    }
    return saw_exponent_digit;
}

inline std::string gmp_strip_leading_plus(std::string token)
{
    if (!token.empty() && token.front() == '+') {
        token.erase(token.begin());
    }
    const std::size_t slash = token.find('/');
    if (slash != std::string::npos && slash + 1 < token.size() && token[slash + 1] == '+') {
        token.erase(token.begin() + static_cast<std::ptrdiff_t>(slash + 1));
    }
    return token;
}

} // namespace detail
} // namespace gmpfrxx_mkII

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

    explicit mpz_class(const char* value, int base = 10)
    {
        mpz_init(value_);
        if (value == nullptr || mpz_set_str(value_, value, base) != 0) {
            mpz_clear(value_);
            throw std::invalid_argument("invalid mpz_class string");
        }
    }

    explicit mpz_class(const std::string& value, int base = 10)
        : mpz_class(value.c_str(), base)
    {
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

    const mpz_t& get_mpz_t() const noexcept
    {
        return value_;
    }

    mpz_t& get_mpz_t() noexcept
    {
        return value_;
    }

    void swap(mpz_class& other) noexcept
    {
        mpz_swap(value_, other.value_);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    mpz_class& operator=(T value)
    {
        set_integral(value);
        return *this;
    }

    std::string get_str(int base = 10) const
    {
        gmpfrxx_mkII::detail::gmp_allocated_string raw(mpz_get_str(nullptr, base, value_));
        if (!raw) {
            throw std::runtime_error("mpz_get_str failed");
        }
        return std::string(raw.c_str());
    }

    std::string to_string(int base = 10) const
    {
        return get_str(base);
    }

    int set_str(const char* value, int base = 10)
    {
        if (value == nullptr) {
            return -1;
        }
        mpz_t temp;
        mpz_init(temp);
        const int rc = mpz_set_str(temp, value, base);
        if (rc == 0) {
            mpz_swap(value_, temp);
        }
        mpz_clear(temp);
        return rc;
    }

    int set_str(const std::string& value, int base = 10)
    {
        return set_str(value.c_str(), base);
    }

    void set(const char* value, int base = 10)
    {
        if (set_str(value, base) != 0) {
            throw std::invalid_argument("invalid mpz_class string");
        }
    }

    void set(const std::string& value, int base = 10)
    {
        set(value.c_str(), base);
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

    explicit mpq_class(const char* value, int base = 10)
    {
        mpq_init(value_);
        if (value == nullptr || mpq_set_str(value_, value, base) != 0) {
            mpq_clear(value_);
            throw std::invalid_argument("invalid mpq_class string");
        }
        mpq_canonicalize(value_);
    }

    explicit mpq_class(const std::string& value, int base = 10)
        : mpq_class(value.c_str(), base)
    {
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

    template <
        typename Numerator,
        typename Denominator,
        typename = std::enable_if_t<std::is_integral_v<Numerator> &&
                                    std::is_integral_v<Denominator> &&
                                    !std::is_same_v<std::remove_cv_t<Numerator>, bool> &&
                                    !std::is_same_v<std::remove_cv_t<Denominator>, bool> &&
                                    (sizeof(Numerator) <= sizeof(std::uint64_t)) &&
                                    (sizeof(Denominator) <= sizeof(std::uint64_t))>>
    mpq_class(Numerator numerator, Denominator denominator)
        : mpq_class(mpz_class(numerator), mpz_class(denominator))
    {
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

    const mpq_t& get_mpq_t() const noexcept
    {
        return value_;
    }

    mpq_t& get_mpq_t() noexcept
    {
        return value_;
    }

    void swap(mpq_class& other) noexcept
    {
        mpq_swap(value_, other.value_);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    mpq_class& operator=(T value)
    {
        const mpz_class integer(value);
        mpq_set_z(value_, integer.mpz_data());
        return *this;
    }

    std::string get_str(int base = 10) const
    {
        gmpfrxx_mkII::detail::gmp_allocated_string raw(mpq_get_str(nullptr, base, value_));
        if (!raw) {
            throw std::runtime_error("mpq_get_str failed");
        }
        return std::string(raw.c_str());
    }

    std::string to_string(int base = 10) const
    {
        return get_str(base);
    }

    int set_str(const char* value, int base = 10)
    {
        if (value == nullptr) {
            return -1;
        }
        mpq_t temp;
        mpq_init(temp);
        const int rc = mpq_set_str(temp, value, base);
        if (rc == 0) {
            mpq_canonicalize(temp);
            mpq_swap(value_, temp);
        }
        mpq_clear(temp);
        return rc;
    }

    int set_str(const std::string& value, int base = 10)
    {
        return set_str(value.c_str(), base);
    }

    void set(const char* value, int base = 10)
    {
        if (set_str(value, base) != 0) {
            throw std::invalid_argument("invalid mpq_class string");
        }
    }

    void set(const std::string& value, int base = 10)
    {
        set(value.c_str(), base);
    }

private:
    mpq_t value_;
};

inline int stream_integer_base(const std::ios_base& out)
{
    const auto basefield = out.flags() & std::ios_base::basefield;
    if (basefield == std::ios_base::hex) {
        return 16;
    }
    if (basefield == std::ios_base::oct) {
        return 8;
    }
    return 10;
}

inline void uppercase_if_requested(std::string& text, const std::ios_base& out)
{
    if (!(out.flags() & std::ios_base::uppercase)) {
        return;
    }
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
}

inline std::string mpz_stream_text(mpz_srcptr value, const std::ios_base& out)
{
    const int base = stream_integer_base(out);
    gmpfrxx_mkII::detail::gmp_allocated_string raw(mpz_get_str(nullptr, base, value));
    if (!raw) {
        throw std::runtime_error("mpz_get_str failed");
    }

    std::string text(raw.c_str());
    const bool negative = !text.empty() && text.front() == '-';
    if (negative) {
        text.erase(text.begin());
    }

    if ((out.flags() & std::ios_base::showbase) && mpz_sgn(value) != 0) {
        if (base == 16) {
            text.insert(0, (out.flags() & std::ios_base::uppercase) ? "0X" : "0x");
        } else if (base == 8 && text.front() != '0') {
            text.insert(0, "0");
        }
    }

    if (negative) {
        text.insert(0, "-");
    } else if (out.flags() & std::ios_base::showpos) {
        text.insert(0, "+");
    }

    uppercase_if_requested(text, out);
    return text;
}

inline void apply_stream_padding(std::string& text, std::ostream& out)
{
    const std::streamsize width = out.width();
    if (static_cast<std::streamsize>(text.size()) >= width) {
        out.width(0);
        return;
    }

    const auto fill_count = static_cast<std::size_t>(width - static_cast<std::streamsize>(text.size()));
    if (out.flags() & std::ios_base::left) {
        text.append(fill_count, out.fill());
    } else if (out.flags() & std::ios_base::internal) {
        std::size_t pos = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1 : 0;
        if (text.size() > pos + 1 && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X')) {
            pos += 2;
        }
        text.insert(pos, fill_count, out.fill());
    } else {
        text.insert(0, fill_count, out.fill());
    }
    out.width(0);
}

inline std::ostream& operator<<(std::ostream& out, const mpz_class& value)
{
    try {
        std::string text = mpz_stream_text(value.mpz_data(), out);
        apply_stream_padding(text, out);
        out << text;
    } catch (...) {
        out.setstate(std::ios_base::badbit);
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const mpq_class& value)
{
    try {
        std::string text = mpz_stream_text(mpq_numref(value.mpq_data()), out);
        if (mpz_cmp_ui(mpq_denref(value.mpq_data()), 1) != 0) {
            std::string denominator = mpz_stream_text(mpq_denref(value.mpq_data()), out);
            if (!denominator.empty() && denominator.front() == '+') {
                denominator.erase(denominator.begin());
            }
            text += "/";
            text += denominator;
        }
        apply_stream_padding(text, out);
        out << text;
    } catch (...) {
        out.setstate(std::ios_base::badbit);
    }
    return out;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                         (std::is_same_v<typename std::decay_t<Expr>::result_type, mpz_class> ||
                          std::is_same_v<typename std::decay_t<Expr>::result_type, mpq_class>),
                     int> = 0>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << typename std::decay_t<Expr>::result_type(expr);
}

inline std::istream& operator>>(std::istream& in, mpz_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    std::string token;
    const int base = gmpfrxx_mkII::detail::gmp_stream_input_base(in);
    const bool parsed_token = gmpfrxx_mkII::detail::gmp_read_integer_token(in, token, base);
    const std::string parse_token = gmpfrxx_mkII::detail::gmp_strip_leading_plus(std::move(token));

    mpz_class tmp;
    if (parsed_token && tmp.set_str(parse_token, base) == 0) {
        value.swap(tmp);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

inline std::istream& operator>>(std::istream& in, mpq_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    const int base = gmpfrxx_mkII::detail::gmp_stream_input_base(in);
    std::string token;
    const bool parsed_token = gmpfrxx_mkII::detail::gmp_read_rational_token(in, token, base);
    const std::string parse_token = gmpfrxx_mkII::detail::gmp_strip_leading_plus(std::move(token));

    mpq_class tmp;
    bool zero_denominator = false;
    const std::size_t slash = parse_token.find('/');
    if (slash != std::string::npos) {
        mpz_class denominator;
        zero_denominator = denominator.set_str(parse_token.substr(slash + 1), base) == 0 &&
                           mpz_sgn(denominator.mpz_data()) == 0;
    }

    if (parsed_token && !zero_denominator && tmp.set_str(parse_token, base) == 0) {
        value.swap(tmp);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

inline void swap(mpz_class& lhs, mpz_class& rhs) noexcept
{
    lhs.swap(rhs);
}

inline void swap(mpq_class& lhs, mpq_class& rhs) noexcept
{
    lhs.swap(rhs);
}

inline bool operator==(const mpz_class& lhs, const mpz_class& rhs)
{
    return mpz_cmp(lhs.mpz_data(), rhs.mpz_data()) == 0;
}

inline bool operator!=(const mpz_class& lhs, const mpz_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpq_class& lhs, const mpq_class& rhs)
{
    return mpq_cmp(lhs.mpq_data(), rhs.mpq_data()) == 0;
}

inline bool operator!=(const mpq_class& lhs, const mpq_class& rhs)
{
    return !(lhs == rhs);
}

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

template <typename T>
struct is_zq_comparison_scalar
    : std::bool_constant<is_supported_expression_integral_v<T>> {};

template <typename T>
inline constexpr bool is_zq_comparison_scalar_v =
    is_zq_comparison_scalar<std::remove_cv_t<T>>::value;

template <typename T>
struct is_zq_comparison_operand
    : std::bool_constant<is_zq_expression_operand_v<T> || is_zq_comparison_scalar_v<T>> {};

template <typename T>
inline constexpr bool is_zq_comparison_operand_v =
    is_zq_comparison_operand<std::decay_t<T>>::value;

template <typename Lhs, typename Rhs>
struct is_zq_comparison_pair
    : std::bool_constant<is_zq_comparison_operand_v<Lhs> &&
                         is_zq_comparison_operand_v<Rhs> &&
                         (is_zq_object_or_node_v<Lhs> ||
                          is_zq_object_or_node_v<Rhs>)> {};

template <typename Lhs, typename Rhs>
inline constexpr bool is_zq_comparison_pair_v =
    is_zq_comparison_pair<Lhs, Rhs>::value;

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

inline gmpxx::mpq_class zq_comparison_value(const gmpxx::mpz_class& value)
{
    return gmpxx::mpq_class(value);
}

inline gmpxx::mpq_class zq_comparison_value(const gmpxx::mpq_class& value)
{
    return value;
}

template <
    typename Expr,
    std::enable_if_t<is_expression_node_v<std::decay_t<Expr>> &&
                         std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpz_class>,
                     int> = 0>
inline gmpxx::mpq_class zq_comparison_value(const Expr& expr)
{
    return gmpxx::mpq_class(gmpxx::mpz_class(expr));
}

template <
    typename Expr,
    std::enable_if_t<is_expression_node_v<std::decay_t<Expr>> &&
                         std::is_same_v<typename std::decay_t<Expr>::result_type, gmpxx::mpq_class>,
                     int> = 0>
inline gmpxx::mpq_class zq_comparison_value(const Expr& expr)
{
    return gmpxx::mpq_class(expr);
}

template <typename T, std::enable_if_t<is_zq_comparison_scalar_v<T>, int> = 0>
inline gmpxx::mpq_class zq_comparison_value(T value)
{
    return gmpxx::mpq_class(gmpxx::mpz_class(value));
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline int cmp(Lhs&& lhs, Rhs&& rhs)
{
    const gmpxx::mpq_class left = zq_comparison_value(std::forward<Lhs>(lhs));
    const gmpxx::mpq_class right = zq_comparison_value(std::forward<Rhs>(rhs));
    return mpq_cmp(left.mpq_data(), right.mpq_data());
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline bool operator==(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) == 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline bool operator!=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) != 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline bool operator<(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) < 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline bool operator<=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) <= 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline bool operator>(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) > 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_zq_comparison_pair_v<Lhs, Rhs>, int> = 0>
inline bool operator>=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) >= 0;
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
using ::gmpfrxx_mkII::detail::cmp;
using ::gmpfrxx_mkII::detail::operator==;
using ::gmpfrxx_mkII::detail::operator!=;
using ::gmpfrxx_mkII::detail::operator<;
using ::gmpfrxx_mkII::detail::operator<=;
using ::gmpfrxx_mkII::detail::operator>;
using ::gmpfrxx_mkII::detail::operator>=;

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_ZQ_IMPL_HPP
