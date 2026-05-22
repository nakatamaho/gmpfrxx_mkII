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

#ifndef GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP

#include <gmpfrxx_mkII/detail/common_type_macros.hpp>
#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/mpc_environment.hpp>
#include <gmpfrxx_mkII/detail/mpfr_impl.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstdint>
#include <locale>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <mpc.h>

namespace gmpfrxx_mkII {
namespace detail {

inline void mpc_check_component_ranges(mpc_t value, mpc_rnd_t rnd, int inex = MPC_INEX(0, 0))
{
    mpfr_check_range(mpc_realref(value), MPC_INEX_RE(inex), MPC_RND_RE(rnd));
    mpfr_check_range(mpc_imagref(value), MPC_INEX_IM(inex), MPC_RND_IM(rnd));
}

class scoped_mpc_init {
public:
    scoped_mpc_init(mpc_t value, mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
        : value_(value)
    {
        require_valid_mpfr_precision(real_precision);
        require_valid_mpfr_precision(imag_precision);
        mpc_init3(value_, real_precision, imag_precision);
    }

    scoped_mpc_init(const scoped_mpc_init&) = delete;
    scoped_mpc_init& operator=(const scoped_mpc_init&) = delete;

    ~scoped_mpc_init()
    {
        if (active_) {
            mpc_clear(value_);
        }
    }

    void release() noexcept
    {
        active_ = false;
    }

private:
    mpc_ptr value_;
    bool active_{true};
};

struct mpc_eval_context {
    mpc_rnd_t rounding_mode;
};

inline mpc_eval_context current_mpc_eval_context(mpfr_prec_t, mpfr_prec_t)
{
    return mpc_eval_context{
        mpfrxx::default_mpc_rounding_mode(),
    };
}

inline bool mpc_is_component_separator(const std::string& text, std::size_t index)
{
    if (index == 0) {
        return false;
    }
    const char current = text[index];
    if (current != '+' && current != '-') {
        return false;
    }
    const char previous = text[index - 1];
    return previous != 'e' && previous != 'E' && previous != 'p' && previous != 'P';
}

inline bool mpc_split_i_suffix(const std::string& text, std::string& real, std::string& imag)
{
    if (text.empty() || (text.back() != 'i' && text.back() != 'I')) {
        return false;
    }

    std::string body = text.substr(0, text.size() - 1);
    for (std::size_t i = body.size(); i > 0; --i) {
        const std::size_t index = i - 1;
        if (mpc_is_component_separator(body, index)) {
            real = body.substr(0, index);
            imag = body.substr(index);
            return true;
        }
    }

    real = "0";
    if (body.empty() || body == "+") {
        imag = "1";
    } else if (body == "-") {
        imag = "-1";
    } else {
        imag = body;
    }
    return true;
}

inline bool mpc_split_string_components(const char* text, std::string& real, std::string& imag)
{
    const std::string trimmed = mpfr_trim_surrounding_whitespace(text);
    if (trimmed.empty()) {
        return false;
    }

    if (trimmed.front() == '(' && trimmed.back() == ')') {
        const std::string inner = trimmed.substr(1, trimmed.size() - 2);
        const std::size_t comma = inner.find(',');
        if (comma == std::string::npos || inner.find(',', comma + 1) != std::string::npos) {
            return false;
        }
        real = mpfr_trim_surrounding_whitespace(inner.substr(0, comma).c_str());
        imag = mpfr_trim_surrounding_whitespace(inner.substr(comma + 1).c_str());
        return !real.empty() && !imag.empty();
    }

    if (mpc_split_i_suffix(trimmed, real, imag)) {
        real = mpfr_trim_surrounding_whitespace(real.c_str());
        imag = mpfr_trim_surrounding_whitespace(imag.c_str());
        return !real.empty() && !imag.empty();
    }

    real = trimmed;
    imag = "0";
    return true;
}

inline int mpc_set_string_components(mpc_t dest,
                                     const char* text,
                                     int base,
                                     mpc_rnd_t rnd)
{
    std::string real_text;
    std::string imag_text;
    if (!mpc_split_string_components(text, real_text, imag_text)) {
        return -1;
    }

    mpfr_prec_t real_precision = 0;
    mpfr_prec_t imag_precision = 0;
    mpc_get_prec2(&real_precision, &imag_precision, dest);
    scoped_mpfr_temporary real(real_precision);
    scoped_mpfr_temporary imag(imag_precision);
    const int real_status = mpfr_set_str(real.get(), real_text.c_str(), base, MPC_RND_RE(rnd));
    const int imag_status = mpfr_set_str(imag.get(), imag_text.c_str(), base, MPC_RND_IM(rnd));
    if (real_status == 0 && imag_status == 0) {
        const int inex = mpc_set_fr_fr(dest, real.get(), imag.get(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
    }
    return real_status == 0 && imag_status == 0 ? 0 : -1;
}

inline bool mpc_has_nan_component(mpc_srcptr value)
{
    return mpfr_nan_p(mpc_realref(value)) != 0 ||
           mpfr_nan_p(mpc_imagref(value)) != 0;
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

struct mpc_evaluation_context {
    mpfr_prec_t real_precision;
    mpfr_prec_t imag_precision;
    mpc_rnd_t rounding_mode;
};

class mpc_class {
public:
    mpc_class()
        : mpc_class(precision_tag{},
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits())
    {
    }

    mpc_class(const mpc_class& other)
    {
        const mpfr_prec_t real_precision = other.real_precision();
        const mpfr_prec_t imag_precision = other.imag_precision();
        gmpfrxx_mkII::detail::scoped_mpc_init init_guard(value_, real_precision, imag_precision);
        const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
        const int inex = mpc_set(value_, other.value_, context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        init_guard.release();
    }

    mpc_class(mpc_class&& other) noexcept
    {
        mpc_init3(value_, other.real_precision(), other.imag_precision());
        mpc_swap(value_, other.value_);
    }

    mpc_class(const mpfr_class& real, const mpfr_class& imag)
    {
        gmpfrxx_mkII::detail::scoped_mpc_init init_guard(value_, real.precision(), imag.precision());
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real.precision(), imag.precision());
        const int inex = mpc_set_fr_fr(value_, real.mpfr_data(), imag.mpfr_data(), context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        init_guard.release();
    }

    explicit mpc_class(const mpfr_class& real)
    {
        mpc_init3(value_, real.precision(), real.precision());
        try {
            set_real_value(real);
        } catch (...) {
            mpc_clear(value_);
            throw;
        }
    }

    explicit mpc_class(const gmpxx::mpz_class& real)
        : mpc_class(precision_tag{},
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits())
    {
        set_real_value(real);
    }

    explicit mpc_class(const gmpxx::mpq_class& real)
        : mpc_class(precision_tag{},
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits())
    {
        set_real_value(real);
    }

    template <
        typename Scalar,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<Scalar> ||
                                    std::is_same_v<std::remove_cv_t<Scalar>, float> ||
                                    std::is_same_v<std::remove_cv_t<Scalar>, double>>>
    explicit mpc_class(Scalar real)
        : mpc_class(precision_tag{},
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits())
    {
        set_real_value(real);
    }

    explicit mpc_class(const std::complex<double>& value)
        : mpc_class(precision_tag{},
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits())
    {
        set_complex_value(value);
    }

    template <
        typename RealScalar,
        typename ImagScalar,
        typename = std::enable_if_t<(gmpfrxx_mkII::detail::is_supported_expression_integral_v<RealScalar> ||
                                     std::is_same_v<std::remove_cv_t<RealScalar>, float> ||
                                     std::is_same_v<std::remove_cv_t<RealScalar>, double>) &&
                                    (gmpfrxx_mkII::detail::is_supported_expression_integral_v<ImagScalar> ||
                                     std::is_same_v<std::remove_cv_t<ImagScalar>, float> ||
                                     std::is_same_v<std::remove_cv_t<ImagScalar>, double>)>>
    mpc_class(RealScalar real, ImagScalar imag)
        : mpc_class(mpfr_class(real, mpfrxx::default_mpc_real_precision_bits()),
                    mpfr_class(imag, mpfrxx::default_mpc_imag_precision_bits()))
    {
    }

    explicit mpc_class(const char* text)
        : mpc_class(text,
                    mpfrxx::default_mpc_real_precision_bits(),
                    mpfrxx::default_mpc_imag_precision_bits(),
                    0)
    {
    }

    explicit mpc_class(const std::string& text)
        : mpc_class(text.c_str())
    {
    }

    mpc_class(const char* text, mpfr_prec_t precision, int base = 0)
        : mpc_class(text, precision, precision, base)
    {
    }

    mpc_class(const std::string& text, mpfr_prec_t precision, int base = 0)
        : mpc_class(text.c_str(), precision, base)
    {
    }

    mpc_class(const char* text, mpfr_prec_t real_precision, mpfr_prec_t imag_precision, int base)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(real_precision);
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(imag_precision);
        mpc_init3(value_, real_precision, imag_precision);
        try {
            const auto context =
                gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
            if (gmpfrxx_mkII::detail::mpc_set_string_components(value_, text, base, context.rounding_mode) != 0) {
                throw std::invalid_argument("invalid mpc_class string");
            }
        } catch (...) {
            mpc_clear(value_);
            throw;
        }
    }

    mpc_class(const std::string& text, mpfr_prec_t real_precision, mpfr_prec_t imag_precision, int base = 0)
        : mpc_class(text.c_str(), real_precision, imag_precision, base)
    {
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
            const auto context =
                gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
            const int inex = mpc_set(value_, other.value_, context.rounding_mode);
            gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        }
        return *this;
    }

    mpc_class& operator=(mpc_class&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if constexpr (gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath) {
            assert((real_precision() == other.real_precision() &&
                    imag_precision() == other.imag_precision()) &&
                   "mpc_class move assignment precision mismatch under "
                   "GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH");
            mpc_swap(value_, other.value_);
        } else {
            if (real_precision() == other.real_precision() &&
                       imag_precision() == other.imag_precision()) {
                mpc_swap(value_, other.value_);
            } else {
                const auto context =
                    gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
                const int inex = mpc_set(value_, other.value_, context.rounding_mode);
                gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
            }
        }
        return *this;
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpc_class>>>
    mpc_class& operator=(const Expr& expr);

    mpc_class& operator=(const mpfr_class& real)
    {
        set_real_value(real);
        return *this;
    }

    mpc_class& operator=(const gmpxx::mpz_class& real)
    {
        set_real_value(real);
        return *this;
    }

    mpc_class& operator=(const gmpxx::mpq_class& real)
    {
        set_real_value(real);
        return *this;
    }

    template <
        typename Scalar,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<Scalar> ||
                                    std::is_same_v<std::remove_cv_t<Scalar>, float> ||
                                    std::is_same_v<std::remove_cv_t<Scalar>, double>>>
    mpc_class& operator=(Scalar real)
    {
        set_real_value(real);
        return *this;
    }

    mpc_class& operator=(const std::complex<double>& value)
    {
        set_complex_value(value);
        return *this;
    }

    mpc_class& operator=(const char* text)
    {
        if (set_str(text, 0) != 0) {
            throw std::invalid_argument("invalid mpc_class string");
        }
        return *this;
    }

    mpc_class& operator=(const std::string& text)
    {
        return *this = text.c_str();
    }

    static mpc_class with_precision(mpfr_prec_t precision)
    {
        return mpc_class(precision_tag{}, precision, precision);
    }

    static mpc_class with_precision(mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
    {
        return mpc_class(precision_tag{}, real_precision, imag_precision);
    }

    static mpc_class with_precision(mpfr_prec_t precision, double real, double imag)
    {
        mpc_class result = with_precision(precision);
        const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(precision, precision);
        const int inex = mpc_set_d_d(result.value_, real, imag, context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(result.value_, context.rounding_mode, inex);
        return result;
    }

    static mpc_class with_precision(mpfr_prec_t real_precision, mpfr_prec_t imag_precision, double real, double imag)
    {
        mpc_class result = with_precision(real_precision, imag_precision);
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
        const int inex = mpc_set_d_d(result.value_, real, imag, context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(result.value_, context.rounding_mode, inex);
        return result;
    }

    mpfr_prec_t precision() const noexcept
    {
        const mpfr_prec_t real = real_precision();
        const mpfr_prec_t imag = imag_precision();
        return std::max(real, imag);
    }

    mpfr_prec_t real_precision() const noexcept
    {
        mpfr_prec_t real = 0;
        mpfr_prec_t imag = 0;
        mpc_get_prec2(&real, &imag, value_);
        return real;
    }

    mpfr_prec_t imag_precision() const noexcept
    {
        mpfr_prec_t real = 0;
        mpfr_prec_t imag = 0;
        mpc_get_prec2(&real, &imag, value_);
        return imag;
    }

    double real_to_double() const
    {
        return mpfr_get_d(mpc_realref(value_), gmpfrxx_mkII::detail::current_mpfr_rounding_mode());
    }

    double real_get_d() const
    {
        return real_to_double();
    }

    double imag_to_double() const
    {
        return mpfr_get_d(mpc_imagref(value_), gmpfrxx_mkII::detail::current_mpfr_rounding_mode());
    }

    double imag_get_d() const
    {
        return imag_to_double();
    }

    int set_str(const char* text, int base = 0)
    {
        if (text == nullptr) {
            return -1;
        }

        mpc_t temp;
        mpc_init3(temp, real_precision(), imag_precision());
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());

        int rc = mpc_set_str(temp, text, base, context.rounding_mode);
        if (rc != 0) {
            rc = gmpfrxx_mkII::detail::mpc_set_string_components(temp, text, base, context.rounding_mode);
        }
        if (rc == 0) {
            const int inex = mpc_set(value_, temp, context.rounding_mode);
            gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        }
        mpc_clear(temp);
        return rc == 0 ? 0 : -1;
    }

    int set_str(const std::string& text, int base = 0)
    {
        return set_str(text.c_str(), base);
    }

    std::string get_str(int base = 10, std::size_t n_digits = 0) const
    {
        const mpc_rnd_t rnd = default_rounding();
        char* raw = mpc_get_str(base, n_digits, value_, rnd);
        if (raw == nullptr) {
            throw std::runtime_error("mpc_get_str failed");
        }
        std::string result(raw);
        mpc_free_str(raw);
        return result;
    }

    std::string to_string(std::size_t n_digits = 0) const
    {
        return get_str(10, n_digits);
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
        return mpfrxx::default_mpc_rounding_mode();
    }

private:
    struct precision_tag {};

    mpc_class(precision_tag, mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
    {
        gmpfrxx_mkII::detail::scoped_mpc_init init_guard(value_, real_precision, imag_precision);
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
        const int inex = mpc_set_ui(value_, 0, context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        init_guard.release();
    }

    void set_real_value(const mpfr_class& real)
    {
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
        const int inex = mpc_set_fr(value_, real.mpfr_data(), context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
    }

    void set_real_value(const gmpxx::mpz_class& real)
    {
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
        const int inex = mpc_set_z(value_, real.mpz_data(), context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
    }

    void set_real_value(const gmpxx::mpq_class& real)
    {
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
        gmpfrxx_mkII::detail::mpq_require_arithmetic_ready(real.mpq_data());
        const int inex = mpc_set_q(value_, real.mpq_data(), context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
    }

    template <typename Scalar>
    void set_real_value(Scalar real)
    {
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
        if constexpr (std::is_same_v<std::remove_cv_t<Scalar>, float> ||
                      std::is_same_v<std::remove_cv_t<Scalar>, double>) {
            const int inex = mpc_set_d(value_, static_cast<double>(real), context.rounding_mode);
            gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        } else {
            const gmpxx::mpz_class integer(real);
            const int inex = mpc_set_z(value_, integer.mpz_data(), context.rounding_mode);
            gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
        }
    }

    void set_complex_value(const std::complex<double>& value)
    {
        const auto context =
            gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision(), imag_precision());
        const int inex = mpc_set_d_d(value_, value.real(), value.imag(), context.rounding_mode);
        gmpfrxx_mkII::detail::mpc_check_component_ranges(value_, context.rounding_mode, inex);
    }

    mpc_t value_;
};

static_assert(sizeof(mpc_class) == sizeof(mpc_t),
              "mpfrxx::mpc_class must preserve mpc_t dense-array stride");
static_assert(alignof(mpc_class) == alignof(mpc_t),
              "mpfrxx::mpc_class must preserve mpc_t alignment");

} // namespace mpfrxx

namespace std {

template <>
struct common_type<gmpxx::mpz_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, gmpxx::mpz_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<gmpxx::mpq_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, gmpxx::mpq_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpfr_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, mpfrxx::mpfr_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpc_class, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(mpfrxx::mpc_class);

template <>
struct common_type<mpfrxx::mpc_class, std::complex<double>> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<std::complex<double>, mpfrxx::mpc_class> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<mpfrxx::mpfr_class, std::complex<double>> {
    using type = mpfrxx::mpc_class;
};

template <>
struct common_type<std::complex<double>, mpfrxx::mpfr_class> {
    using type = mpfrxx::mpc_class;
};

} // namespace std

namespace gmpfrxx_mkII {
namespace detail {

struct mpc_expression_precision_bits {
    mpfr_prec_t real;
    mpfr_prec_t imag;
};

inline mpfr_prec_t mpc_context_precision(mpc_expression_precision_bits precision)
{
    return std::max({precision.real, precision.imag, mpfrxx::default_precision_bits()});
}

inline mpc_eval_context current_mpc_eval_context(mpc_expression_precision_bits precision)
{
    return current_mpc_eval_context(precision.real, precision.imag);
}

inline mpc_expression_precision_bits max_mpc_precision(
    mpc_expression_precision_bits lhs,
    mpc_expression_precision_bits rhs)
{
    return mpc_expression_precision_bits{
        std::max(lhs.real, rhs.real),
        std::max(lhs.imag, rhs.imag),
    };
}

template <typename T>
struct is_supported_mpc_complex_scalar : std::false_type {};

template <>
struct is_supported_mpc_complex_scalar<std::complex<double>> : std::true_type {};

template <typename T>
inline constexpr bool is_supported_mpc_complex_scalar_v =
    is_supported_mpc_complex_scalar<std::remove_cv_t<T>>::value;

template <typename T>
struct is_supported_mpc_real_scalar
    : std::bool_constant<is_supported_expression_integral_v<T> ||
                         std::is_same_v<std::remove_cv_t<T>, float> ||
                         std::is_same_v<std::remove_cv_t<T>, double>> {};

template <typename T>
inline constexpr bool is_supported_mpc_real_scalar_v =
    is_supported_mpc_real_scalar<std::remove_cv_t<T>>::value;

template <typename T>
struct is_supported_mpc_scalar
    : std::bool_constant<is_supported_mpc_real_scalar_v<T> ||
                         is_supported_mpc_complex_scalar_v<T>> {};

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

template <>
struct normalized_mpc_scalar<std::complex<double>> {
    using type = std::complex<double>;
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

template <typename T, typename = void>
struct is_mpc_non_scalar_operand : std::false_type {};

template <typename T>
struct is_mpc_non_scalar_operand<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, mpfrxx::mpc_class> ||
                     std::is_same_v<std::decay_t<T>, mpfrxx::mpfr_class> ||
                     std::is_same_v<std::decay_t<T>, gmpxx::mpz_class> ||
                     std::is_same_v<std::decay_t<T>, gmpxx::mpq_class>>> : std::true_type {};

template <typename T>
struct is_mpc_non_scalar_operand<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     (std::is_same_v<typename std::decay_t<T>::result_type, mpfrxx::mpc_class> ||
                      std::is_same_v<typename std::decay_t<T>::result_type, mpfrxx::mpfr_class> ||
                      std::is_same_v<typename std::decay_t<T>::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename std::decay_t<T>::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpc_non_scalar_operand_v = is_mpc_non_scalar_operand<T>::value;

template <typename Lhs, typename Rhs>
inline constexpr bool is_mpc_operator_pair_v =
    is_mpc_expression_operand_v<Lhs> && is_mpc_expression_operand_v<Rhs> &&
    (is_mpc_object_or_node_v<Lhs> || is_mpc_object_or_node_v<Rhs> ||
     (is_supported_mpc_complex_scalar_v<std::decay_t<Lhs>> && is_mpc_non_scalar_operand_v<Rhs>) ||
     (is_supported_mpc_complex_scalar_v<std::decay_t<Rhs>> && is_mpc_non_scalar_operand_v<Lhs>));

inline borrowed_object_leaf<mpfrxx::mpc_class> make_mpc_operand(const mpfrxx::mpc_class& value)
{
    return borrowed_object_leaf<mpfrxx::mpc_class>(value);
}

inline object_leaf<mpfrxx::mpc_class> make_mpc_operand(mpfrxx::mpc_class&& value)
{
    return object_leaf<mpfrxx::mpc_class>(std::move(value));
}

inline borrowed_object_leaf<mpfrxx::mpfr_class> make_mpc_operand(const mpfrxx::mpfr_class& value)
{
    return borrowed_object_leaf<mpfrxx::mpfr_class>(value);
}

inline object_leaf<mpfrxx::mpfr_class> make_mpc_operand(mpfrxx::mpfr_class&& value)
{
    return object_leaf<mpfrxx::mpfr_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpz_class> make_mpc_operand(const gmpxx::mpz_class& value)
{
    return borrowed_object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpc_operand(gmpxx::mpz_class&& value)
{
    return object_leaf<gmpxx::mpz_class>(std::move(value));
}

inline borrowed_object_leaf<gmpxx::mpq_class> make_mpc_operand(const gmpxx::mpq_class& value)
{
    return borrowed_object_leaf<gmpxx::mpq_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpc_operand(gmpxx::mpq_class&& value)
{
    return object_leaf<gmpxx::mpq_class>(std::move(value));
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

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<mpfrxx::mpc_class>& expr)
{
    return mpc_expression_precision_bits{expr.get().real_precision(), expr.get().imag_precision()};
}

inline mpc_expression_precision_bits mpc_expression_precision(const borrowed_object_leaf<mpfrxx::mpc_class>& expr)
{
    return mpc_expression_precision_bits{expr.get().real_precision(), expr.get().imag_precision()};
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return mpc_expression_precision_bits{expr.get().precision(), expr.get().precision()};
}

inline mpc_expression_precision_bits mpc_expression_precision(const borrowed_object_leaf<mpfrxx::mpfr_class>& expr)
{
    return mpc_expression_precision_bits{expr.get().precision(), expr.get().precision()};
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return mpc_expression_precision_bits{0, 0};
}

inline mpc_expression_precision_bits mpc_expression_precision(const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return mpc_expression_precision_bits{0, 0};
}

inline mpc_expression_precision_bits mpc_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return mpc_expression_precision_bits{0, 0};
}

inline mpc_expression_precision_bits mpc_expression_precision(const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return mpc_expression_precision_bits{0, 0};
}

template <typename T, typename Result>
mpc_expression_precision_bits mpc_expression_precision(const scalar_leaf<T, Result>&)
{
    return mpc_expression_precision_bits{
        mpfrxx::default_mpc_real_precision_bits(),
        mpfrxx::default_mpc_imag_precision_bits(),
    };
}

template <typename Op, typename Expr, typename Result>
mpc_expression_precision_bits mpc_expression_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpc_expression_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mpc_expression_precision_bits mpc_expression_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return max_mpc_precision(mpc_expression_precision(expr.lhs()), mpc_expression_precision(expr.rhs()));
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<mpfrxx::mpc_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    const int inex = mpc_set(dest, expr.get().mpc_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const borrowed_object_leaf<mpfrxx::mpc_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    const int inex = mpc_set(dest, expr.get().mpc_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<mpfrxx::mpfr_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    const int inex = mpc_set_fr(dest, expr.get().mpfr_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const borrowed_object_leaf<mpfrxx::mpfr_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    const int inex = mpc_set_fr(dest, expr.get().mpfr_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<gmpxx::mpz_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    const int inex = mpc_set_z(dest, expr.get().mpz_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const borrowed_object_leaf<gmpxx::mpz_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    const int inex = mpc_set_z(dest, expr.get().mpz_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const object_leaf<gmpxx::mpq_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    gmpfrxx_mkII::detail::mpq_require_arithmetic_ready(expr.get().mpq_data());
    const int inex = mpc_set_q(dest, expr.get().mpq_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

inline void mpc_evaluate(
    mpc_t dest,
    const borrowed_object_leaf<gmpxx::mpq_class>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    gmpfrxx_mkII::detail::mpq_require_arithmetic_ready(expr.get().mpq_data());
    const int inex = mpc_set_q(dest, expr.get().mpq_data(), rnd);
    mpc_check_component_ranges(dest, rnd, inex);
}

template <typename T, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const scalar_leaf<T, Result>& expr,
    mpc_expression_precision_bits,
    mpc_rnd_t rnd)
{
    if constexpr (std::is_same_v<T, double>) {
        const int inex = mpc_set_d(dest, expr.value(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
    } else if constexpr (std::is_same_v<T, std::complex<double>>) {
        const int inex = mpc_set_d_d(dest, expr.value().real(), expr.value().imag(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
    } else if constexpr (std::is_same_v<T, std::int64_t> ||
                         std::is_same_v<T, std::uint64_t>) {
        const gmpxx::mpz_class integer(expr.value());
        const int inex = mpc_set_z(dest, integer.mpz_data(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPC scalar leaf");
    }
}

inline bool mpc_expression_references(const mpc_t target, const object_leaf<mpfrxx::mpc_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpc_data()[0]);
}

inline bool mpc_expression_references(const mpc_t target, const borrowed_object_leaf<mpfrxx::mpc_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpc_data()[0]);
}

inline bool mpc_expression_references(const mpc_t, const object_leaf<mpfrxx::mpfr_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const borrowed_object_leaf<mpfrxx::mpfr_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const borrowed_object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

inline bool mpc_expression_references(const mpc_t, const borrowed_object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

template <typename T>
struct is_mpc_class_leaf : std::false_type {};

template <>
struct is_mpc_class_leaf<object_leaf<mpfrxx::mpc_class>> : std::true_type {};

template <>
struct is_mpc_class_leaf<borrowed_object_leaf<mpfrxx::mpc_class>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpc_class_leaf_v = is_mpc_class_leaf<std::decay_t<T>>::value;

template <typename Expr>
inline constexpr bool mpc_materialization_precision_is_nonzero_v = false;

template <typename Op, typename Lhs, typename Rhs>
inline constexpr bool mpc_materialization_precision_is_nonzero_v<binary_expr<Op, Lhs, Rhs, mpfrxx::mpc_class>> =
    is_mpc_class_leaf_v<Lhs> && is_mpc_class_leaf_v<Rhs>;

template <typename Expr>
mpc_expression_precision_bits mpc_constructor_materialization_precision(const Expr& expr)
{
    auto precision = mpc_expression_precision(expr);
    if constexpr (!mpc_materialization_precision_is_nonzero_v<std::decay_t<Expr>>) {
        if (precision.real == 0) {
            precision.real = mpfrxx::default_mpc_real_precision_bits();
        }
        if (precision.imag == 0) {
            precision.imag = mpfrxx::default_mpc_imag_precision_bits();
        }
    }
    return precision;
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

#if defined(_WIN32)
inline bool mpc_component_is_finite_number(mpfr_srcptr value) noexcept
{
    return mpfr_number_p(value) != 0;
}

inline bool mpc_component_is_near_exponent_edge(mpfr_srcptr value) noexcept
{
    if (mpfr_regular_p(value) == 0) {
        return false;
    }
    constexpr mpfr_exp_t edge_margin = 4;
    const mpfr_exp_t exponent = mpfr_get_exp(value);
    return exponent >= mpfr_get_emax() - edge_margin ||
           exponent <= mpfr_get_emin() + edge_margin;
}

inline bool mpc_division_scaled_workaround_is_applicable(mpc_srcptr lhs, mpc_srcptr rhs) noexcept
{
    return mpc_component_is_finite_number(mpc_realref(lhs)) &&
           mpc_component_is_finite_number(mpc_imagref(lhs)) &&
           mpc_component_is_finite_number(mpc_realref(rhs)) &&
           mpc_component_is_finite_number(mpc_imagref(rhs)) &&
           !(mpfr_zero_p(mpc_realref(rhs)) != 0 &&
             mpfr_zero_p(mpc_imagref(rhs)) != 0) &&
           (mpc_component_is_near_exponent_edge(mpc_realref(lhs)) ||
            mpc_component_is_near_exponent_edge(mpc_imagref(lhs)) ||
            mpc_component_is_near_exponent_edge(mpc_realref(rhs)) ||
            mpc_component_is_near_exponent_edge(mpc_imagref(rhs)));
}

inline int mpc_div_scaled_finite_workaround(mpc_t dest, mpc_srcptr lhs, mpc_srcptr rhs, mpc_rnd_t rnd)
{
    const mpfr_prec_t real_precision = mpfr_get_prec(mpc_realref(dest));
    const mpfr_prec_t imag_precision = mpfr_get_prec(mpc_imagref(dest));
    const mpfr_prec_t work_precision = std::max(real_precision, imag_precision);
    const mpfr_rnd_t real_rounding = MPC_RND_RE(rnd);
    const mpfr_rnd_t imag_rounding = MPC_RND_IM(rnd);
    const mpfr_rnd_t work_rounding = MPFR_RNDN;

    scoped_mpfr_temporary scale(work_precision);
    scoped_mpfr_temporary ar(work_precision);
    scoped_mpfr_temporary bi(work_precision);
    scoped_mpfr_temporary cr(work_precision);
    scoped_mpfr_temporary di(work_precision);
    scoped_mpfr_temporary denominator(work_precision);
    scoped_mpfr_temporary work(work_precision);
    scoped_mpfr_temporary work2(work_precision);
    scoped_mpfr_temporary real_result(real_precision);
    scoped_mpfr_temporary imag_result(imag_precision);

    const mpfr_srcptr a = mpc_realref(lhs);
    const mpfr_srcptr b = mpc_imagref(lhs);
    const mpfr_srcptr c = mpc_realref(rhs);
    const mpfr_srcptr d = mpc_imagref(rhs);

    if (mpfr_cmpabs(c, d) >= 0) {
        mpfr_abs(scale.get(), c, work_rounding);
    } else {
        mpfr_abs(scale.get(), d, work_rounding);
    }

    mpfr_div(ar.get(), a, scale.get(), work_rounding);
    mpfr_div(bi.get(), b, scale.get(), work_rounding);
    mpfr_div(cr.get(), c, scale.get(), work_rounding);
    mpfr_div(di.get(), d, scale.get(), work_rounding);

    mpfr_sqr(denominator.get(), cr.get(), work_rounding);
    mpfr_sqr(work.get(), di.get(), work_rounding);
    mpfr_add(denominator.get(), denominator.get(), work.get(), work_rounding);

    mpfr_mul(work.get(), ar.get(), cr.get(), work_rounding);
    mpfr_mul(work2.get(), bi.get(), di.get(), work_rounding);
    mpfr_add(work.get(), work.get(), work2.get(), work_rounding);
    const int real_inex = mpfr_div(real_result.get(), work.get(), denominator.get(), real_rounding);

    mpfr_mul(work.get(), bi.get(), cr.get(), work_rounding);
    mpfr_mul(work2.get(), ar.get(), di.get(), work_rounding);
    mpfr_sub(work.get(), work.get(), work2.get(), work_rounding);
    const int imag_inex = mpfr_div(imag_result.get(), work.get(), denominator.get(), imag_rounding);

    mpfr_set(mpc_realref(dest), real_result.get(), real_rounding);
    mpfr_set(mpc_imagref(dest), imag_result.get(), imag_rounding);
    return MPC_INEX(real_inex, imag_inex);
}
#endif

template <typename Op>
void mpc_apply_binary(mpc_t dest, const mpc_t lhs, const mpc_t rhs, mpc_rnd_t rnd)
{
    int inex = 0;
    if constexpr (std::is_same_v<Op, add_op>) {
        inex = mpc_add(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        inex = mpc_sub(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        inex = mpc_mul(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, div_op>) {
#if defined(_WIN32)
        if (mpc_division_scaled_workaround_is_applicable(lhs, rhs)) {
            // Work around MinGW MPC builds where mpc_div can overflow near the
            // 32-bit MPFR exponent limit even when the exact quotient is small.
            inex = mpc_div_scaled_finite_workaround(dest, lhs, rhs, rnd);
        } else
#endif
        {
            inex = mpc_div(dest, lhs, rhs, rnd);
        }
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPC expression operation");
    }
    mpc_check_component_ranges(dest, rnd, inex);
}

template <typename Expr, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const unary_expr<pos_op, Expr, Result>& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    mpc_evaluate(dest, expr.expr(), eval_precision, rnd);
}

inline mpc_rnd_t mpc_dual_rounding_for_negated_result(mpc_rnd_t rnd) noexcept
{
    return MPC_RND(
        mpfr_dual_rounding_for_negated_result(MPC_RND_RE(rnd)),
        mpfr_dual_rounding_for_negated_result(MPC_RND_IM(rnd)));
}

template <typename Expr, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const unary_expr<neg_op, Expr, Result>& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    const mpc_rnd_t inner_rnd = mpc_dual_rounding_for_negated_result(rnd);
    mpc_evaluate(dest, expr.expr(), eval_precision, inner_rnd);
    const mpc_rnd_t exact_negation_rnd = MPC_RND(MPFR_RNDN, MPFR_RNDN);
    const int inex = mpc_neg(dest, dest, exact_negation_rnd);
    mpc_check_component_ranges(dest, exact_negation_rnd, inex);
}

template <typename Expr>
void mpc_evaluate_to_temporary(
    mpc_t temp,
    const Expr& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(eval_precision.real);
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(eval_precision.imag);
    mpc_init3(temp, eval_precision.real, eval_precision.imag);
    try {
        mpc_evaluate(temp, expr, eval_precision, rnd);
    } catch (...) {
        mpc_clear(temp);
        throw;
    }
}

class scoped_mpc_temporary {
public:
    explicit scoped_mpc_temporary(mpc_expression_precision_bits precision)
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision.real);
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision.imag);
        mpc_init3(value_, precision.real, precision.imag);
    }

    scoped_mpc_temporary(const scoped_mpc_temporary&) = delete;
    scoped_mpc_temporary& operator=(const scoped_mpc_temporary&) = delete;

    ~scoped_mpc_temporary()
    {
        mpc_clear(value_);
    }

    mpc_ptr get() noexcept { return value_; }
    mpc_srcptr get() const noexcept { return value_; }

private:
    mpc_t value_;
};

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpc_evaluate(
    mpc_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpc_expression_precision_bits eval_precision,
    mpc_rnd_t rnd)
{
    if constexpr (std::is_same_v<Result, mpfrxx::mpfr_class>) {
        scoped_mpfr_temporary real(eval_precision.real);
        mpfr_evaluate(real.get(), expr, eval_precision.real, MPC_RND_RE(rnd));
        const int inex = mpc_set_fr(dest, real.get(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        scoped_mpz_t exact;
        mpz_evaluate(exact.get(), expr);
        const int inex = mpc_set_z(dest, exact.get(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        scoped_mpq_t exact;
        mpq_evaluate(exact.get(), expr);
        gmpfrxx_mkII::detail::mpq_require_arithmetic_ready(exact.get());
        const int inex = mpc_set_q(dest, exact.get(), rnd);
        mpc_check_component_ranges(dest, rnd, inex);
        return;
    }

    if (mpc_expression_references(dest, expr)) {
        scoped_mpc_temporary lhs(eval_precision);
        scoped_mpc_temporary rhs(eval_precision);
        mpc_evaluate(lhs.get(), expr.lhs(), eval_precision, rnd);
        mpc_evaluate(rhs.get(), expr.rhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, lhs.get(), rhs.get(), rnd);
        return;
    }

    if constexpr (is_mpc_class_leaf_v<Lhs> &&
                  is_mpc_class_leaf_v<Rhs>) {
        mpc_apply_binary<Op>(dest, expr.lhs().get().mpc_data(), expr.rhs().get().mpc_data(), rnd);
    } else if constexpr (is_mpc_class_leaf_v<Rhs>) {
        mpc_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, dest, expr.rhs().get().mpc_data(), rnd);
    } else if constexpr (is_mpc_class_leaf_v<Lhs> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpc_evaluate(dest, expr.rhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, expr.lhs().get().mpc_data(), dest, rnd);
    } else {
        scoped_mpc_temporary rhs(eval_precision);
        mpc_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpc_evaluate(rhs.get(), expr.rhs(), eval_precision, rnd);
        mpc_apply_binary<Op>(dest, dest, rhs.get(), rnd);
    }
}

template <typename Op, typename Rhs>
void mpc_compound_assign_with_context(
    mpfrxx::mpc_class& lhs,
    Rhs&& rhs,
    mpc_expression_precision_bits precision,
    mpc_rnd_t rnd)
{
    auto operand = make_mpc_operand(std::forward<Rhs>(rhs));
    using operand_type = std::decay_t<decltype(operand)>;
    if constexpr (is_mpc_class_leaf_v<operand_type>) {
        mpc_apply_binary<Op>(lhs.mpc_data(), lhs.mpc_data(), operand.get().mpc_data(), rnd);
    } else {
        scoped_mpc_temporary value(precision);
        mpc_evaluate(value.get(), operand, precision, rnd);
        mpc_apply_binary<Op>(lhs.mpc_data(), lhs.mpc_data(), value.get(), rnd);
    }
}

template <typename Op, typename Rhs>
void mpc_compound_assign(mpfrxx::mpc_class& lhs, Rhs&& rhs)
{
    mpfr_prec_t real_precision = 0;
    mpfr_prec_t imag_precision = 0;
    mpc_get_prec2(&real_precision, &imag_precision, lhs.mpc_data());
    const mpc_expression_precision_bits precision{real_precision, imag_precision};
    const auto context = current_mpc_eval_context(precision);
    mpc_compound_assign_with_context<Op>(lhs, std::forward<Rhs>(rhs), precision, context.rounding_mode);
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_operator_pair_v<Lhs, Rhs>,
                                    short> = 0>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<add_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_operator_pair_v<Lhs, Rhs>,
                                    short> = 0>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<sub_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_operator_pair_v<Lhs, Rhs>,
                                    short> = 0>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpc_operand(std::forward<Lhs>(lhs));
    auto right = make_mpc_operand(std::forward<Rhs>(rhs));
    return binary_expr<mul_op, decltype(left), decltype(right), mpfrxx::mpc_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpc_operator_pair_v<Lhs, Rhs>,
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
    const auto precision = gmpfrxx_mkII::detail::mpc_constructor_materialization_precision(expr);
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision.real);
    gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision.imag);
    mpc_init3(value_, precision.real, precision.imag);
    try {
        const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(precision);
        gmpfrxx_mkII::detail::mpc_evaluate(value_, expr, precision, context.rounding_mode);
    } catch (...) {
        mpc_clear(value_);
        throw;
    }
}

template <typename Expr, typename>
mpc_class& mpc_class::operator=(const Expr& expr)
{
    const gmpfrxx_mkII::detail::mpc_expression_precision_bits precision{
        this->real_precision(),
        this->imag_precision(),
    };
    const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(precision);
    gmpfrxx_mkII::detail::mpc_evaluate(value_, expr, precision, context.rounding_mode);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

inline bool operator==(const mpc_class& lhs, const mpc_class& rhs)
{
    if (gmpfrxx_mkII::detail::mpc_has_nan_component(lhs.mpc_data()) ||
        gmpfrxx_mkII::detail::mpc_has_nan_component(rhs.mpc_data())) {
        return false;
    }
    return mpc_cmp(lhs.mpc_data(), rhs.mpc_data()) == 0;
}

inline bool operator!=(const mpc_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpc_class& lhs, const mpfr_class& rhs)
{
    if (gmpfrxx_mkII::detail::mpc_has_nan_component(lhs.mpc_data()) ||
        mpfr_nan_p(rhs.mpfr_data()) != 0) {
        return false;
    }
    return mpfr_cmp(mpc_realref(lhs.mpc_data()), rhs.mpfr_data()) == 0 &&
           mpfr_zero_p(mpc_imagref(lhs.mpc_data())) != 0;
}

inline bool operator==(const mpfr_class& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpc_class& lhs, const mpfr_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const mpfr_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

template <typename Exact>
inline bool mpc_real_component_equals_exact(const mpc_class& lhs, const Exact& rhs)
{
    if (gmpfrxx_mkII::detail::mpc_has_nan_component(lhs.mpc_data()) ||
        mpfr_zero_p(mpc_imagref(lhs.mpc_data())) == 0) {
        return false;
    }

    auto operand = gmpfrxx_mkII::detail::make_mpfr_operand(rhs);
    const auto result = mpfr_compare_mpfr_to_exact_leaf(mpc_realref(lhs.mpc_data()), operand);
    return !result.has_nan && result.order == 0;
}

inline bool operator==(const mpc_class& lhs, const gmpxx::mpz_class& rhs)
{
    return mpc_real_component_equals_exact(lhs, rhs);
}

inline bool operator==(const gmpxx::mpz_class& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpc_class& lhs, const gmpxx::mpz_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const gmpxx::mpz_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const mpc_class& lhs, const gmpxx::mpq_class& rhs)
{
    return mpc_real_component_equals_exact(lhs, rhs);
}

inline bool operator==(const gmpxx::mpq_class& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

inline bool operator!=(const mpc_class& lhs, const gmpxx::mpq_class& rhs)
{
    return !(lhs == rhs);
}

inline bool operator!=(const gmpxx::mpq_class& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

template <typename Complex>
inline bool mpc_components_equal_complex_scalar(const mpc_class& lhs, const Complex& rhs)
{
    if (gmpfrxx_mkII::detail::mpc_has_nan_component(lhs.mpc_data())) {
        return false;
    }

    const auto real_operand = gmpfrxx_mkII::detail::make_mpfr_operand(rhs.real());
    const auto imag_operand = gmpfrxx_mkII::detail::make_mpfr_operand(rhs.imag());
    const auto real_result =
        mpfr_compare_mpfr_to_exact_leaf(mpc_realref(lhs.mpc_data()), real_operand);
    const auto imag_result =
        mpfr_compare_mpfr_to_exact_leaf(mpc_imagref(lhs.mpc_data()), imag_operand);
    return !real_result.has_nan && !imag_result.has_nan &&
           real_result.order == 0 && imag_result.order == 0;
}

template <typename Scalar,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_real_scalar_v<Scalar>, int> = 0>
inline bool operator==(const mpc_class& lhs, Scalar rhs)
{
    return mpc_real_component_equals_exact(lhs, rhs);
}

template <typename Scalar,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_real_scalar_v<Scalar>, int> = 0>
inline bool operator==(Scalar lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

template <typename Complex,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_complex_scalar_v<Complex>, int> = 0>
inline bool operator==(const mpc_class& lhs, const Complex& rhs)
{
    return mpc_components_equal_complex_scalar(lhs, rhs);
}

template <typename Complex,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_complex_scalar_v<Complex>, int> = 0>
inline bool operator==(const Complex& lhs, const mpc_class& rhs)
{
    return rhs == lhs;
}

template <typename Scalar,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_real_scalar_v<Scalar>, int> = 0>
inline bool operator!=(const mpc_class& lhs, Scalar rhs)
{
    return !(lhs == rhs);
}

template <typename Scalar,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_real_scalar_v<Scalar>, int> = 0>
inline bool operator!=(Scalar lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

template <typename Complex,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_complex_scalar_v<Complex>, int> = 0>
inline bool operator!=(const mpc_class& lhs, const Complex& rhs)
{
    return !(lhs == rhs);
}

template <typename Complex,
          std::enable_if_t<gmpfrxx_mkII::detail::is_supported_mpc_complex_scalar_v<Complex>, int> = 0>
inline bool operator!=(const Complex& lhs, const mpc_class& rhs)
{
    return !(lhs == rhs);
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator+=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::add_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator-=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::sub_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator*=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::mul_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
inline mpc_class& operator/=(mpc_class& lhs, Rhs&& rhs)
{
    gmpfrxx_mkII::detail::mpc_compound_assign<gmpfrxx_mkII::detail::div_op>(lhs, std::forward<Rhs>(rhs));
    return lhs;
}

class mpc_context_ref {
public:
    mpc_context_ref(mpc_class& value, mpc_evaluation_context context)
        : value_(&value),
          context_(context),
          precision_{context.real_precision, context.imag_precision},
          rounding_mode_(context.rounding_mode)
    {
        validate_precision();
        check_precision();
    }

    mpc_context_ref(mpc_class& value, mpfr_prec_t precision, mpc_rnd_t rounding_mode)
        : mpc_context_ref(value, precision, precision, rounding_mode)
    {
    }

    mpc_context_ref(mpc_class& value, mpfr_prec_t real_precision, mpfr_prec_t imag_precision, mpc_rnd_t rounding_mode)
        : value_(&value),
          context_{real_precision, imag_precision, rounding_mode},
          precision_{real_precision, imag_precision},
          rounding_mode_(rounding_mode)
    {
        validate_precision();
        check_precision();
    }

    mpc_context_ref(mpc_class& value,
                    mpfr_prec_t real_precision,
                    mpfr_prec_t imag_precision,
                    mpfr_rnd_t real_rounding_mode,
                    mpfr_rnd_t imag_rounding_mode)
        : mpc_context_ref(
              value,
              real_precision,
              imag_precision,
              MPC_RND(real_rounding_mode, imag_rounding_mode))
    {
    }

    mpc_context_ref(const mpc_context_ref&) = default;
    mpc_context_ref& operator=(const mpc_context_ref&) = default;

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
    mpc_context_ref& operator=(Rhs&& rhs)
    {
        auto operand = gmpfrxx_mkII::detail::make_mpc_operand(std::forward<Rhs>(rhs));
        gmpfrxx_mkII::detail::mpc_evaluate(
            value_->mpc_data(),
            operand,
            precision_,
            rounding_mode_);
        return *this;
    }

    mpc_class& value() const noexcept
    {
        return *value_;
    }

    const mpc_evaluation_context& context() const noexcept
    {
        return context_;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
    mpc_context_ref& operator+=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpc_compound_assign_with_context<gmpfrxx_mkII::detail::add_op>(
            *value_,
            std::forward<Rhs>(rhs),
            precision_,
            rounding_mode_);
        return *this;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
    mpc_context_ref& operator-=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpc_compound_assign_with_context<gmpfrxx_mkII::detail::sub_op>(
            *value_,
            std::forward<Rhs>(rhs),
            precision_,
            rounding_mode_);
        return *this;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
    mpc_context_ref& operator*=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpc_compound_assign_with_context<gmpfrxx_mkII::detail::mul_op>(
            *value_,
            std::forward<Rhs>(rhs),
            precision_,
            rounding_mode_);
        return *this;
    }

    template <typename Rhs, std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs>, int> = 0>
    mpc_context_ref& operator/=(Rhs&& rhs)
    {
        gmpfrxx_mkII::detail::mpc_compound_assign_with_context<gmpfrxx_mkII::detail::div_op>(
            *value_,
            std::forward<Rhs>(rhs),
            precision_,
            rounding_mode_);
        return *this;
    }

private:
    void validate_precision() const
    {
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision_.real);
        gmpfrxx_mkII::detail::require_valid_mpfr_precision(precision_.imag);
    }

    void check_precision() const
    {
        if constexpr (!gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath) {
            if (precision_.real != value_->real_precision() ||
                precision_.imag != value_->imag_precision()) {
                throw std::invalid_argument("mpc evaluation context precision must match target precision");
            }
        }
    }

    mpc_class* value_;
    mpc_evaluation_context context_;
    gmpfrxx_mkII::detail::mpc_expression_precision_bits precision_;
    mpc_rnd_t rounding_mode_;
};

inline mpc_context_ref with_context(mpc_class& value, mpc_evaluation_context context)
{
    return mpc_context_ref(value, context);
}

inline mpc_context_ref with_context(mpc_class& value, mpfr_prec_t precision, mpc_rnd_t rounding_mode)
{
    return mpc_context_ref(value, precision, rounding_mode);
}

inline mpc_context_ref with_context(
    mpc_class& value,
    mpfr_prec_t real_precision,
    mpfr_prec_t imag_precision,
    mpc_rnd_t rounding_mode)
{
    return mpc_context_ref(value, real_precision, imag_precision, rounding_mode);
}

inline mpc_context_ref with_context(
    mpc_class& value,
    mpfr_prec_t real_precision,
    mpfr_prec_t imag_precision,
    mpfr_rnd_t real_rounding_mode,
    mpfr_rnd_t imag_rounding_mode)
{
    return mpc_context_ref(
        value,
        real_precision,
        imag_precision,
        real_rounding_mode,
        imag_rounding_mode);
}

namespace detail {

template <typename Expr>
inline mpc_class materialize_mpc_math_operand(const Expr& expr)
{
    if constexpr (std::is_same_v<std::decay_t<Expr>, mpc_class>) {
        return expr;
    } else {
        return mpc_class(expr);
    }
}

template <typename Expr, typename Function>
inline mpc_class unary_mpc_math(const Expr& expr, Function function)
{
    const mpc_class operand = materialize_mpc_math_operand(expr);
    mpc_class result = mpc_class::with_precision(operand.real_precision(), operand.imag_precision());
    const auto context =
        gmpfrxx_mkII::detail::current_mpc_eval_context(result.real_precision(), result.imag_precision());
    const mpc_rnd_t rnd = context.rounding_mode;
    int inex = MPC_INEX(0, 0);
    if constexpr (std::is_void_v<std::invoke_result_t<Function, mpc_t, const mpc_t, mpc_rnd_t>>) {
        function(result.mpc_data(), operand.mpc_data(), rnd);
    } else {
        inex = function(result.mpc_data(), operand.mpc_data(), rnd);
    }
    gmpfrxx_mkII::detail::mpc_check_component_ranges(result.mpc_data(), rnd, inex);
    return result;
}

template <typename Expr, typename Function>
inline mpfr_class unary_mpc_real_math(const Expr& expr, mpfr_prec_t precision, Function function)
{
    const mpc_class operand = materialize_mpc_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(precision);
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const mpfr_rnd_t rnd = context.rounding_mode;
    int inex = 0;
    if constexpr (std::is_void_v<std::invoke_result_t<Function, mpfr_t, const mpc_t, mpfr_rnd_t>>) {
        function(result.mpfr_data(), operand.mpc_data(), rnd);
    } else {
        inex = function(result.mpfr_data(), operand.mpc_data(), rnd);
    }
    mpfr_check_range(result.mpfr_data(), inex, rnd);
    return result;
}

template <typename Lhs, typename Rhs, typename Function>
inline mpc_class binary_mpc_math(const Lhs& lhs, const Rhs& rhs, Function function)
{
    const mpc_class left = materialize_mpc_math_operand(lhs);
    const mpc_class right = materialize_mpc_math_operand(rhs);
    const mpfr_prec_t real_precision = std::max(left.real_precision(), right.real_precision());
    const mpfr_prec_t imag_precision = std::max(left.imag_precision(), right.imag_precision());
    mpc_class result = mpc_class::with_precision(real_precision, imag_precision);
    const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
    const mpc_rnd_t rnd = context.rounding_mode;
    int inex = MPC_INEX(0, 0);
    if constexpr (std::is_void_v<std::invoke_result_t<Function, mpc_t, const mpc_t, const mpc_t, mpc_rnd_t>>) {
        function(result.mpc_data(), left.mpc_data(), right.mpc_data(), rnd);
    } else {
        inex = function(result.mpc_data(), left.mpc_data(), right.mpc_data(), rnd);
    }
    gmpfrxx_mkII::detail::mpc_check_component_ranges(result.mpc_data(), rnd, inex);
    return result;
}

template <typename A, typename B, typename C, typename Function>
inline mpc_class ternary_mpc_math(const A& a, const B& b, const C& c, Function function)
{
    const mpc_class first = materialize_mpc_math_operand(a);
    const mpc_class second = materialize_mpc_math_operand(b);
    const mpc_class third = materialize_mpc_math_operand(c);
    const mpfr_prec_t real_precision = std::max({first.real_precision(),
                                                 second.real_precision(),
                                                 third.real_precision()});
    const mpfr_prec_t imag_precision = std::max({first.imag_precision(),
                                                 second.imag_precision(),
                                                 third.imag_precision()});
    mpc_class result = mpc_class::with_precision(real_precision, imag_precision);
    const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
    const mpc_rnd_t rnd = context.rounding_mode;
    int inex = MPC_INEX(0, 0);
    if constexpr (std::is_void_v<std::invoke_result_t<Function, mpc_t, const mpc_t, const mpc_t, const mpc_t, mpc_rnd_t>>) {
        function(result.mpc_data(), first.mpc_data(), second.mpc_data(), third.mpc_data(), rnd);
    } else {
        inex = function(result.mpc_data(),
                        first.mpc_data(),
                        second.mpc_data(),
                        third.mpc_data(),
                        rnd);
    }
    gmpfrxx_mkII::detail::mpc_check_component_ranges(result.mpc_data(), rnd, inex);
    return result;
}

} // namespace detail

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class sqrt(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_sqrt(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class exp(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_exp(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class log(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_log(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class log10(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_log10(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class sqr(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_sqr(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class conj(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_conj(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpc_class proj(const Expr& expr)
{
    return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
        return mpc_proj(rop, op, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class real(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.real_precision());
    const auto context = gmpfrxx_mkII::detail::current_eval_context(operand.real_precision());
    const int inex = mpc_real(result.mpfr_data(), operand.mpc_data(), context.rounding_mode);
    mpfr_check_range(result.mpfr_data(), inex, context.rounding_mode);
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class imag(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    mpfr_class result = mpfr_class::with_precision(operand.imag_precision());
    const auto context = gmpfrxx_mkII::detail::current_eval_context(operand.imag_precision());
    const int inex = mpc_imag(result.mpfr_data(), operand.mpc_data(), context.rounding_mode);
    mpfr_check_range(result.mpfr_data(), inex, context.rounding_mode);
    return result;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class abs(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    return detail::unary_mpc_real_math(operand,
                                       std::max(operand.real_precision(), operand.imag_precision()),
                                       [](mpfr_t rop, const mpc_t op, mpfr_rnd_t rnd) {
                                           return mpc_abs(rop, op, rnd);
                                       });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class norm(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    return detail::unary_mpc_real_math(operand,
                                       std::max(operand.real_precision(), operand.imag_precision()),
                                       [](mpfr_t rop, const mpc_t op, mpfr_rnd_t rnd) {
                                           return mpc_norm(rop, op, rnd);
                                       });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline mpfr_class arg(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    return detail::unary_mpc_real_math(operand,
                                       std::max(operand.real_precision(), operand.imag_precision()),
                                       [](mpfr_t rop, const mpc_t op, mpfr_rnd_t rnd) {
                                           return mpc_arg(rop, op, rnd);
                                       });
}

#define GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(name) \
    template < \
        typename Expr, \
        std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> && \
                             gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>, \
                         int> = 0> \
    inline mpc_class name(const Expr& expr) \
    { \
        return detail::unary_mpc_math(expr, [](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) { \
            return mpc_##name(rop, op, rnd); \
        }); \
    }

GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(sin)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(cos)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(tan)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(sinh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(cosh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(tanh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(asin)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(acos)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(atan)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(asinh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(acosh)
GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION(atanh)

#undef GMPFRXX_MKII_DEFINE_MPC_UNARY_FUNCTION

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Rhs>),
                     int> = 0>
inline mpc_class pow(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpc_math(lhs, rhs, [](mpc_t rop, const mpc_t op1, const mpc_t op2, mpc_rnd_t rnd) {
        return mpc_pow(rop, op1, op2, rnd);
    });
}

template <
    typename Lhs,
    typename Rhs,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Lhs> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Rhs> &&
                         (gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Lhs> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Rhs>),
                     int> = 0>
inline mpc_class agm(const Lhs& lhs, const Rhs& rhs)
{
    return detail::binary_mpc_math(lhs, rhs, [](mpc_t rop, const mpc_t op1, const mpc_t op2, mpc_rnd_t rnd) {
        return mpc_agm(rop, op1, op2, rnd);
    });
}

template <
    typename A,
    typename B,
    typename C,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<A> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<B> &&
                         gmpfrxx_mkII::detail::is_mpc_expression_operand_v<C> &&
                         (gmpfrxx_mkII::detail::is_mpc_object_or_node_v<A> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<B> ||
                          gmpfrxx_mkII::detail::is_mpc_object_or_node_v<C>),
                     int> = 0>
inline mpc_class fma(const A& a, const B& b, const C& c)
{
    return detail::ternary_mpc_math(a, b, c, [](mpc_t rop,
                                                const mpc_t op1,
                                                const mpc_t op2,
                                                const mpc_t op3,
                                                mpc_rnd_t rnd) {
        return mpc_fma(rop, op1, op2, op3, rnd);
    });
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_mpc_expression_operand_v<Expr> &&
                         gmpfrxx_mkII::detail::is_mpc_object_or_node_v<Expr>,
                     int> = 0>
inline std::pair<mpc_class, mpc_class> sin_cos(const Expr& expr)
{
    const mpc_class operand = detail::materialize_mpc_math_operand(expr);
    mpc_class sine = mpc_class::with_precision(operand.real_precision(), operand.imag_precision());
    mpc_class cosine = mpc_class::with_precision(operand.real_precision(), operand.imag_precision());
    const auto context =
        gmpfrxx_mkII::detail::current_mpc_eval_context(operand.real_precision(), operand.imag_precision());
    const mpc_rnd_t rnd = context.rounding_mode;
    mpc_sin_cos(sine.mpc_data(),
                cosine.mpc_data(),
                operand.mpc_data(),
                rnd,
                rnd);
    gmpfrxx_mkII::detail::mpc_check_component_ranges(sine.mpc_data(), rnd);
    gmpfrxx_mkII::detail::mpc_check_component_ranges(cosine.mpc_data(), rnd);
    return {std::move(sine), std::move(cosine)};
}

inline mpc_class rootofunity(unsigned long order,
                             unsigned long index,
    mpfr_prec_t real_precision,
    mpfr_prec_t imag_precision)
{
    mpc_class result = mpc_class::with_precision(real_precision, imag_precision);
    const auto context = gmpfrxx_mkII::detail::current_mpc_eval_context(real_precision, imag_precision);
    const mpc_rnd_t rnd = context.rounding_mode;
    const int inex = mpc_rootofunity(result.mpc_data(), order, index, rnd);
    gmpfrxx_mkII::detail::mpc_check_component_ranges(result.mpc_data(), rnd, inex);
    return result;
}

inline mpc_class rootofunity(unsigned long order, unsigned long index, mpfr_prec_t precision)
{
    return rootofunity(order, index, precision, precision);
}

inline mpc_class rootofunity(unsigned long order, unsigned long index)
{
    return rootofunity(order,
                       index,
                       mpfrxx::default_mpc_real_precision_bits(),
                       mpfrxx::default_mpc_imag_precision_bits());
}

namespace detail {

inline std::string mpc_format_component(std::ostream& out, mpfr_srcptr value)
{
    std::string text = gmpfrxx_mkII::detail::mpfr_stream_text(value, out);
    if (text.empty()) {
        return {};
    }
    const char decimal_point = std::use_facet<std::numpunct<char>>(out.getloc()).decimal_point();
    if (decimal_point != '.' && text.find('.') != std::string::npos) {
        text[text.find('.')] = decimal_point;
    }
    if ((out.flags() & std::ios_base::showpos) && mpfr_sgn(value) >= 0) {
        text.insert(0, "+");
    }
    return text;
}

} // namespace detail

inline std::ostream& operator<<(std::ostream& out, const mpc_class& value)
{
    out.width(0);
    const std::string real = detail::mpc_format_component(out, mpc_realref(value.mpc_data()));
    const std::string imag = detail::mpc_format_component(out, mpc_imagref(value.mpc_data()));
    if (real.empty() || imag.empty()) {
        out.setstate(std::ios_base::badbit);
        return out;
    }
    out << '(' << real << ',' << imag << ')';
    return out;
}

template <
    typename Expr,
    std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                         std::is_same_v<typename std::decay_t<Expr>::result_type, mpc_class>,
                     int> = 0>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << mpc_class(expr);
}

inline std::istream& operator>>(std::istream& in, mpc_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    mpfr_class real = mpfr_class::with_precision(value.real_precision());
    mpfr_class imag = mpfr_class::with_precision(value.imag_precision());
    char open = '\0';
    char comma = '\0';
    char close = '\0';

    if ((in >> open) && open == '(' &&
        (in >> real) &&
        (in >> comma) && comma == ',' &&
        (in >> imag) &&
        (in >> close) && close == ')') {
        value = mpc_class(real, imag);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

namespace literals {

inline mpc_class operator"" _mpc_i(long double value)
{
    return mpc_class::with_precision(
        default_mpc_real_precision_bits(),
        default_mpc_imag_precision_bits(),
        0.0,
        static_cast<double>(value));
}

inline mpc_class operator"" _mpc_i(const char* value, std::size_t)
{
    const mpfr_prec_t real_precision = default_mpc_real_precision_bits();
    const mpfr_prec_t imag_precision = default_mpc_imag_precision_bits();
    mpfr_class real = mpfr_class::with_precision(real_precision);
    mpfr_class imag(value, imag_precision, 0);
    return mpc_class(real, imag);
}

} // namespace literals

using literals::operator"" _mpc_i;

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPC_IMPL_HPP
