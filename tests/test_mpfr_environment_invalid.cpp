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

#include <cstdlib>

int main()
{
    const mpfr_exp_t initial_emin = mpfr_get_emin();
    const mpfr_exp_t initial_emax = mpfr_get_emax();

    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "0", 1);
    setenv("MPFRXX_EMIN", "100", 1);
    setenv("MPFRXX_EMAX", "-100", 1);
    setenv("MPFRXX_ROUNDING_MODE", "SIDEWAYS", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    const auto invalid_defaults = mpfrxx::default_options();
    if (invalid_defaults.precision_bits != 512) {
        std::abort();
    }
    if (invalid_defaults.rounding_mode != MPFR_RNDN) {
        std::abort();
    }
    if (invalid_defaults.emin != initial_emin || invalid_defaults.emax != initial_emax) {
        std::abort();
    }

    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "96", 1);
    setenv("MPFRXX_EMIN", "-200", 1);
    setenv("MPFRXX_EMAX", "200", 1);
    setenv("MPFRXX_ROUNDING_MODE", "MPFR_RNDD", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    const auto valid_defaults = mpfrxx::default_options();
    if (valid_defaults.precision_bits != 96 ||
        valid_defaults.emin != -200 ||
        valid_defaults.emax != 200 ||
        valid_defaults.rounding_mode != MPFR_RNDD) {
        std::abort();
    }

    return 0;
}
