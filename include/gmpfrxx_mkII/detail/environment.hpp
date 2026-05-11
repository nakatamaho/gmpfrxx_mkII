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

#include <gmpfrxx_mkII/detail/config.hpp>

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits>

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

inline mpfr_prec_t mpfr_library_default_precision() noexcept
{
    return 53;
}

inline mpfr_rnd_t builtin_mpfr_default_rounding() noexcept
{
    return mpfr_get_default_rounding_mode();
}

inline mpfr_rnd_t& stable_mpfr_rounding_mode_storage() noexcept
{
    static thread_local mpfr_rnd_t rounding = MPFR_RNDN;
    return rounding;
}

inline mpfr_rnd_t stable_mpfr_rounding_mode() noexcept
{
    return stable_mpfr_rounding_mode_storage();
}

inline void refresh_stable_mpfr_rounding_mode() noexcept
{
    stable_mpfr_rounding_mode_storage() = mpfr_get_default_rounding_mode();
}

inline bool valid_mpfr_precision(mpfr_prec_t precision) noexcept
{
    return precision >= MPFR_PREC_MIN && precision <= MPFR_PREC_MAX;
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
    if (value < static_cast<unsigned long long>(MPFR_PREC_MIN) ||
        value > static_cast<unsigned long long>(MPFR_PREC_MAX)) {
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

inline bool valid_mpfr_exponent_range(mpfr_exp_t emin, mpfr_exp_t emax) noexcept
{
    return emin <= emax &&
           emin >= mpfr_get_emin_min() &&
           emin <= mpfr_get_emin_max() &&
           emax >= mpfr_get_emax_min() &&
           emax <= mpfr_get_emax_max();
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

    const char* precision_text = std::getenv("MPFRXX_DEFAULT_PRECISION_BITS");
    mpfr_prec_t precision = result.precision;
    if (parse_mpfr_precision(precision_text, precision)) {
        result.precision = precision;
    } else if (precision_text != nullptr) {
        result.precision = mpfr_get_default_prec();
    }

    mpfr_exp_t emin = result.emin;
    mpfr_exp_t emax = result.emax;
    const bool has_emin = parse_mpfr_exponent(std::getenv("MPFRXX_EMIN"), emin);
    const bool has_emax = parse_mpfr_exponent(std::getenv("MPFRXX_EMAX"), emax);
    if (has_emin && has_emax && valid_mpfr_exponent_range(emin, emax)) {
        result.emin = emin;
        result.emax = emax;
    } else if (has_emin && !has_emax && valid_mpfr_exponent_range(emin, result.emax)) {
        result.emin = emin;
    } else if (!has_emin && has_emax && valid_mpfr_exponent_range(result.emin, emax)) {
        result.emax = emax;
    }

    mpfr_rnd_t rounding = result.rounding;
    if (parse_mpfr_rounding(std::getenv("MPFRXX_ROUNDING_MODE"), rounding)) {
        result.rounding = rounding;
    }

    return result;
}

inline bool set_mpfr_default_exponent_range(mpfr_exp_t emin, mpfr_exp_t emax) noexcept
{
    if (!valid_mpfr_exponent_range(emin, emax)) {
        return false;
    }

    const mpfr_exp_t current_emin = mpfr_get_emin();
    const mpfr_exp_t current_emax = mpfr_get_emax();

    if (emax > current_emax && mpfr_set_emax(emax) != 0) {
        return false;
    }
    if (emin < current_emin && mpfr_set_emin(emin) != 0) {
        return false;
    }
    if (emin >= current_emin && mpfr_set_emin(emin) != 0) {
        return false;
    }
    if (emax <= current_emax && mpfr_set_emax(emax) != 0) {
        return false;
    }

    return mpfr_get_emin() == emin && mpfr_get_emax() == emax;
}

inline void apply_mpfr_environment_defaults()
{
    const auto loaded = load_mpfr_environment();
    if (valid_mpfr_precision(loaded.precision)) {
        mpfr_set_default_prec(loaded.precision);
    }
    set_mpfr_default_exponent_range(loaded.emin, loaded.emax);
    mpfr_set_default_rounding_mode(loaded.rounding);
    refresh_stable_mpfr_rounding_mode();
}

inline bool mpfr_default_state_is_library_initial() noexcept
{
    return mpfr_get_default_prec() == mpfr_library_default_precision() &&
           mpfr_get_default_rounding_mode() == MPFR_RNDN &&
           mpfr_get_emin() == MPFR_EMIN_DEFAULT &&
           mpfr_get_emax() == MPFR_EMAX_DEFAULT;
}

inline void initialize_mpfr_defaults_for_current_thread()
{
    // This wrapper requires a thread-safe MPFR build. In that configuration
    // MPFR default precision, rounding mode, and exponent range are owned by
    // libmpfr as thread-local state. Do not use a wrapper-owned thread_local
    // "initialized" flag here: inline function-local TLS can be DSO-local on
    // some platforms. The libmpfr state itself is the source of truth, so apply
    // wrapper environment defaults only while that state still has MPFR's
    // library-initial values.
    if (mpfr_default_state_is_library_initial()) {
        apply_mpfr_environment_defaults();
    }
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

struct mpfr_default_options {
    mpfr_prec_t precision_bits;
    mpfr_exp_t emin;
    mpfr_exp_t emax;
    mpfr_rnd_t rounding_mode;
};

inline void reload_mpfr_defaults_from_environment()
{
    ::gmpfrxx_mkII::detail::initialize_mpfr_defaults_for_current_thread();
    ::gmpfrxx_mkII::detail::apply_mpfr_environment_defaults();
}

inline mpfr_default_options default_options()
{
    ::gmpfrxx_mkII::detail::initialize_mpfr_defaults_for_current_thread();
    return mpfr_default_options{
        mpfr_get_default_prec(),
        mpfr_get_emin(),
        mpfr_get_emax(),
        mpfr_get_default_rounding_mode(),
    };
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
    ::gmpfrxx_mkII::detail::initialize_mpfr_defaults_for_current_thread();
    if (::gmpfrxx_mkII::detail::valid_mpfr_precision(precision)) {
        mpfr_set_default_prec(precision);
    }
}

inline mpfr_rnd_t default_rounding_mode()
{
    return default_options().rounding_mode;
}

inline void set_default_rounding_mode(mpfr_rnd_t rounding)
{
    ::gmpfrxx_mkII::detail::initialize_mpfr_defaults_for_current_thread();
    mpfr_set_default_rounding_mode(rounding);
    ::gmpfrxx_mkII::detail::refresh_stable_mpfr_rounding_mode();
}

class rounding_mode_scope {
public:
    explicit rounding_mode_scope(mpfr_rnd_t rounding) noexcept
        : old_rounding_(mpfr_get_default_rounding_mode()),
          old_stable_rounding_(::gmpfrxx_mkII::detail::stable_mpfr_rounding_mode())
    {
        ::gmpfrxx_mkII::detail::initialize_mpfr_defaults_for_current_thread();
        old_rounding_ = mpfr_get_default_rounding_mode();
        old_stable_rounding_ = ::gmpfrxx_mkII::detail::stable_mpfr_rounding_mode();
        mpfr_set_default_rounding_mode(rounding);
        ::gmpfrxx_mkII::detail::refresh_stable_mpfr_rounding_mode();
    }

    ~rounding_mode_scope() noexcept
    {
        mpfr_set_default_rounding_mode(old_rounding_);
        ::gmpfrxx_mkII::detail::stable_mpfr_rounding_mode_storage() = old_stable_rounding_;
    }

    rounding_mode_scope(const rounding_mode_scope&) = delete;
    rounding_mode_scope& operator=(const rounding_mode_scope&) = delete;

private:
    mpfr_rnd_t old_rounding_;
    mpfr_rnd_t old_stable_rounding_;
};

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
    ::gmpfrxx_mkII::detail::initialize_mpfr_defaults_for_current_thread();
    ::gmpfrxx_mkII::detail::set_mpfr_default_exponent_range(emin, emax);
}

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP
