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
    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "160", 1);
    setenv("MPFRXX_DEFAULT_EMIN", "-64", 1);
    setenv("MPFRXX_DEFAULT_EMAX", "64", 1);
    setenv("MPFRXX_DEFAULT_ROUNDING_MODE", "RNDU", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    const auto defaults = mpfrxx::default_options();
    if (defaults.precision_bits != 160) {
        std::abort();
    }
    if (defaults.emin != -64 || defaults.emax != 64) {
        std::abort();
    }
    if (defaults.rounding_mode != MPFR_RNDU) {
        std::abort();
    }

    mpfrxx::mpfr_class default_prec_value;
    if (default_prec_value.precision() != 160) {
        std::abort();
    }

    auto rounded_up = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded_up.to_double() != 1.5) {
        std::abort();
    }

    const mpfr_exp_t old_emin = mpfr_get_emin();
    const mpfr_exp_t old_emax = mpfr_get_emax();
    auto x = mpfrxx::mpfr_class::with_precision(128, 2.0);
    auto y = mpfrxx::mpfr_class::with_precision(128, 3.0);
    mpfrxx::mpfr_class z = x + y;
    (void)z;
    if (mpfr_get_emin() != old_emin || mpfr_get_emax() != old_emax) {
        std::abort();
    }

    mpfr_set_default_prec(53);
    if (mpfr_set_emax(MPFR_EMAX_DEFAULT) != 0 ||
        mpfr_set_emin(MPFR_EMIN_DEFAULT) != 0) {
        std::abort();
    }
    mpfr_set_default_rounding_mode(MPFR_RNDN);

    const auto raw_reset_defaults = mpfrxx::default_options();
    if (raw_reset_defaults.precision_bits != 53) {
        std::abort();
    }
    if (raw_reset_defaults.emin != MPFR_EMIN_DEFAULT ||
        raw_reset_defaults.emax != MPFR_EMAX_DEFAULT) {
        std::abort();
    }
    if (raw_reset_defaults.rounding_mode != MPFR_RNDN) {
        std::abort();
    }

    mpfrxx::mpfr_class raw_reset_value;
    if (raw_reset_value.precision() != 53) {
        std::abort();
    }

    return 0;
}
