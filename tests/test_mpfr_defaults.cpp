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
#include <limits>
#include <string>

namespace {

void clear_mpfr_environment()
{
    unsetenv("MPFRXX_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_EMIN");
    unsetenv("MPFRXX_EMAX");
    unsetenv("MPFRXX_ROUNDING_MODE");
    mpfrxx::reload_mpfr_defaults_from_environment();
}

} // namespace

int main()
{
    const auto old_defaults = mpfrxx::default_options();

    mpfrxx::set_default_precision_bits(512);
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    clear_mpfr_environment();

    if (mpfrxx::default_precision_bits() != 512) {
        std::abort();
    }
    if (mpfrxx::default_prec() != mpfrxx::default_precision_bits()) {
        std::abort();
    }
    if (mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        std::abort();
    }

    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "384", 1);
    setenv("MPFRXX_ROUNDING_MODE", "MPFR_RNDA", 1);
    setenv("MPFRXX_EMIN", "-30", 1);
    setenv("MPFRXX_EMAX", "30", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();
    if (mpfrxx::default_precision_bits() != 384 ||
        mpfrxx::default_prec() != 384 ||
        mpfrxx::default_rounding_mode() != MPFR_RNDA ||
        mpfrxx::default_emin() != -30 ||
        mpfrxx::default_emax() != 30) {
        std::abort();
    }

    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "0", 1);
    setenv("MPFRXX_ROUNDING_MODE", "invalid-rounding", 1);
    setenv("MPFRXX_EMIN", "40", 1);
    setenv("MPFRXX_EMAX", "-40", 1);
    const auto before_invalid_environment = mpfrxx::default_options();
    mpfrxx::reload_mpfr_defaults_from_environment();
    if (mpfrxx::default_precision_bits() != before_invalid_environment.precision_bits ||
        mpfrxx::default_rounding_mode() != before_invalid_environment.rounding_mode ||
        mpfrxx::default_emin() == 40 ||
        mpfrxx::default_emax() == -40) {
        std::abort();
    }

    mpfrxx::set_default_precision_bits(113);
    if (mpfr_get_default_prec() != 113) {
        std::abort();
    }
    mpfrxx::mpfr_class default_prec_value;
    if (default_prec_value.precision() != 113) {
        std::abort();
    }

    mpfrxx::set_default_precision_bits(0);
    if (mpfrxx::default_precision_bits() != 113) {
        std::abort();
    }
    if (MPFR_PREC_MAX < std::numeric_limits<mpfr_prec_t>::max()) {
        const auto too_large_precision =
            static_cast<mpfr_prec_t>(static_cast<unsigned long long>(MPFR_PREC_MAX) + 1ull);
        mpfrxx::set_default_precision_bits(too_large_precision);
        if (mpfrxx::default_precision_bits() != 113) {
            std::abort();
        }
    }

    mpfrxx::set_default_rounding_mode(MPFR_RNDU);
    if (mpfr_get_default_rounding_mode() != MPFR_RNDU) {
        std::abort();
    }
    auto rounded_up = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded_up.to_double() != 1.5) {
        std::abort();
    }

    mpfrxx::set_default_rounding_mode(MPFR_RNDD);
    if (mpfr_get_default_rounding_mode() != MPFR_RNDD) {
        std::abort();
    }
    auto rounded_down = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded_down.to_double() != 1.0) {
        std::abort();
    }

    mpfrxx::set_default_exponent_range(-20, 20);
    if (mpfrxx::default_emin() != -20 ||
        mpfrxx::default_emax() != 20 ||
        mpfr_get_emin() != -20 ||
        mpfr_get_emax() != 20) {
        std::abort();
    }
    mpfrxx::set_default_exponent_range(20, -20);
    if (mpfrxx::default_emin() != -20 || mpfrxx::default_emax() != 20) {
        std::abort();
    }
    if (MPFR_EMIN_MIN > std::numeric_limits<mpfr_exp_t>::min()) {
        mpfrxx::set_default_exponent_range(MPFR_EMIN_MIN - 1, 20);
        if (mpfrxx::default_emin() != -20 || mpfrxx::default_emax() != 20) {
            std::abort();
        }
    }
    if (MPFR_EMAX_MAX < std::numeric_limits<mpfr_exp_t>::max()) {
        mpfrxx::set_default_exponent_range(-20, MPFR_EMAX_MAX + 1);
        if (mpfrxx::default_emin() != -20 || mpfrxx::default_emax() != 20) {
            std::abort();
        }
    }

    mpfrxx::set_default_precision_bits(old_defaults.precision_bits);
    mpfrxx::set_default_rounding_mode(old_defaults.rounding_mode);
    mpfrxx::set_default_exponent_range(old_defaults.emin, old_defaults.emax);

    return 0;
}
