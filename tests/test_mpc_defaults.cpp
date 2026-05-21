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

#include <mpfrxx_mkII.h>
#include <mpcxx_mkII.h>

#include <cstdlib>
#include <limits>
#include <stdexcept>

namespace {

void clear_mpc_environment()
{
    unsetenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_REAL_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_IMAG_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_REAL_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_IMAG_ROUNDING_MODE");
}

} // namespace

int main()
{
    clear_mpc_environment();

    mpfrxx::set_default_precision_bits(512);
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    mpfrxx::reload_mpc_defaults_from_environment();

    auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 512 || defaults.imag_precision_bits != 512) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDN || defaults.imag_rounding_mode != MPFR_RNDN) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RNDNN) {
        std::abort();
    }

    mpfrxx::set_default_mpc_precision_bits(160, 192);
    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 160 || defaults.imag_precision_bits != 192) {
        std::abort();
    }
    {
        mpfrxx::mpc_class asymmetric_value;
        if (asymmetric_value.real_precision() != 160 || asymmetric_value.imag_precision() != 192) {
            std::abort();
        }
    }

    if (mpfrxx::default_precision_bits() != 512 || mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        std::abort();
    }

    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDU);
    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_rounding_mode != MPFR_RNDU || defaults.imag_rounding_mode != MPFR_RNDU) {
        std::abort();
    }
    if (mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        std::abort();
    }

    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDU, MPFR_RNDD);
    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_rounding_mode != MPFR_RNDU || defaults.imag_rounding_mode != MPFR_RNDD) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RND(MPFR_RNDU, MPFR_RNDD)) {
        std::abort();
    }

    mpfrxx::set_default_mpc_precision_bits(192, 192);
    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDU, MPFR_RNDU);

    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 192) {
        std::abort();
    }
    if (mpfrxx::default_mpc_precision_bits() != 192) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDU || defaults.imag_rounding_mode != MPFR_RNDU) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RND(MPFR_RNDU, MPFR_RNDU)) {
        std::abort();
    }
    if (mpfrxx::default_precision_bits() != 512 || mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        std::abort();
    }

    mpfrxx::mpc_class value;
    if (value.real_precision() != 192 || value.imag_precision() != 192) {
        std::abort();
    }
    if (value.precision() != 192) {
        std::abort();
    }

    if (MPFR_PREC_MAX < std::numeric_limits<mpfr_prec_t>::max()) {
        const auto too_large_precision =
            static_cast<mpfr_prec_t>(static_cast<unsigned long long>(MPFR_PREC_MAX) + 1ull);
        mpfrxx::set_default_mpc_precision_bits(too_large_precision);
        defaults = mpfrxx::default_mpc_options();
        if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 192) {
            std::abort();
        }

        mpfrxx::set_default_mpc_precision_bits(
            too_large_precision,
            too_large_precision);
        defaults = mpfrxx::default_mpc_options();
        if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 192) {
            std::abort();
        }
    }

    return 0;
}
