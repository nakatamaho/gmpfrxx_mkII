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

#ifndef GMPFRXX_MKII_DETAIL_MPC_ENVIRONMENT_HPP
#define GMPFRXX_MKII_DETAIL_MPC_ENVIRONMENT_HPP

#include <gmpfrxx_mkII/detail/environment.hpp>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <mpc.h>

namespace mpfrxx {

struct mpc_default_options {
    mpfr_prec_t real_precision_bits;
    mpfr_prec_t imag_precision_bits;
    mpfr_rnd_t real_rounding_mode;
    mpfr_rnd_t imag_rounding_mode;
};

inline mpc_default_options load_mpc_defaults_from_environment()
{
    mpfr_prec_t precision = default_precision_bits();
    const char* precision_text = std::getenv("MPCXX_DEFAULT_PRECISION_BITS");
    if (!::gmpfrxx_mkII::detail::parse_mpfr_precision(precision_text, precision)) {
        ::gmpfrxx_mkII::detail::warn_invalid_environment_value(
            "MPCXX_DEFAULT_PRECISION_BITS", precision_text);
    }

    mpfr_prec_t real_precision = precision;
    const char* real_precision_text = std::getenv("MPCXX_DEFAULT_REAL_PRECISION_BITS");
    const bool has_real_precision =
        ::gmpfrxx_mkII::detail::parse_mpfr_precision(real_precision_text, real_precision);
    if (!has_real_precision) {
        ::gmpfrxx_mkII::detail::warn_invalid_environment_value(
            "MPCXX_DEFAULT_REAL_PRECISION_BITS", real_precision_text);
    }

    mpfr_prec_t imag_precision = precision;
    const char* imag_precision_text = std::getenv("MPCXX_DEFAULT_IMAG_PRECISION_BITS");
    const bool has_imag_precision =
        ::gmpfrxx_mkII::detail::parse_mpfr_precision(imag_precision_text, imag_precision);
    if (!has_imag_precision) {
        ::gmpfrxx_mkII::detail::warn_invalid_environment_value(
            "MPCXX_DEFAULT_IMAG_PRECISION_BITS", imag_precision_text);
    }


    mpfr_rnd_t rounding = default_rounding_mode();
    const char* rounding_text = std::getenv("MPCXX_DEFAULT_ROUNDING_MODE");
    if (!::gmpfrxx_mkII::detail::parse_mpfr_rounding(rounding_text, rounding)) {
        ::gmpfrxx_mkII::detail::warn_invalid_environment_value(
            "MPCXX_DEFAULT_ROUNDING_MODE", rounding_text);
    }

    mpfr_rnd_t real_rounding = rounding;
    const char* real_rounding_text = std::getenv("MPCXX_DEFAULT_REAL_ROUNDING_MODE");
    const bool has_real_rounding =
        ::gmpfrxx_mkII::detail::parse_mpfr_rounding(real_rounding_text, real_rounding);
    if (!has_real_rounding) {
        ::gmpfrxx_mkII::detail::warn_invalid_environment_value(
            "MPCXX_DEFAULT_REAL_ROUNDING_MODE", real_rounding_text);
    }

    mpfr_rnd_t imag_rounding = rounding;
    const char* imag_rounding_text = std::getenv("MPCXX_DEFAULT_IMAG_ROUNDING_MODE");
    const bool has_imag_rounding =
        ::gmpfrxx_mkII::detail::parse_mpfr_rounding(imag_rounding_text, imag_rounding);
    if (!has_imag_rounding) {
        ::gmpfrxx_mkII::detail::warn_invalid_environment_value(
            "MPCXX_DEFAULT_IMAG_ROUNDING_MODE", imag_rounding_text);
    }


    return mpc_default_options{
        real_precision,
        imag_precision,
        real_rounding,
        imag_rounding,
    };
}

inline bool valid_mpc_precision_environment_override_is_present() noexcept
{
    mpfr_prec_t parsed = 0;
    return ::gmpfrxx_mkII::detail::parse_mpfr_precision(
               std::getenv("MPCXX_DEFAULT_PRECISION_BITS"), parsed) ||
           ::gmpfrxx_mkII::detail::parse_mpfr_precision(
               std::getenv("MPCXX_DEFAULT_REAL_PRECISION_BITS"), parsed) ||
           ::gmpfrxx_mkII::detail::parse_mpfr_precision(
               std::getenv("MPCXX_DEFAULT_IMAG_PRECISION_BITS"), parsed);
}

inline bool valid_mpc_rounding_environment_override_is_present() noexcept
{
    mpfr_rnd_t parsed = MPFR_RNDN;
    return ::gmpfrxx_mkII::detail::parse_mpfr_rounding(
               std::getenv("MPCXX_DEFAULT_ROUNDING_MODE"), parsed) ||
           ::gmpfrxx_mkII::detail::parse_mpfr_rounding(
               std::getenv("MPCXX_DEFAULT_REAL_ROUNDING_MODE"), parsed) ||
           ::gmpfrxx_mkII::detail::parse_mpfr_rounding(
               std::getenv("MPCXX_DEFAULT_IMAG_ROUNDING_MODE"), parsed);
}

inline bool mpc_environment_is_present() noexcept
{
    return std::getenv("MPCXX_DEFAULT_PRECISION_BITS") != nullptr ||
           std::getenv("MPCXX_DEFAULT_REAL_PRECISION_BITS") != nullptr ||
           std::getenv("MPCXX_DEFAULT_IMAG_PRECISION_BITS") != nullptr ||
           std::getenv("MPCXX_DEFAULT_ROUNDING_MODE") != nullptr ||
           std::getenv("MPCXX_DEFAULT_REAL_ROUNDING_MODE") != nullptr ||
           std::getenv("MPCXX_DEFAULT_IMAG_ROUNDING_MODE") != nullptr;
}

inline bool& mpc_defaults_initialized_storage() noexcept
{
    static thread_local bool initialized = false;
    return initialized;
}

struct mpc_precision_override_state {
    bool active;
    mpfr_prec_t real_precision_bits;
    mpfr_prec_t imag_precision_bits;
};

struct mpc_rounding_override_state {
    bool active;
    mpfr_rnd_t real_rounding_mode;
    mpfr_rnd_t imag_rounding_mode;
};

inline mpc_precision_override_state& mpc_precision_override_storage() noexcept
{
    static thread_local mpc_precision_override_state state{false, 0, 0};
    return state;
}

inline mpc_rounding_override_state& mpc_rounding_override_storage() noexcept
{
    static thread_local mpc_rounding_override_state state{false, MPFR_RNDN, MPFR_RNDN};
    return state;
}

inline void clear_mpc_precision_override() noexcept
{
    mpc_precision_override_storage() = mpc_precision_override_state{false, 0, 0};
}

inline void clear_mpc_rounding_override() noexcept
{
    mpc_rounding_override_storage() = mpc_rounding_override_state{false, MPFR_RNDN, MPFR_RNDN};
}

inline void set_mpc_precision_override(mpfr_prec_t real_precision, mpfr_prec_t imag_precision) noexcept
{
    mpc_precision_override_storage() = mpc_precision_override_state{true, real_precision, imag_precision};
}

inline void set_mpc_rounding_override(mpfr_rnd_t real_rounding, mpfr_rnd_t imag_rounding) noexcept
{
    mpc_rounding_override_storage() = mpc_rounding_override_state{true, real_rounding, imag_rounding};
}

inline void reload_mpc_defaults_from_environment()
{
    const auto loaded = load_mpc_defaults_from_environment();
    if (valid_mpc_precision_environment_override_is_present()) {
        set_mpc_precision_override(loaded.real_precision_bits, loaded.imag_precision_bits);
    } else {
        clear_mpc_precision_override();
    }
    if (valid_mpc_rounding_environment_override_is_present()) {
        set_mpc_rounding_override(loaded.real_rounding_mode, loaded.imag_rounding_mode);
    } else {
        clear_mpc_rounding_override();
    }
    mpc_defaults_initialized_storage() = true;
}

inline void initialize_mpc_defaults_for_current_thread()
{
    auto& initialized = mpc_defaults_initialized_storage();
    if (initialized) {
        return;
    }

    if (mpc_environment_is_present()) {
        reload_mpc_defaults_from_environment();
        return;
    }

    initialized = true;
}

inline mpc_default_options default_mpc_options()
{
    initialize_mpc_defaults_for_current_thread();
    const mpfr_prec_t inherited_precision = default_precision_bits();
    const mpfr_rnd_t inherited_rounding = default_rounding_mode();
    const auto& precision_override = mpc_precision_override_storage();
    const auto& rounding_override = mpc_rounding_override_storage();
    return mpc_default_options{
        precision_override.active ? precision_override.real_precision_bits : inherited_precision,
        precision_override.active ? precision_override.imag_precision_bits : inherited_precision,
        rounding_override.active ? rounding_override.real_rounding_mode : inherited_rounding,
        rounding_override.active ? rounding_override.imag_rounding_mode : inherited_rounding,
    };
}

inline mpfr_prec_t default_mpc_real_precision_bits()
{
    initialize_mpc_defaults_for_current_thread();
    const auto& precision_override = mpc_precision_override_storage();
    return precision_override.active ? precision_override.real_precision_bits : default_precision_bits();
}

inline mpfr_prec_t default_mpc_imag_precision_bits()
{
    initialize_mpc_defaults_for_current_thread();
    const auto& precision_override = mpc_precision_override_storage();
    return precision_override.active ? precision_override.imag_precision_bits : default_precision_bits();
}

inline mpfr_prec_t default_mpc_precision_bits()
{
    return default_mpc_real_precision_bits();
}

inline void set_default_mpc_precision_bits(mpfr_prec_t precision)
{
    initialize_mpc_defaults_for_current_thread();
    if (::gmpfrxx_mkII::detail::valid_mpfr_precision(precision)) {
        set_mpc_precision_override(precision, precision);
    }
}

inline void set_default_mpc_precision_bits(mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
{
    initialize_mpc_defaults_for_current_thread();
    if (!::gmpfrxx_mkII::detail::valid_mpfr_precision(real_precision) ||
        !::gmpfrxx_mkII::detail::valid_mpfr_precision(imag_precision)) {
        return;
    }
    set_mpc_precision_override(real_precision, imag_precision);
}

inline mpfr_rnd_t default_mpc_real_rounding_mode()
{
    initialize_mpc_defaults_for_current_thread();
    const auto& rounding_override = mpc_rounding_override_storage();
    return rounding_override.active ? rounding_override.real_rounding_mode
                                    : ::gmpfrxx_mkII::detail::current_mpfr_rounding_mode();
}

inline mpfr_rnd_t default_mpc_imag_rounding_mode()
{
    initialize_mpc_defaults_for_current_thread();
    const auto& rounding_override = mpc_rounding_override_storage();
    return rounding_override.active ? rounding_override.imag_rounding_mode
                                    : ::gmpfrxx_mkII::detail::current_mpfr_rounding_mode();
}

inline mpc_rnd_t default_mpc_rounding_mode()
{
    initialize_mpc_defaults_for_current_thread();
    const auto& rounding_override = mpc_rounding_override_storage();
    if (rounding_override.active) {
        return MPC_RND(rounding_override.real_rounding_mode, rounding_override.imag_rounding_mode);
    }
    const mpfr_rnd_t rounding = ::gmpfrxx_mkII::detail::current_mpfr_rounding_mode();
    return MPC_RND(rounding, rounding);
}

inline void set_default_mpc_rounding_mode(mpfr_rnd_t rounding)
{
    initialize_mpc_defaults_for_current_thread();
    set_mpc_rounding_override(rounding, rounding);
}

inline void set_default_mpc_rounding_mode(mpfr_rnd_t real_rounding, mpfr_rnd_t imag_rounding)
{
    initialize_mpc_defaults_for_current_thread();
    set_mpc_rounding_override(real_rounding, imag_rounding);
}

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPC_ENVIRONMENT_HPP
