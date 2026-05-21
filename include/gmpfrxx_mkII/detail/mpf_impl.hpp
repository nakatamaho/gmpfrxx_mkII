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

#include <gmpfrxx_mkII/detail/common_type_macros.hpp>
#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/gmp_default_context.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <gmp.h>

namespace gmpxx {

inline std::uintmax_t mpf_mp_exp_negative_magnitude(mp_exp_t value) noexcept
{
    return static_cast<std::uintmax_t>(-(value + 1)) + std::uintmax_t{1};
}

inline void ensure_mpf_shift_result_exponent_fits(mpf_srcptr value,
                                                  mp_bitcnt_t bits,
                                                  bool left_shift,
                                                  const char* message)
{
    if (mpf_sgn(value) == 0 || bits == 0) {
        return;
    }

    mp_exp_t exponent = 0;
    mpf_get_d_2exp(&exponent, value);
    std::uintmax_t allowed = 0;
    if (left_shift) {
        if (exponent >= 0) {
            allowed = static_cast<std::uintmax_t>(std::numeric_limits<mp_exp_t>::max() - exponent);
        } else {
            allowed = static_cast<std::uintmax_t>(std::numeric_limits<mp_exp_t>::max()) +
                      mpf_mp_exp_negative_magnitude(exponent);
        }
    } else {
        if (exponent >= 0) {
            allowed = static_cast<std::uintmax_t>(exponent) +
                      mpf_mp_exp_negative_magnitude(std::numeric_limits<mp_exp_t>::min());
        } else {
            const std::uintmax_t exponent_magnitude = mpf_mp_exp_negative_magnitude(exponent);
            const std::uintmax_t min_magnitude =
                mpf_mp_exp_negative_magnitude(std::numeric_limits<mp_exp_t>::min());
            allowed = min_magnitude - exponent_magnitude;
        }
    }

    if (static_cast<std::uintmax_t>(bits) > allowed) {
        throw std::overflow_error(message);
    }
}

class mpf_class {
public:
    mpf_class() : mpf_class(precision_tag{}, default_mpf_precision_bits()) {}

    mpf_class(const mpf_class& other)
    {
        const mp_bitcnt_t precision = other.precision();
        mpf_init2(value_, precision);
        mpf_set(value_, other.value_);
    }

    mpf_class(mpf_class&& other) noexcept
    {
        mpf_init2(value_, other.precision());
        mpf_swap(value_, other.value_);
    }

    mpf_class(double value) : mpf_class(precision_tag{}, default_mpf_precision_bits())
    {
        gmpfrxx_mkII::detail::require_finite_gmp_double(value, "mpf_class");
        mpf_set_d(value_, value);
    }

    mpf_class(double value, mp_bitcnt_t precision) : mpf_class(precision_tag{}, precision)
    {
        gmpfrxx_mkII::detail::require_finite_gmp_double(value, "mpf_class");
        mpf_set_d(value_, value);
    }

    mpf_class(bool) = delete;
    mpf_class(bool, mp_bitcnt_t) = delete;

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T> &&
                                                      !std::is_same_v<std::remove_cv_t<T>, bool> &&
                                                      (sizeof(T) <= sizeof(std::uint64_t))>>
    mpf_class(T value) : mpf_class(precision_tag{}, default_mpf_precision_bits())
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

    mpf_class(const mpz_class& value) : mpf_class(precision_tag{}, default_mpf_precision_bits())
    {
        mpf_set_z(value_, value.mpz_data());
    }

    mpf_class(const mpq_class& value) : mpf_class(precision_tag{}, default_mpf_precision_bits())
    {
        mpf_set_q(value_, value.mpq_data());
    }

    mpf_class(const mpz_class& value, mp_bitcnt_t precision) : mpf_class(precision_tag{}, precision)
    {
        mpf_set_z(value_, value.mpz_data());
    }

    mpf_class(const mpq_class& value, mp_bitcnt_t precision) : mpf_class(precision_tag{}, precision)
    {
        mpf_set_q(value_, value.mpq_data());
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

    mpf_class(const char* value) : mpf_class(value, default_mpf_precision_bits(), 10) {}

    mpf_class(const std::string& value, mp_bitcnt_t precision, int base = 10)
        : mpf_class(value.c_str(), precision, base)
    {
    }

    mpf_class(const std::string& value) : mpf_class(value.c_str(), default_mpf_precision_bits(), 10) {}

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpf_class>>>
    mpf_class(const Expr& expr);

    template <
        typename Expr,
        std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                             (std::is_same_v<typename std::decay_t<Expr>::result_type, mpz_class> ||
                              std::is_same_v<typename std::decay_t<Expr>::result_type, mpq_class>),
                         int> = 0>
    mpf_class(const Expr& expr) : mpf_class(precision_tag{}, default_mpf_precision_bits())
    {
        try {
            using result_type = typename std::decay_t<Expr>::result_type;
            result_type value(expr);
            if constexpr (std::is_same_v<result_type, mpz_class>) {
                mpf_set_z(value_, value.mpz_data());
            } else {
                mpf_set_q(value_, value.mpq_data());
            }
        } catch (...) {
            mpf_clear(value_);
            throw;
        }
    }

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
        if (this == &other) {
            return *this;
        }

        if constexpr (gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath) {
            assert((precision() == other.precision()) &&
                   "mpf_class move assignment precision mismatch under "
                   "GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH");
            mpf_swap(value_, other.value_);
        } else {
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
        set(value);
        return *this;
    }

    mpf_class& operator=(const mpz_class& value)
    {
        mpf_set_z(value_, value.mpz_data());
        return *this;
    }

    mpf_class& operator=(const mpq_class& value)
    {
        mpf_set_q(value_, value.mpq_data());
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
        if (set_str(value) != 0) {
            throw std::invalid_argument("invalid mpf_class decimal string");
        }
        return *this;
    }

    mpf_class& operator=(const std::string& value)
    {
        if (set_str(value) != 0) {
            throw std::invalid_argument("invalid mpf_class decimal string");
        }
        return *this;
    }

    static mpf_class with_precision(mp_bitcnt_t precision)
    {
        return mpf_class(precision_tag{}, precision);
    }

    static mpf_class with_precision(mp_bitcnt_t precision, double value)
    {
        gmpfrxx_mkII::detail::require_finite_gmp_double(value, "mpf_class");
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

    double get_d() const
    {
        return mpf_get_d(value_);
    }

    unsigned long get_ui() const
    {
        return mpf_get_ui(value_);
    }

    signed long get_si() const
    {
        return mpf_get_si(value_);
    }

    std::uint64_t get_u64() const
    {
        gmpfrxx_mkII::detail::scoped_mpz_t integer;
        mpz_set_f(integer.get(), value_);
        return gmpfrxx_mkII::detail::mpz_get_uint64_checked(integer.get());
    }

    std::int64_t get_i64() const
    {
        gmpfrxx_mkII::detail::scoped_mpz_t integer;
        mpz_set_f(integer.get(), value_);
        return gmpfrxx_mkII::detail::mpz_get_int64_checked(integer.get());
    }

    void set_prec(mp_bitcnt_t precision)
    {
        mpf_set_prec(value_, precision);
    }

    bool fits_sint_p() const
    {
        return mpf_fits_sint_p(value_) != 0;
    }

    bool fits_slong_p() const
    {
        return mpf_fits_slong_p(value_) != 0;
    }

    bool fits_sshort_p() const
    {
        return mpf_fits_sshort_p(value_) != 0;
    }

    bool fits_uint_p() const
    {
        return mpf_fits_uint_p(value_) != 0;
    }

    bool fits_ulong_p() const
    {
        return mpf_fits_ulong_p(value_) != 0;
    }

    bool fits_ushort_p() const
    {
        return mpf_fits_ushort_p(value_) != 0;
    }

    explicit operator bool() const noexcept
    {
        return mpf_sgn(value_) != 0;
    }

    void div_2exp(mp_bitcnt_t bits)
    {
        ensure_mpf_shift_result_exponent_fits(value_, bits, false, "mpf right shift result exponent is too small");
        mpf_div_2exp(value_, value_, bits);
    }

    void mul_2exp(mp_bitcnt_t bits)
    {
        ensure_mpf_shift_result_exponent_fits(value_, bits, true, "mpf left shift result exponent is too large");
        mpf_mul_2exp(value_, value_, bits);
    }

    void set_epsilon()
    {
        mpf_set_ui(value_, 1);
        const mp_bitcnt_t bits = precision();
        if (bits > 0) {
            mpf_div_2exp(value_, value_, bits - 1);
        }
    }

    void set(double value)
    {
        gmpfrxx_mkII::detail::require_finite_gmp_double(value, "mpf_class");
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
        using value_type = std::remove_cv_t<T>;
        if constexpr (std::is_signed_v<value_type> &&
                      std::numeric_limits<value_type>::digits <= std::numeric_limits<long>::digits) {
            mpf_set_si(value_, static_cast<long>(value));
        } else if constexpr (std::is_unsigned_v<value_type> &&
                             std::numeric_limits<value_type>::digits <= std::numeric_limits<unsigned long>::digits) {
            mpf_set_ui(value_, static_cast<unsigned long>(value));
        } else {
            const mpz_class integer(value);
            mpf_set_z(value_, integer.mpz_data());
        }
    }

    mpf_t value_;
};

inline void swap(mpf_class& lhs, mpf_class& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace gmpxx

namespace std {

template <>
struct common_type<gmpxx::mpz_class, gmpxx::mpf_class> {
    using type = gmpxx::mpf_class;
};

template <>
struct common_type<gmpxx::mpf_class, gmpxx::mpz_class> {
    using type = gmpxx::mpf_class;
};

template <>
struct common_type<gmpxx::mpq_class, gmpxx::mpf_class> {
    using type = gmpxx::mpf_class;
};

template <>
struct common_type<gmpxx::mpf_class, gmpxx::mpq_class> {
    using type = gmpxx::mpf_class;
};

template <>
struct common_type<gmpxx::mpf_class, gmpxx::mpf_class> {
    using type = gmpxx::mpf_class;
};

GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(gmpxx::mpf_class);

template <>
struct numeric_limits<gmpxx::mpf_class> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;
};

} // namespace std

namespace gmpxx {

inline std::string mpf_get_str_abs(mpf_srcptr value, mp_exp_t& exponent, int base, std::size_t digits)
{
    mpf_t magnitude;
    mpf_init2(magnitude, mpf_get_prec(value));
    mpf_abs(magnitude, value);
    gmpfrxx_mkII::detail::gmp_allocated_string raw(mpf_get_str(nullptr, &exponent, base, digits, magnitude));
    mpf_clear(magnitude);
    if (!raw) {
        throw std::bad_alloc();
    }
    return std::string(raw.c_str());
}

inline void mpf_uppercase_if_requested(std::string& text, std::ios_base::fmtflags flags)
{
    if (!(flags & std::ios_base::uppercase)) {
        return;
    }
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
}

inline void add_mpf_base_prefix(std::string& text, mpf_srcptr value, int base, std::ios_base::fmtflags flags)
{
    if (!(flags & std::ios_base::showbase)) {
        return;
    }
    if (base == 16) {
        text.insert(0, "0x");
    } else if (base == 8 && mpf_sgn(value) != 0) {
        text.insert(0, "0");
    }
}

inline bool mpf_formatted_has_nonzero_digit(const std::string& text)
{
    return std::any_of(text.begin(), text.end(), [](unsigned char c) {
        return std::isdigit(c) && c != '0';
    });
}

inline void add_mpf_sign(std::string& text, mpf_srcptr value)
{
    if (mpf_sgn(value) < 0) {
        text.insert(0, "-");
    }
}

inline std::size_t mpf_fixed_digits_wanted(
    mpf_srcptr value,
    int base,
    std::size_t fractional_digits)
{
    mp_exp_t exponent = 0;
    (void)mpf_get_str_abs(value, exponent, base, 1);

    const mp_exp_t integral_digits = std::max<mp_exp_t>(exponent, 1);
    const auto integral_digits_unsigned = static_cast<std::uintmax_t>(integral_digits);
    const auto max_size_unsigned = static_cast<std::uintmax_t>(std::numeric_limits<std::size_t>::max());
    if (integral_digits_unsigned > max_size_unsigned) {
        throw std::length_error("mpf fixed output is too large");
    }

    const std::size_t integral = static_cast<std::size_t>(integral_digits);
    constexpr std::size_t max_size = std::numeric_limits<std::size_t>::max();
    if (integral > max_size - 1 || fractional_digits > max_size - integral - 1) {
        throw std::length_error("mpf fixed output is too large");
    }

    return integral + fractional_digits + 1;
}

inline std::size_t effective_mpf_base_precision(std::streamsize precision)
{
    return static_cast<std::size_t>(precision == 0 ? 6 : precision);
}

inline std::string mpf_to_base_string_default(
    mpf_srcptr value,
    int base,
    std::ios_base::fmtflags flags,
    std::streamsize precision)
{
    if (mpf_sgn(value) == 0) {
        std::string text = (flags & std::ios_base::showpoint) ? "0." : "0";
        add_mpf_base_prefix(text, value, base, flags);
        mpf_uppercase_if_requested(text, flags);
        return text;
    }

    const std::size_t effective_precision = effective_mpf_base_precision(precision);
    mp_exp_t exponent = 0;
    std::string digits = mpf_get_str_abs(value, exponent, base, effective_precision);
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
        const mp_exp_t adjusted_exponent = exponent - 1;
        text += (base == 16) ? "@" : "e";
        text += adjusted_exponent >= 0 ? "+" : "-";
        const mp_exp_t absolute_exponent = adjusted_exponent >= 0 ? adjusted_exponent : -adjusted_exponent;
        if (absolute_exponent < 10) {
            text += "0";
        }
        text += std::to_string(absolute_exponent);
    } else {
        text = digits.substr(0, static_cast<std::size_t>(exponent));
        if (exponent < static_cast<mp_exp_t>(digits.size())) {
            text += ".";
            text += digits.substr(static_cast<std::size_t>(exponent));
        }
    }

    if ((flags & std::ios_base::showpoint) && text.find('.') == std::string::npos) {
        text += ".";
    }
    if (!(flags & std::ios_base::showpoint) && !text.empty() && text.back() == '.') {
        text.pop_back();
    }
    if (base != 8 || !(flags & std::ios_base::showbase) || mpf_formatted_has_nonzero_digit(text)) {
        add_mpf_base_prefix(text, value, base, flags);
    }
    add_mpf_sign(text, value);
    mpf_uppercase_if_requested(text, flags);
    return text;
}

inline std::string mpf_to_base_string_fixed(
    mpf_srcptr value,
    int base,
    std::ios_base::fmtflags flags,
    std::streamsize precision)
{
    const std::size_t effective_precision = effective_mpf_base_precision(precision);
    mp_exp_t exponent = 0;
    const std::size_t digits_wanted = mpf_fixed_digits_wanted(value, base, effective_precision);
    std::string digits = mpf_get_str_abs(value, exponent, base, digits_wanted);
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

    if (base != 8 || !(flags & std::ios_base::showbase) || mpf_formatted_has_nonzero_digit(text)) {
        add_mpf_base_prefix(text, value, base, flags);
    }
    add_mpf_sign(text, value);
    mpf_uppercase_if_requested(text, flags);
    return text;
}

inline std::string mpf_to_base_string_scientific(
    mpf_srcptr value,
    int base,
    std::ios_base::fmtflags flags,
    std::streamsize precision)
{
    const std::size_t effective_precision = effective_mpf_base_precision(precision);
    mp_exp_t exponent = 0;
    std::string digits = mpf_get_str_abs(value, exponent, base, effective_precision + 1);
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

    const mp_exp_t adjusted_exponent = mpf_sgn(value) == 0 ? 0 : exponent - 1;
    text += (base == 16) ? "@" : "e";
    text += adjusted_exponent >= 0 ? "+" : "-";
    const mp_exp_t absolute_exponent = adjusted_exponent >= 0 ? adjusted_exponent : -adjusted_exponent;
    if (absolute_exponent < 10) {
        text += "0";
    }
    text += std::to_string(absolute_exponent);

    add_mpf_base_prefix(text, value, base, flags);
    add_mpf_sign(text, value);
    mpf_uppercase_if_requested(text, flags);
    return text;
}

inline std::string mpf_stream_text(mpf_srcptr value, const std::ios_base& out)
{
    const auto flags = out.flags();
    const auto basefield = flags & std::ios_base::basefield;
    if (basefield != std::ios_base::hex && basefield != std::ios_base::oct) {
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

        std::string format = (flags & std::ios_base::showpoint) ? "%#.*F" : "%.*F";
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
        const int count = gmp_asprintf(&raw, format.c_str(), static_cast<int>(stream_precision), value);
        gmpfrxx_mkII::detail::gmp_allocated_string formatted(raw);
        if (count < 0 || !formatted) {
            return {};
        }

        std::string text(formatted.c_str());
        const char decimal_point = gmpfrxx_mkII::detail::gmp_stream_decimal_point(out);
        if (decimal_point != '.') {
            const std::size_t point = text.find('.');
            if (point != std::string::npos) {
                text[point] = decimal_point;
            }
        }
        return text;
    }

    const int base = basefield == std::ios_base::hex ? 16 : 8;
    const auto floatfield = flags & std::ios_base::floatfield;
    if (floatfield == std::ios_base::fixed) {
        return mpf_to_base_string_fixed(value, base, flags, out.precision());
    }
    if (floatfield == std::ios_base::scientific) {
        return mpf_to_base_string_scientific(value, base, flags, out.precision());
    }
    return mpf_to_base_string_default(value, base, flags, out.precision());
}

inline void print_mpf(std::ostream& out, mpf_srcptr value)
{
    try {
        std::string text = mpf_stream_text(value, out);
        if (text.empty()) {
            out.setstate(std::ios_base::badbit);
            return;
        }

        if ((out.flags() & std::ios_base::showpos) && mpf_sgn(value) >= 0) {
            text.insert(0, "+");
        }

        apply_stream_padding(text, out);
        out << text;
    } catch (...) {
        out.setstate(std::ios_base::badbit);
    }
}

inline std::ostream& operator<<(std::ostream& out, const mpf_class& value)
{
    print_mpf(out, value.mpf_data());
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

class gmp_randclass;

class random_mpf_expr {
public:
    using result_type = mpf_class;

    mp_bitcnt_t materialization_precision() const noexcept
    {
        return has_requested_precision_ ? requested_precision_ : default_mpf_precision_bits();
    }

    void generate(mpf_t dest, mp_bitcnt_t destination_precision) const;

private:
    friend class gmp_randclass;

    random_mpf_expr(
        std::shared_ptr<gmpfrxx_mkII::detail::gmp_randstate_holder> state,
        mp_bitcnt_t requested_precision,
        bool has_requested_precision) noexcept
        : state_(std::move(state)),
          requested_precision_(requested_precision),
          has_requested_precision_(has_requested_precision)
    {
    }

    std::shared_ptr<gmpfrxx_mkII::detail::gmp_randstate_holder> state_;
    mp_bitcnt_t requested_precision_;
    bool has_requested_precision_;
};

inline mpz_class::mpz_class(const mpf_class& value)
{
    mpz_init(value_);
    mpz_set_f(value_, value.mpf_data());
}

inline mpz_class& mpz_class::operator=(const mpf_class& value)
{
    mpz_set_f(value_, value.mpf_data());
    return *this;
}

inline mpq_class::mpq_class(const mpf_class& value)
{
    mpq_init(value_);
    mpq_set_f(value_, value.mpf_data());
    mpq_canonicalize(value_);
}

inline mpq_class& mpq_class::operator=(const mpf_class& value)
{
    mpq_set_f(value_, value.mpf_data());
    mpq_canonicalize(value_);
    return *this;
}

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
        const mpz_class& a,
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

    void seed(const mpz_class& value)
    {
        gmp_randseed(state_->state, value.mpz_data());
    }

    mpz_class get_z_bits(mp_bitcnt_t bits)
    {
        mpz_class result;
        mpz_urandomb(result.mpz_data(), state_->state, bits);
        return result;
    }

    mpz_class get_z_bits(const mpz_class& bits)
    {
        if (mpz_sgn(bits.mpz_data()) < 0) {
            throw std::invalid_argument("random bit count must be non-negative");
        }
        if (mpz_fits_ulong_p(bits.mpz_data()) == 0) {
            throw std::length_error("random bit count does not fit mp_bitcnt_t");
        }
        return get_z_bits(static_cast<mp_bitcnt_t>(mpz_get_ui(bits.mpz_data())));
    }

    mpz_class get_z_range(const mpz_class& limit)
    {
        if (mpz_sgn(limit.mpz_data()) <= 0) {
            throw std::invalid_argument("random range limit must be positive");
        }
        mpz_class result;
        mpz_urandomm(result.mpz_data(), state_->state, limit.mpz_data());
        return result;
    }

    random_mpf_expr get_f() noexcept
    {
        return random_mpf_expr(state_, 0, false);
    }

    random_mpf_expr get_f(mp_bitcnt_t precision) noexcept
    {
        return random_mpf_expr(state_, precision, true);
    }

    random_mpf_expr get_f(const mpf_class& prototype) noexcept
    {
        return get_f(prototype.precision());
    }

private:
    std::shared_ptr<gmpfrxx_mkII::detail::gmp_randstate_holder> state_;
};

inline void random_mpf_expr::generate(mpf_t dest, mp_bitcnt_t destination_precision) const
{
    const mp_bitcnt_t random_bits = has_requested_precision_ ? requested_precision_ : destination_precision;
    mpf_urandomb(dest, state_->state, random_bits);
}

} // namespace gmpxx

inline void print_mpf(std::ostream& out, mpf_srcptr value)
{
    gmpxx::print_mpf(out, value);
}

inline std::ostream& operator<<(std::ostream& out, mpf_srcptr value)
{
    print_mpf(out, value);
    return out;
}

inline std::istream& operator>>(std::istream& in, mpf_ptr value)
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

    gmpxx::mpf_class tmp = gmpxx::mpf_class::with_precision(mpf_get_prec(value));
    if (parsed_token && tmp.set_str(parse_token, base) == 0) {
        mpf_set(value, tmp.get_mpf_t());
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

namespace gmpfrxx_mkII {
namespace detail {

template <>
struct is_expression_node<gmpxx::random_mpf_expr> : std::true_type {};

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

inline borrowed_object_leaf<gmpxx::mpf_class> make_mpf_operand(const gmpxx::mpf_class& value)
{
    return borrowed_object_leaf<gmpxx::mpf_class>(value);
}

inline object_leaf<gmpxx::mpf_class> make_mpf_operand(gmpxx::mpf_class&& value)
{
    return object_leaf<gmpxx::mpf_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpz_class> make_mpf_operand(const gmpxx::mpz_class& value)
{
    return borrowed_object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpf_operand(gmpxx::mpz_class&& value)
{
    return object_leaf<gmpxx::mpz_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpq_class> make_mpf_operand(const gmpxx::mpq_class& value)
{
    return borrowed_object_leaf<gmpxx::mpq_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpf_operand(gmpxx::mpq_class&& value)
{
    return object_leaf<gmpxx::mpq_class>(std::move(value));
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

inline mp_bitcnt_t mpf_expression_precision(const borrowed_object_leaf<gmpxx::mpf_class>& expr)
{
    return expr.get().precision();
}

inline mp_bitcnt_t mpf_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpf_expression_precision(const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpf_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpf_expression_precision(const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

inline mp_bitcnt_t mpf_expression_precision(const gmpxx::random_mpf_expr& expr)
{
    return expr.materialization_precision();
}

template <typename T, typename Result>
mp_bitcnt_t mpf_expression_precision(const scalar_leaf<T, Result>&)
{
    return 0;
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

template <typename T>
struct is_mpf_class_leaf : std::false_type {};

template <>
struct is_mpf_class_leaf<object_leaf<gmpxx::mpf_class>> : std::true_type {};

template <>
struct is_mpf_class_leaf<borrowed_object_leaf<gmpxx::mpf_class>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpf_class_leaf_v =
    is_mpf_class_leaf<std::decay_t<T>>::value;

template <typename T>
struct mpf_expression_contains_random : std::false_type {};

template <>
struct mpf_expression_contains_random<gmpxx::random_mpf_expr> : std::true_type {};

template <typename Op, typename Expr, typename Result>
struct mpf_expression_contains_random<unary_expr<Op, Expr, Result>>
    : mpf_expression_contains_random<std::decay_t<Expr>> {};

template <typename Op, typename Lhs, typename Rhs, typename Result>
struct mpf_expression_contains_random<binary_expr<Op, Lhs, Rhs, Result>>
    : std::bool_constant<
          mpf_expression_contains_random<std::decay_t<Lhs>>::value ||
          mpf_expression_contains_random<std::decay_t<Rhs>>::value> {};

template <typename Expr>
inline constexpr bool mpf_expression_contains_random_v =
    mpf_expression_contains_random<std::decay_t<Expr>>::value;

template <typename Expr>
mp_bitcnt_t mpf_materialization_precision(const Expr& expr)
{
    return mpf_expression_precision(expr);
}

template <typename Expr>
inline constexpr bool mpf_materialization_precision_is_nonzero_v = false;

template <typename Op, typename Lhs, typename Rhs>
inline constexpr bool mpf_materialization_precision_is_nonzero_v<binary_expr<Op, Lhs, Rhs, gmpxx::mpf_class>> =
    is_mpf_class_leaf_v<Lhs> && is_mpf_class_leaf_v<Rhs>;

template <typename Expr>
mp_bitcnt_t mpf_constructor_materialization_precision(const Expr& expr)
{
    mp_bitcnt_t precision = mpf_materialization_precision(expr);
    if constexpr (!mpf_materialization_precision_is_nonzero_v<std::decay_t<Expr>>) {
        if (precision == 0) {
            precision = gmpxx::default_mpf_precision_bits();
        }
    }
    return precision;
}

inline void mpf_evaluate(mpf_t dest, const object_leaf<gmpxx::mpf_class>& expr, mp_bitcnt_t)
{
    mpf_set(dest, expr.get().mpf_data());
}

inline void mpf_evaluate(mpf_t dest, const borrowed_object_leaf<gmpxx::mpf_class>& expr, mp_bitcnt_t)
{
    mpf_set(dest, expr.get().mpf_data());
}

inline void mpf_set_q_at_precision(mpf_t dest, const mpq_t value, mp_bitcnt_t eval_precision)
{
    mpf_t denominator;
    mpf_init2(denominator, eval_precision);
    mpf_set_z(dest, mpq_numref(value));
    mpf_set_z(denominator, mpq_denref(value));
    mpf_div(dest, dest, denominator);
    mpf_clear(denominator);
}

inline void mpf_evaluate(mpf_t dest, const object_leaf<gmpxx::mpz_class>& expr, mp_bitcnt_t)
{
    mpf_set_z(dest, expr.get().mpz_data());
}

inline void mpf_evaluate(mpf_t dest, const borrowed_object_leaf<gmpxx::mpz_class>& expr, mp_bitcnt_t)
{
    mpf_set_z(dest, expr.get().mpz_data());
}

inline void mpf_evaluate(mpf_t dest, const object_leaf<gmpxx::mpq_class>& expr, mp_bitcnt_t eval_precision)
{
    mpf_set_q_at_precision(dest, expr.get().mpq_data(), eval_precision);
}

inline void mpf_evaluate(mpf_t dest, const borrowed_object_leaf<gmpxx::mpq_class>& expr, mp_bitcnt_t eval_precision)
{
    mpf_set_q_at_precision(dest, expr.get().mpq_data(), eval_precision);
}

inline void mpf_evaluate(mpf_t dest, const gmpxx::random_mpf_expr& expr, mp_bitcnt_t eval_precision)
{
    expr.generate(dest, eval_precision);
}

template <typename T, typename Result>
void mpf_evaluate(mpf_t dest, const scalar_leaf<T, Result>& expr, mp_bitcnt_t)
{
    if constexpr (std::is_same_v<T, double>) {
        const double value = expr.value();
        gmpfrxx_mkII::detail::require_finite_gmp_double(value, "mpf_class");
        mpf_set_d(dest, value);
    } else if constexpr (std::is_same_v<T, std::int64_t>) {
        if constexpr (std::numeric_limits<long>::digits >= 63) {
            mpf_set_si(dest, static_cast<long>(expr.value()));
        } else {
            const gmpxx::mpz_class integer(expr.value());
            mpf_set_z(dest, integer.mpz_data());
        }
    } else if constexpr (std::is_same_v<T, std::uint64_t>) {
        if constexpr (std::numeric_limits<unsigned long>::digits >= 64) {
            mpf_set_ui(dest, static_cast<unsigned long>(expr.value()));
        } else {
            const gmpxx::mpz_class integer(expr.value());
            mpf_set_z(dest, integer.mpz_data());
        }
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPF scalar leaf");
    }
}

inline bool mpf_expression_references(const mpf_t target, const object_leaf<gmpxx::mpf_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpf_data()[0]);
}

inline bool mpf_expression_references(const mpf_t target, const borrowed_object_leaf<gmpxx::mpf_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpf_data()[0]);
}

inline bool mpf_expression_references(const mpf_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpf_expression_references(const mpf_t, const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpf_expression_references(const mpf_t, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

inline bool mpf_expression_references(const mpf_t, const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

inline bool mpf_expression_references(const mpf_t, const gmpxx::random_mpf_expr&)
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
        if (mpf_sgn(rhs) == 0) {
            throw std::domain_error("mpf division by zero");
        }
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
    try {
        mpf_evaluate(temp, expr, eval_precision);
    } catch (...) {
        mpf_clear(temp);
        throw;
    }
}

class scoped_mpf_temporary {
public:
    explicit scoped_mpf_temporary(mp_bitcnt_t precision)
    {
        mpf_init2(value_, precision);
    }

    scoped_mpf_temporary(const scoped_mpf_temporary&) = delete;
    scoped_mpf_temporary& operator=(const scoped_mpf_temporary&) = delete;

    ~scoped_mpf_temporary()
    {
        mpf_clear(value_);
    }

    mpf_ptr get() noexcept { return value_; }
    mpf_srcptr get() const noexcept { return value_; }

private:
    mpf_t value_;
};

class mpf_thread_scratch_pool {
public:
    static constexpr std::size_t slot_count = 16;
    static constexpr mp_bitcnt_t max_retained_precision = mp_bitcnt_t{1} << 20;

    struct slot {
        mpf_t value;
        bool initialized = false;
        bool in_use = false;
        mp_bitcnt_t allocated_precision = 0;
        mp_bitcnt_t active_precision = 0;
    };

    ~mpf_thread_scratch_pool()
    {
        for (auto& item : slots_) {
            if (item.initialized) {
                restore_allocated_precision(item);
                mpf_clear(item.value);
            }
        }
    }

    slot* acquire(mp_bitcnt_t precision)
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
        // put mpf_set_prec_raw back into the hot loop.
        item.in_use = false;
    }

private:
    static void restore_allocated_precision(slot& item) noexcept
    {
        if (item.initialized && item.active_precision != item.allocated_precision) {
            mpf_set_prec_raw(item.value, item.allocated_precision);
            item.active_precision = item.allocated_precision;
        }
    }

    static void prepare(slot& item, mp_bitcnt_t precision)
    {
        if (!item.initialized) {
            mpf_init2(item.value, precision);
            item.initialized = true;
            item.allocated_precision = mpf_get_prec(item.value);
            item.active_precision = item.allocated_precision;
        } else if (item.allocated_precision < precision) {
            restore_allocated_precision(item);
            mpf_set_prec(item.value, precision);
            item.allocated_precision = mpf_get_prec(item.value);
            item.active_precision = item.allocated_precision;
        }

        if (item.active_precision != precision) {
            mpf_set_prec_raw(item.value, precision);
            item.active_precision = precision;
        }
    }

    slot slots_[slot_count]{};
};

inline mpf_thread_scratch_pool& mpf_current_thread_scratch_pool()
{
    thread_local mpf_thread_scratch_pool pool;
    return pool;
}

class mpf_thread_scratch {
public:
    explicit mpf_thread_scratch(mp_bitcnt_t precision)
        : slot_(mpf_current_thread_scratch_pool().acquire(precision))
    {
        if (slot_ == nullptr) {
            mpf_init2(fallback_, precision);
            using_fallback_ = true;
        }
    }

    mpf_thread_scratch(const mpf_thread_scratch&) = delete;
    mpf_thread_scratch& operator=(const mpf_thread_scratch&) = delete;

    ~mpf_thread_scratch()
    {
        if (using_fallback_) {
            mpf_clear(fallback_);
        } else {
            mpf_thread_scratch_pool::release(*slot_);
        }
    }

    mpf_ptr get() noexcept
    {
        return using_fallback_ ? fallback_ : slot_->value;
    }

private:
    mpf_thread_scratch_pool::slot* slot_ = nullptr;
    bool using_fallback_ = false;
    mpf_t fallback_;
};

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpf_evaluate(mpf_t dest, const binary_expr<Op, Lhs, Rhs, Result>& expr, mp_bitcnt_t eval_precision)
{
    if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        scoped_mpz_t exact;
        mpz_evaluate(exact.get(), expr);
        mpf_set_z(dest, exact.get());
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        scoped_mpq_t exact;
        mpq_evaluate(exact.get(), expr);
        mpf_set_q_at_precision(dest, exact.get(), eval_precision);
        return;
    }

    if constexpr (std::is_same_v<Op, shl_op> || std::is_same_v<Op, shr_op>) {
        mpf_evaluate(dest, expr.lhs(), eval_precision);
        unsigned long shift_count = 0;
        if constexpr (is_zq_shift_scalar_leaf_v<Rhs>) {
            shift_count = zq_shift_count_from_scalar(expr.rhs().value());
        } else {
            scoped_mpz_t bits_value;
            mpz_evaluate(bits_value.get(), expr.rhs());
            shift_count = zq_shift_count_from_mpz(bits_value.get());
        }
        if constexpr (std::is_same_v<Op, shl_op>) {
            gmpxx::ensure_mpf_shift_result_exponent_fits(
                dest, shift_count, true, "mpf left shift result exponent is too large");
            mpf_mul_2exp(dest, dest, shift_count);
        } else {
            gmpxx::ensure_mpf_shift_result_exponent_fits(
                dest, shift_count, false, "mpf right shift result exponent is too small");
            mpf_div_2exp(dest, dest, shift_count);
        }
        return;
    } else {
        if (mpf_expression_references(dest, expr)) {
            scoped_mpf_temporary lhs(eval_precision);
            scoped_mpf_temporary rhs(eval_precision);
            mpf_evaluate(lhs.get(), expr.lhs(), eval_precision);
            mpf_evaluate(rhs.get(), expr.rhs(), eval_precision);
            mpf_apply_binary<Op>(dest, lhs.get(), rhs.get());
            return;
        }

        if constexpr (is_mpf_class_leaf_v<Lhs> &&
                      is_mpf_class_leaf_v<Rhs>) {
            mpf_apply_binary<Op>(dest, expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
        } else if constexpr (is_mpf_class_leaf_v<Rhs>) {
            mpf_evaluate(dest, expr.lhs(), eval_precision);
            mpf_apply_binary<Op>(dest, dest, expr.rhs().get().mpf_data());
        } else if constexpr (is_mpf_class_leaf_v<Lhs> &&
                             (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
            mpf_evaluate(dest, expr.rhs(), eval_precision);
            mpf_apply_binary<Op>(dest, expr.lhs().get().mpf_data(), dest);
        } else {
            scoped_mpf_temporary rhs(eval_precision);
            mpf_evaluate(dest, expr.lhs(), eval_precision);
            mpf_evaluate(rhs.get(), expr.rhs(), eval_precision);
            mpf_apply_binary<Op>(dest, dest, rhs.get());
        }
    }
}

template <typename Expr>
GMPFRXX_MKII_ALWAYS_INLINE bool mpf_try_assign_direct_leaf_binary(mpf_t, const Expr&)
{
    return false;
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
GMPFRXX_MKII_ALWAYS_INLINE bool mpf_try_assign_direct_leaf_binary(
    mpf_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    if constexpr (std::is_same_v<Result, gmpxx::mpf_class> &&
                  is_mpf_class_leaf_v<Lhs> &&
                  is_mpf_class_leaf_v<Rhs> &&
                  (std::is_same_v<Op, add_op> ||
                   std::is_same_v<Op, sub_op> ||
                   std::is_same_v<Op, mul_op> ||
                   std::is_same_v<Op, div_op>)) {
        mpf_apply_binary<Op>(dest, expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
        return true;
    } else {
        return false;
    }
}

template <typename T>
struct is_mpf_mul_direct_expr : std::false_type {};

template <typename Lhs, typename Rhs>
struct is_mpf_mul_direct_expr<binary_expr<mul_op, Lhs, Rhs, gmpxx::mpf_class>>
    : std::bool_constant<
          is_mpf_class_leaf_v<Lhs> &&
          is_mpf_class_leaf_v<Rhs>> {};

template <typename T>
inline constexpr bool is_mpf_mul_direct_expr_v =
    is_mpf_mul_direct_expr<std::decay_t<T>>::value;

template <typename Lhs, typename Rhs>
GMPFRXX_MKII_ALWAYS_INLINE void mpf_compound_mul_apply(
    mpf_t dest,
    const binary_expr<mul_op, Lhs, Rhs, gmpxx::mpf_class>& expr,
    mp_bitcnt_t precision)
{
    if constexpr (build_options::assume_fixed_precision_fastpath) {
        mpf_thread_scratch product(precision);
        mpf_mul(product.get(), expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
        mpf_add(dest, dest, product.get());
    } else {
        scoped_mpf_temporary product(precision);
        mpf_mul(product.get(), expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
        mpf_add(dest, dest, product.get());
    }
}

template <typename Lhs, typename Rhs>
GMPFRXX_MKII_ALWAYS_INLINE void mpf_compound_submul_apply(
    mpf_t dest,
    const binary_expr<mul_op, Lhs, Rhs, gmpxx::mpf_class>& expr,
    mp_bitcnt_t precision)
{
    if constexpr (build_options::assume_fixed_precision_fastpath) {
        mpf_thread_scratch product(precision);
        mpf_mul(product.get(), expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
        mpf_sub(dest, dest, product.get());
    } else {
        scoped_mpf_temporary product(precision);
        mpf_mul(product.get(), expr.lhs().get().mpf_data(), expr.rhs().get().mpf_data());
        mpf_sub(dest, dest, product.get());
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

template <typename Expr, std::enable_if_t<is_mpf_expression_operand_v<Expr> && is_mpf_object_or_node_v<Expr>, long> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpf_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), gmpxx::mpf_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpf_expression_operand_v<Expr> && is_mpf_object_or_node_v<Expr>, long> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpf_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), gmpxx::mpf_class>(std::move(operand));
}

template <typename Lhs, typename Bits, std::enable_if_t<
                                    is_mpf_object_or_node_v<Lhs> &&
                                        is_supported_expression_integral_v<std::decay_t<Bits>>,
                                    int> = 0>
auto operator<<(Lhs&& lhs, Bits bits)
{
    if constexpr (std::is_signed_v<std::decay_t<Bits>>) {
        if (bits < 0) {
            throw std::invalid_argument("negative shift count");
        }
    }
    auto left = make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = scalar_leaf<std::uint64_t, gmpxx::mpz_class>(static_cast<std::uint64_t>(bits));
    return binary_expr<shl_op, decltype(left), decltype(right), gmpxx::mpf_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Bits, std::enable_if_t<
                                    is_mpf_object_or_node_v<Lhs> &&
                                        is_supported_expression_integral_v<std::decay_t<Bits>>,
                                    int> = 0>
auto operator>>(Lhs&& lhs, Bits bits)
{
    if constexpr (std::is_signed_v<std::decay_t<Bits>>) {
        if (bits < 0) {
            throw std::invalid_argument("negative shift count");
        }
    }
    auto left = make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = scalar_leaf<std::uint64_t, gmpxx::mpz_class>(static_cast<std::uint64_t>(bits));
    return binary_expr<shr_op, decltype(left), decltype(right), gmpxx::mpf_class>(
        std::move(left), std::move(right));
}

template <typename Op, typename Rhs>
GMPFRXX_MKII_ALWAYS_INLINE void mpf_compound_assign(gmpxx::mpf_class& lhs, Rhs&& rhs)
{
    auto operand = make_mpf_operand(std::forward<Rhs>(rhs));
    using operand_type = std::decay_t<decltype(operand)>;
    if constexpr (is_mpf_class_leaf_v<operand_type>) {
        mpf_apply_binary<Op>(lhs.mpf_data(), lhs.mpf_data(), operand.get().mpf_data());
    } else if constexpr (
        is_mpf_mul_direct_expr_v<operand_type> &&
        (std::is_same_v<Op, add_op> || std::is_same_v<Op, sub_op>)) {
        const mp_bitcnt_t precision = mpf_get_prec(lhs.mpf_data());
        if constexpr (std::is_same_v<Op, add_op>) {
            mpf_compound_mul_apply(lhs.mpf_data(), operand, precision);
        } else {
            mpf_compound_submul_apply(lhs.mpf_data(), operand, precision);
        }
    } else {
        const mp_bitcnt_t precision = lhs.precision();
        scoped_mpf_temporary value(precision);
        mpf_evaluate(value.get(), operand, precision);
        mpf_apply_binary<Op>(lhs.mpf_data(), lhs.mpf_data(), value.get());
    }
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

template <typename Expr, typename>
mpf_class::mpf_class(const Expr& expr)
{
    const mp_bitcnt_t precision = gmpfrxx_mkII::detail::mpf_constructor_materialization_precision(expr);
    mpf_init2(value_, precision);
    try {
        if (!gmpfrxx_mkII::detail::mpf_try_assign_direct_leaf_binary(value_, expr)) {
            gmpfrxx_mkII::detail::mpf_evaluate(value_, expr, precision);
        }
    } catch (...) {
        mpf_clear(value_);
        throw;
    }
}

template <typename Expr, typename>
mpf_class::mpf_class(const Expr& expr, mp_bitcnt_t precision)
{
    mpf_init2(value_, precision);
    try {
        if (!gmpfrxx_mkII::detail::mpf_try_assign_direct_leaf_binary(value_, expr)) {
            gmpfrxx_mkII::detail::mpf_evaluate(value_, expr, precision);
        }
    } catch (...) {
        mpf_clear(value_);
        throw;
    }
}

template <typename Expr, typename>
mpf_class& mpf_class::operator=(const Expr& expr)
{
    if (gmpfrxx_mkII::detail::mpf_try_assign_direct_leaf_binary(value_, expr)) {
        return *this;
    }
    const mp_bitcnt_t precision = this->precision();
    gmpfrxx_mkII::detail::mpf_evaluate(value_, expr, precision);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;
using ::gmpfrxx_mkII::detail::operator<<;
using ::gmpfrxx_mkII::detail::operator>>;

template <typename T>
struct is_mpf_comparison_non_scalar
    : std::bool_constant<
          std::is_same_v<std::decay_t<T>, mpf_class> ||
          std::is_same_v<std::decay_t<T>, mpz_class> ||
          std::is_same_v<std::decay_t<T>, mpq_class> ||
          gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<T>>> {};

template <typename Lhs, typename Rhs>
struct is_mpf_comparison_pair
    : std::bool_constant<
          gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Lhs> &&
          gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
          !gmpfrxx_mkII::detail::is_zq_comparison_pair_v<Lhs, Rhs> &&
          (is_mpf_comparison_non_scalar<Lhs>::value ||
           is_mpf_comparison_non_scalar<Rhs>::value)> {};

template <typename Expr>
inline void mpf_compare_evaluate(
    mpf_t dest,
    const Expr& expr,
    mp_bitcnt_t precision)
{
    gmpfrxx_mkII::detail::mpf_evaluate(dest, expr, precision);
}

class scoped_mpf_t {
public:
    explicit scoped_mpf_t(mp_bitcnt_t precision)
    {
        mpf_init2(value_, precision);
    }

    scoped_mpf_t(const scoped_mpf_t&) = delete;
    scoped_mpf_t& operator=(const scoped_mpf_t&) = delete;

    ~scoped_mpf_t()
    {
        mpf_clear(value_);
    }

    mpf_ptr get() noexcept { return value_; }
    mpf_srcptr get() const noexcept { return value_; }

private:
    mpf_t value_;
};

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline int cmp(Lhs&& lhs, Rhs&& rhs)
{
    auto left = gmpfrxx_mkII::detail::make_mpf_operand(std::forward<Lhs>(lhs));
    auto right = gmpfrxx_mkII::detail::make_mpf_operand(std::forward<Rhs>(rhs));
    mp_bitcnt_t precision = std::max(
        gmpfrxx_mkII::detail::mpf_expression_precision(left),
        gmpfrxx_mkII::detail::mpf_expression_precision(right));
    if (precision == 0) {
        precision = default_mpf_precision_bits();
    }

    scoped_mpf_t lhs_value(precision);
    scoped_mpf_t rhs_value(precision);
    mpf_compare_evaluate(lhs_value.get(), left, precision);
    mpf_compare_evaluate(rhs_value.get(), right, precision);
    return mpf_cmp(lhs_value.get(), rhs_value.get());
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator==(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) == 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator!=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) != 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator<(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) < 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator<=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) <= 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator>(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) > 0;
}

template <typename Lhs, typename Rhs, std::enable_if_t<is_mpf_comparison_pair<Lhs, Rhs>::value, int> = 0>
inline bool operator>=(Lhs&& lhs, Rhs&& rhs)
{
    return cmp(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)) >= 0;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpz_class& operator+=(mpz_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs + std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpz_class& operator-=(mpz_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs - std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpz_class& operator*=(mpz_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs * std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpz_class& operator/=(mpz_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs / std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpq_class& operator+=(mpq_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs + std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpq_class& operator-=(mpq_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs - std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpq_class& operator*=(mpq_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs * std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs,
          std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_zq_expression_operand_v<Rhs> &&
                               !gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<std::decay_t<Rhs>>,
                           int> = 0>
inline mpq_class& operator/=(mpq_class& lhs, Rhs&& rhs)
{
    lhs = mpf_class(lhs / std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs>, int> = 0>
inline mpf_class& operator+=(mpf_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpf_compound_assign<gmpfrxx_mkII::detail::add_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs>, int> = 0>
inline mpf_class& operator-=(mpf_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpf_compound_assign<gmpfrxx_mkII::detail::sub_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs>, int> = 0>
inline mpf_class& operator*=(mpf_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpf_compound_assign<gmpfrxx_mkII::detail::mul_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpf_expression_operand_v<Rhs>, int> = 0>
inline mpf_class& operator/=(mpf_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpf_compound_assign<gmpfrxx_mkII::detail::div_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

inline mpf_class& operator<<=(mpf_class& value, unsigned long bits)
{
    value.mul_2exp(bits);
    return value;
}

inline mpf_class& operator>>=(mpf_class& value, unsigned long bits)
{
    value.div_2exp(bits);
    return value;
}

inline mpf_class& operator++(mpf_class& value)
{
    value += 1;
    return value;
}

inline mpf_class operator++(mpf_class& value, int)
{
    mpf_class old(value);
    ++value;
    return old;
}

inline mpf_class operator++(mpf_class&& value)
{
    ++value;
    return std::move(value);
}

inline mpf_class operator++(mpf_class&& value, int)
{
    mpf_class old(value);
    ++value;
    return old;
}

inline mpf_class& operator--(mpf_class& value)
{
    value -= 1;
    return value;
}

inline mpf_class operator--(mpf_class& value, int)
{
    mpf_class old(value);
    --value;
    return old;
}

inline mpf_class operator--(mpf_class&& value)
{
    --value;
    return std::move(value);
}

inline mpf_class operator--(mpf_class&& value, int)
{
    mpf_class old(value);
    --value;
    return old;
}

namespace literals {

inline mpf_class operator"" _mpf(long double value)
{
    return mpf_class(static_cast<double>(value));
}

inline mpf_class operator"" _mpf(unsigned long long value)
{
    return mpf_class(value);
}

inline mpf_class operator"" _mpf(const char* value, std::size_t)
{
    return mpf_class(value);
}

} // namespace literals

using literals::operator"" _mpf;

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_MPF_IMPL_HPP
