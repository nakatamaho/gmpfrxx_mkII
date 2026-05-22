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
#include <string>

int main()
{
    mpfrxx::set_default_precision_bits(128);
    mpfrxx::set_default_rounding_mode(MPFR_RNDA);

    setenv("MPCXX_DEFAULT_PRECISION_BITS", "0", 1);
    setenv("MPCXX_DEFAULT_REAL_PRECISION_BITS", "not-a-number", 1);
    setenv("MPCXX_DEFAULT_IMAG_PRECISION_BITS", "0", 1);
    setenv("MPCXX_DEFAULT_ROUNDING_MODE", "SIDEWAYS", 1);
    setenv("MPCXX_DEFAULT_REAL_ROUNDING_MODE", "NOPE", 1);
    setenv("MPCXX_DEFAULT_IMAG_ROUNDING_MODE", "", 1);
    mpfrxx::reload_mpc_defaults_from_environment();

    const auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 128 || defaults.imag_precision_bits != 128) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDA || defaults.imag_rounding_mode != MPFR_RNDA) {
        std::abort();
    }

    if (MPFR_PREC_MAX < std::numeric_limits<mpfr_prec_t>::max()) {
        const std::string too_large_precision =
            std::to_string(static_cast<unsigned long long>(MPFR_PREC_MAX) + 1ull);
        setenv("MPCXX_DEFAULT_PRECISION_BITS", too_large_precision.c_str(), 1);
        setenv("MPCXX_DEFAULT_REAL_PRECISION_BITS", too_large_precision.c_str(), 1);
        setenv("MPCXX_DEFAULT_IMAG_PRECISION_BITS", too_large_precision.c_str(), 1);
        unsetenv("MPCXX_DEFAULT_ROUNDING_MODE");
        unsetenv("MPCXX_DEFAULT_REAL_ROUNDING_MODE");
        unsetenv("MPCXX_DEFAULT_IMAG_ROUNDING_MODE");
        mpfrxx::reload_mpc_defaults_from_environment();

        const auto oversized_defaults = mpfrxx::default_mpc_options();
        if (oversized_defaults.real_precision_bits != 128 ||
            oversized_defaults.imag_precision_bits != 128) {
            std::abort();
        }
    }

    return 0;
}
