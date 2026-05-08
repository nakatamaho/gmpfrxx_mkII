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

#ifndef GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP
#define GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <mutex>

#include <mpfr.h>

namespace gmpfrxx_mkII {
namespace detail {

struct parsed_mpfr_environment {
    mpfr_prec_t precision;
    mpfr_exp_t emin;
    mpfr_exp_t emax;
    mpfr_rnd_t rounding;
};

inline mpfr_prec_t builtin_mpfr_default_precision() noexcept
{
    return 512;
}

inline mpfr_rnd_t builtin_mpfr_default_rounding() noexcept
{
    return MPFR_RNDN;
}

inline bool parse_mpfr_precision(const char* text, mpfr_prec_t& out) noexcept
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
    if (value > static_cast<unsigned long long>(std::numeric_limits<mpfr_prec_t>::max())) {
        return false;
    }
    if (value < static_cast<unsigned long long>(MPFR_PREC_MIN)) {
        return false;
    }
    out = static_cast<mpfr_prec_t>(value);
    return true;
}

inline bool parse_mpfr_exponent(const char* text, mpfr_exp_t& out) noexcept
{
    if (text == nullptr || *text == '\0') {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const long long value = std::strtoll(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return false;
    }
    if (value < static_cast<long long>(std::numeric_limits<mpfr_exp_t>::min()) ||
        value > static_cast<long long>(std::numeric_limits<mpfr_exp_t>::max())) {
        return false;
    }
    out = static_cast<mpfr_exp_t>(value);
    return true;
}

inline bool parse_mpfr_rounding(const char* text, mpfr_rnd_t& out) noexcept
{
    if (text == nullptr) {
        return false;
    }

    if (std::strcmp(text, "RNDN") == 0 || std::strcmp(text, "MPFR_RNDN") == 0) {
        out = MPFR_RNDN;
        return true;
    }
    if (std::strcmp(text, "RNDZ") == 0 || std::strcmp(text, "MPFR_RNDZ") == 0) {
        out = MPFR_RNDZ;
        return true;
    }
    if (std::strcmp(text, "RNDU") == 0 || std::strcmp(text, "MPFR_RNDU") == 0) {
        out = MPFR_RNDU;
        return true;
    }
    if (std::strcmp(text, "RNDD") == 0 || std::strcmp(text, "MPFR_RNDD") == 0) {
        out = MPFR_RNDD;
        return true;
    }
    if (std::strcmp(text, "RNDA") == 0 || std::strcmp(text, "MPFR_RNDA") == 0) {
        out = MPFR_RNDA;
        return true;
    }
    if (std::strcmp(text, "RNDF") == 0 || std::strcmp(text, "MPFR_RNDF") == 0) {
        out = MPFR_RNDF;
        return true;
    }

    return false;
}

inline parsed_mpfr_environment load_mpfr_environment() noexcept
{
    parsed_mpfr_environment result{
        builtin_mpfr_default_precision(),
        mpfr_get_emin(),
        mpfr_get_emax(),
        builtin_mpfr_default_rounding(),
    };

    mpfr_prec_t precision = result.precision;
    if (parse_mpfr_precision(std::getenv("MPFRXX_DEFAULT_PRECISION_BITS"), precision)) {
        result.precision = precision;
    }

    mpfr_exp_t emin = result.emin;
    mpfr_exp_t emax = result.emax;
    const bool has_emin = parse_mpfr_exponent(std::getenv("MPFRXX_EMIN"), emin);
    const bool has_emax = parse_mpfr_exponent(std::getenv("MPFRXX_EMAX"), emax);
    if (has_emin && has_emax && emin <= emax) {
        result.emin = emin;
        result.emax = emax;
    } else if (has_emin && !has_emax && emin <= result.emax) {
        result.emin = emin;
    } else if (!has_emin && has_emax && result.emin <= emax) {
        result.emax = emax;
    }

    mpfr_rnd_t rounding = result.rounding;
    if (parse_mpfr_rounding(std::getenv("MPFRXX_ROUNDING_MODE"), rounding)) {
        result.rounding = rounding;
    }

    return result;
}

class mpfr_exponent_range_guard {
public:
    mpfr_exponent_range_guard(mpfr_exp_t emin, mpfr_exp_t emax)
        : old_emin_(mpfr_get_emin()), old_emax_(mpfr_get_emax())
    {
        active_ = (old_emin_ != emin) || (old_emax_ != emax);
        if (active_) {
            mpfr_set_emin(emin);
            mpfr_set_emax(emax);
        }
    }

    mpfr_exponent_range_guard(const mpfr_exponent_range_guard&) = delete;
    mpfr_exponent_range_guard& operator=(const mpfr_exponent_range_guard&) = delete;

    ~mpfr_exponent_range_guard()
    {
        if (active_) {
            mpfr_set_emin(old_emin_);
            mpfr_set_emax(old_emax_);
        }
    }

private:
    mpfr_exp_t old_emin_;
    mpfr_exp_t old_emax_;
    bool active_{false};
};

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

struct mpfr_default_options {
    mpfr_prec_t precision_bits;
    mpfr_exp_t emin;
    mpfr_exp_t emax;
    mpfr_rnd_t rounding_mode;
};

inline std::mutex& mpfr_default_options_mutex()
{
    static std::mutex mutex;
    return mutex;
}

inline mpfr_default_options& mutable_mpfr_default_options_unlocked()
{
    static mpfr_default_options options = [] {
        const auto loaded = ::gmpfrxx_mkII::detail::load_mpfr_environment();
        return mpfr_default_options{
            loaded.precision,
            loaded.emin,
            loaded.emax,
            loaded.rounding,
        };
    }();
    return options;
}

inline void reload_mpfr_defaults_from_environment()
{
    const auto loaded = ::gmpfrxx_mkII::detail::load_mpfr_environment();
    const std::lock_guard<std::mutex> lock(mpfr_default_options_mutex());
    mutable_mpfr_default_options_unlocked() = mpfr_default_options{
        loaded.precision,
        loaded.emin,
        loaded.emax,
        loaded.rounding,
    };
}

inline mpfr_default_options default_options()
{
    const std::lock_guard<std::mutex> lock(mpfr_default_options_mutex());
    return mutable_mpfr_default_options_unlocked();
}

inline mpfr_prec_t default_precision_bits()
{
    return default_options().precision_bits;
}

inline mpfr_prec_t default_prec()
{
    return default_precision_bits();
}

inline void set_default_precision_bits(mpfr_prec_t precision)
{
    if (precision >= MPFR_PREC_MIN) {
        const std::lock_guard<std::mutex> lock(mpfr_default_options_mutex());
        mutable_mpfr_default_options_unlocked().precision_bits = precision;
    }
}

inline mpfr_rnd_t default_rounding_mode()
{
    return default_options().rounding_mode;
}

inline void set_default_rounding_mode(mpfr_rnd_t rounding)
{
    const std::lock_guard<std::mutex> lock(mpfr_default_options_mutex());
    mutable_mpfr_default_options_unlocked().rounding_mode = rounding;
}

inline mpfr_exp_t default_emin()
{
    return default_options().emin;
}

inline mpfr_exp_t default_emax()
{
    return default_options().emax;
}

inline void set_default_exponent_range(mpfr_exp_t emin, mpfr_exp_t emax)
{
    if (emin <= emax) {
        const std::lock_guard<std::mutex> lock(mpfr_default_options_mutex());
        mutable_mpfr_default_options_unlocked().emin = emin;
        mutable_mpfr_default_options_unlocked().emax = emax;
    }
}

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP
