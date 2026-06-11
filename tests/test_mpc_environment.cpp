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

#include "test_env.hpp"

#include <mpfrxx_mkII.h>
#include <mpcxx_mkII.h>

#include <atomic>
#include <cstdlib>
#include <stdexcept>
#include <thread>
#include <vector>

int main()
{
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_PRECISION_BITS", "144", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_ROUNDING_MODE", "RNDD", 1);
    mpfrxx::reload_mpfr_defaults_from_environment();

    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_REAL_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_IMAG_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_ROUNDING_MODE");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_REAL_ROUNDING_MODE");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_IMAG_ROUNDING_MODE");
    mpfrxx::reload_mpc_defaults_from_environment();

    auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 144 || defaults.imag_precision_bits != 144) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDD || defaults.imag_rounding_mode != MPFR_RNDD) {
        std::abort();
    }

    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_PRECISION_BITS", "160", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_PRECISION_BITS", "192", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_IMAG_PRECISION_BITS", "224", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_ROUNDING_MODE", "RNDU", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_ROUNDING_MODE", "RNDD", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_IMAG_ROUNDING_MODE", "RNDZ", 1);
    mpfrxx::reload_mpc_defaults_from_environment();
    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 224) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDD || defaults.imag_rounding_mode != MPFR_RNDZ) {
        std::abort();
    }
    if (mpfrxx::default_precision_bits() != 144 || mpfrxx::default_rounding_mode() != MPFR_RNDD) {
        std::abort();
    }

    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_PRECISION_BITS", "224", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_IMAG_PRECISION_BITS", "224", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_ROUNDING_MODE", "RNDU", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_IMAG_ROUNDING_MODE", "RNDU", 1);
    mpfrxx::reload_mpc_defaults_from_environment();

    defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 224 || defaults.imag_precision_bits != 224) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDU || defaults.imag_rounding_mode != MPFR_RNDU) {
        std::abort();
    }
    if (mpfrxx::default_mpc_rounding_mode() != MPC_RND(MPFR_RNDU, MPFR_RNDU)) {
        std::abort();
    }
    if (mpfrxx::default_precision_bits() != 144 ||
        mpfrxx::default_rounding_mode() != MPFR_RNDD) {
        std::abort();
    }

    {
        std::atomic<int> mismatches{0};
        mpfrxx::set_default_mpc_precision_bits(192, 224);
        defaults = mpfrxx::default_mpc_options();
        if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 224) {
            std::abort();
        }
        mpfrxx::set_default_mpc_precision_bits(224, 224);
        mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDZ);

        std::vector<std::thread> threads;
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back([&, i] {
                const mpfr_prec_t real_precision = (i % 2) == 0 ? 193 : 307;
                const mpfr_rnd_t rounding = (i % 2) == 0 ? MPFR_RNDU : MPFR_RNDD;
                mpfrxx::set_default_mpc_precision_bits(real_precision, real_precision);
                mpfrxx::set_default_mpc_rounding_mode(rounding);
                for (int j = 0; j < 2000; ++j) {
                    const auto options = mpfrxx::default_mpc_options();
                    if (options.real_precision_bits != real_precision ||
                        options.imag_precision_bits != real_precision ||
                        options.real_rounding_mode != rounding ||
                        options.imag_rounding_mode != rounding) {
                        ++mismatches;
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
        defaults = mpfrxx::default_mpc_options();
        if (defaults.real_precision_bits != 224 ||
            defaults.imag_precision_bits != 224 ||
            defaults.real_rounding_mode != MPFR_RNDZ ||
            defaults.imag_rounding_mode != MPFR_RNDZ) {
            ++mismatches;
        }
        if (mismatches.load() != 0) {
            std::abort();
        }
    }

    mpfrxx::mpc_class value;
    if (value.real_precision() != 224 || value.imag_precision() != 224) {
        std::abort();
    }

    const mpfr_exp_t old_emin = mpfr_get_emin();
    const mpfr_exp_t old_emax = mpfr_get_emax();
    const mpfr_exp_t old_default_emin = mpfrxx::default_emin();
    const mpfr_exp_t old_default_emax = mpfrxx::default_emax();

    mpfrxx::set_default_exponent_range(old_emin, old_emax);
    mpfrxx::mpc_class finite_argument = mpfrxx::mpc_class::with_precision(128, 100.0, 0.0);
    mpfrxx::set_default_exponent_range(-20, 20);
    bool saw_mpfr_tls_range = false;
    mpfrxx::mpc_class result = mpfrxx::detail::unary_mpc_math(
        finite_argument,
        [&saw_mpfr_tls_range](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
            saw_mpfr_tls_range = (mpfr_get_emin() == -20 && mpfr_get_emax() == 20);
            mpc_set(rop, op, rnd);
        });
    (void)result;
    if (!saw_mpfr_tls_range) {
        std::abort();
    }

    if (mpfr_get_emin() != -20 || mpfr_get_emax() != 20) {
        std::abort();
    }
    mpfrxx::set_default_exponent_range(old_default_emin, old_default_emax);

    return 0;
}
