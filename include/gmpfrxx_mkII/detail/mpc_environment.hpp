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

#include <algorithm>
#include <cstdlib>

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
    const auto inherited = default_options();
    mpfr_prec_t precision = inherited.precision_bits;
    ::gmpfrxx_mkII::detail::parse_mpfr_precision(
        std::getenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS"), precision);

    mpfr_prec_t real_precision = precision;
    const bool has_real_precision =
        ::gmpfrxx_mkII::detail::parse_mpfr_precision(
            std::getenv("MPFRXX_MPC_REAL_PRECISION_BITS"), real_precision);

    mpfr_prec_t imag_precision = precision;
    const bool has_imag_precision =
        ::gmpfrxx_mkII::detail::parse_mpfr_precision(
            std::getenv("MPFRXX_MPC_IMAG_PRECISION_BITS"), imag_precision);

    if (has_real_precision && has_imag_precision) {
        precision = std::max(real_precision, imag_precision);
    } else if (has_real_precision) {
        precision = real_precision;
    } else if (has_imag_precision) {
        precision = imag_precision;
    }

    mpfr_rnd_t rounding = inherited.rounding_mode;
    ::gmpfrxx_mkII::detail::parse_mpfr_rounding(
        std::getenv("MPFRXX_MPC_ROUNDING_MODE"), rounding);

    mpfr_rnd_t real_rounding = rounding;
    const bool has_real_rounding =
        ::gmpfrxx_mkII::detail::parse_mpfr_rounding(
            std::getenv("MPFRXX_MPC_REAL_ROUNDING_MODE"), real_rounding);

    mpfr_rnd_t imag_rounding = rounding;
    const bool has_imag_rounding =
        ::gmpfrxx_mkII::detail::parse_mpfr_rounding(
            std::getenv("MPFRXX_MPC_IMAG_ROUNDING_MODE"), imag_rounding);

    if (has_real_rounding && has_imag_rounding && real_rounding == imag_rounding) {
        rounding = real_rounding;
    } else if (has_real_rounding && !has_imag_rounding) {
        rounding = real_rounding;
    } else if (!has_real_rounding && has_imag_rounding) {
        rounding = imag_rounding;
    }

    return mpc_default_options{
        precision,
        precision,
        rounding,
        rounding,
    };
}

inline void reload_mpc_defaults_from_environment()
{
    const auto loaded = load_mpc_defaults_from_environment();
    set_default_precision_bits(loaded.real_precision_bits);
    set_default_rounding_mode(loaded.real_rounding_mode);
}

inline mpc_default_options default_mpc_options()
{
    const auto inherited = default_options();
    return mpc_default_options{
        inherited.precision_bits,
        inherited.precision_bits,
        inherited.rounding_mode,
        inherited.rounding_mode,
    };
}

inline mpfr_prec_t default_mpc_real_precision_bits()
{
    return default_precision_bits();
}

inline mpfr_prec_t default_mpc_imag_precision_bits()
{
    return default_precision_bits();
}

inline mpfr_prec_t default_mpc_precision_bits()
{
    return default_precision_bits();
}

inline void set_default_mpc_precision_bits(mpfr_prec_t precision)
{
    set_default_precision_bits(precision);
}

inline void set_default_mpc_precision_bits(mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
{
    if (::gmpfrxx_mkII::detail::valid_mpfr_precision(real_precision) &&
        ::gmpfrxx_mkII::detail::valid_mpfr_precision(imag_precision)) {
        set_default_precision_bits(std::max(real_precision, imag_precision));
    }
}

inline mpfr_rnd_t default_mpc_real_rounding_mode()
{
    return default_rounding_mode();
}

inline mpfr_rnd_t default_mpc_imag_rounding_mode()
{
    return default_rounding_mode();
}

inline mpc_rnd_t default_mpc_rounding_mode()
{
    const auto rounding = default_rounding_mode();
    return MPC_RND(rounding, rounding);
}

inline void set_default_mpc_rounding_mode(mpfr_rnd_t rounding)
{
    set_default_rounding_mode(rounding);
}

inline void set_default_mpc_rounding_mode(mpfr_rnd_t real_rounding, mpfr_rnd_t imag_rounding)
{
    if (real_rounding == imag_rounding) {
        set_default_rounding_mode(real_rounding);
    }
}

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPC_ENVIRONMENT_HPP
