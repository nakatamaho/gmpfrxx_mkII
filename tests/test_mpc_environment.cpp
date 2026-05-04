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
    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "144", 1);
    setenv("MPFRXX_ROUNDING_MODE", "RNDD", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    unsetenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_REAL_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_IMAG_PRECISION_BITS");
    unsetenv("MPFRXX_MPC_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_REAL_ROUNDING_MODE");
    unsetenv("MPFRXX_MPC_IMAG_ROUNDING_MODE");
    mpfrxx::reload_mpc_defaults_from_environment();

    auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 144 || defaults.imag_precision_bits != 144) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDD || defaults.imag_rounding_mode != MPFR_RNDD) {
        std::abort();
    }

    setenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS", "160", 1);
    setenv("MPFRXX_MPC_REAL_PRECISION_BITS", "192", 1);
    setenv("MPFRXX_MPC_IMAG_PRECISION_BITS", "224", 1);
    setenv("MPFRXX_MPC_ROUNDING_MODE", "RNDU", 1);
    setenv("MPFRXX_MPC_REAL_ROUNDING_MODE", "RNDD", 1);
    setenv("MPFRXX_MPC_IMAG_ROUNDING_MODE", "RNDZ", 1);
    mpfrxx::reload_mpc_defaults_from_environment();

    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 224) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDD || defaults.imag_rounding_mode != MPFR_RNDZ) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RND(MPFR_RNDD, MPFR_RNDZ)) {
        std::abort();
    }

    mpfrxx::mpc_class value;
    if (value.real_precision() != 192 || value.imag_precision() != 224) {
        std::abort();
    }

    return 0;
}
