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

#include <atomic>
#include <cstdlib>
#include <thread>
#include <vector>

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

    {
        std::atomic<bool> stop{false};
        std::atomic<int> mismatches{0};
        std::thread writer([&] {
            for (int i = 0; i < 2000; ++i) {
                if ((i % 2) == 0) {
                    mpfrxx::set_default_mpc_precision_bits(193, 257);
                } else {
                    mpfrxx::set_default_mpc_precision_bits(307, 409);
                }
            }
            stop.store(true, std::memory_order_release);
        });

        std::vector<std::thread> readers;
        for (int i = 0; i < 8; ++i) {
            readers.emplace_back([&] {
                while (!stop.load(std::memory_order_acquire)) {
                    const auto options = mpfrxx::default_mpc_options();
                    const bool initial =
                        options.real_precision_bits == 192 &&
                        options.imag_precision_bits == 224 &&
                        options.real_rounding_mode == MPFR_RNDD &&
                        options.imag_rounding_mode == MPFR_RNDZ;
                    const bool first =
                        options.real_precision_bits == 193 &&
                        options.imag_precision_bits == 257 &&
                        options.real_rounding_mode == MPFR_RNDD &&
                        options.imag_rounding_mode == MPFR_RNDZ;
                    const bool second =
                        options.real_precision_bits == 307 &&
                        options.imag_precision_bits == 409 &&
                        options.real_rounding_mode == MPFR_RNDD &&
                        options.imag_rounding_mode == MPFR_RNDZ;
                    if (!initial && !first && !second) {
                        ++mismatches;
                    }
                }
            });
        }

        writer.join();
        for (auto& thread : readers) {
            thread.join();
        }
        if (mismatches.load() != 0) {
            std::abort();
        }

        mpfrxx::set_default_mpc_precision_bits(192, 224);
        mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDD, MPFR_RNDZ);
    }

    mpfrxx::mpc_class value;
    if (value.real_precision() != 192 || value.imag_precision() != 224) {
        std::abort();
    }

    const mpfr_exp_t old_emin = mpfr_get_emin();
    const mpfr_exp_t old_emax = mpfr_get_emax();
    const mpfr_exp_t old_default_emin = mpfrxx::default_emin();
    const mpfr_exp_t old_default_emax = mpfrxx::default_emax();

    mpfrxx::set_default_exponent_range(old_emin, old_emax);
    mpfrxx::mpc_class finite_argument = mpfrxx::mpc_class::with_precision(128, 100.0, 0.0);
    mpfrxx::set_default_exponent_range(-20, 20);
    bool saw_mpc_math_guard = false;
    mpfrxx::mpc_class guarded_result = mpfrxx::detail::unary_mpc_math(
        finite_argument,
        [&saw_mpc_math_guard](mpc_t rop, const mpc_t op, mpc_rnd_t rnd) {
            saw_mpc_math_guard = (mpfr_get_emin() == -20 && mpfr_get_emax() == 20);
            mpc_set(rop, op, rnd);
        });
    (void)guarded_result;
    if (!saw_mpc_math_guard) {
        std::abort();
    }

    if (mpfr_get_emin() != old_emin || mpfr_get_emax() != old_emax) {
        std::abort();
    }
    mpfrxx::set_default_exponent_range(old_default_emin, old_default_emax);

    return 0;
}
