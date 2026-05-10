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

#include <atomic>
#include <cstdlib>
#include <thread>
#include <vector>

namespace {

void require_wrapper_defaults_are_initialized_per_thread()
{
    mpfrxx::set_default_precision_bits(257);

    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            mpfrxx::mpfr_class value;
            if (value.precision() != 512 ||
                mpfrxx::default_precision_bits() != 512 ||
                mpfr_get_default_prec() != 512) {
                ++mismatches;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    if (mpfrxx::default_precision_bits() != 257) {
        ++mismatches;
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
}

void require_default_precision_is_thread_local()
{
    constexpr mpfr_prec_t main_precision = 333;
    constexpr mpfr_prec_t worker_precision = 777;
    mpfrxx::set_default_precision_bits(main_precision);
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);

    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            mpfrxx::set_default_precision_bits(worker_precision);
            mpfrxx::mpfr_class value;
            if (value.precision() != worker_precision ||
                mpfr_get_default_prec() != worker_precision) {
                ++mismatches;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    if (mpfrxx::default_precision_bits() != main_precision) {
        ++mismatches;
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
}

void require_default_options_are_thread_local()
{
    std::atomic<int> mismatches{0};
    mpfrxx::set_default_precision_bits(257);
    mpfrxx::set_default_rounding_mode(MPFR_RNDU);
    mpfrxx::set_default_exponent_range(-40, 40);

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&, i] {
            const mpfr_prec_t precision = (i % 2) == 0 ? 193 : 307;
            const mpfr_rnd_t rounding = (i % 2) == 0 ? MPFR_RNDU : MPFR_RNDD;
            const mpfr_exp_t bound = (i % 2) == 0 ? 40 : 80;
            mpfrxx::set_default_precision_bits(precision);
            mpfrxx::set_default_rounding_mode(rounding);
            mpfrxx::set_default_exponent_range(-bound, bound);
            for (int j = 0; j < 200; ++j) {
                const auto options = mpfrxx::default_options();
                if (options.precision_bits != precision ||
                    options.rounding_mode != rounding ||
                    options.emin != -bound ||
                    options.emax != bound) {
                    ++mismatches;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    const auto main_options = mpfrxx::default_options();
    if (main_options.precision_bits != 257 ||
        main_options.rounding_mode != MPFR_RNDU ||
        main_options.emin != -40 ||
        main_options.emax != 40) {
        ++mismatches;
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
}

void require_concurrent_default_options_access()
{
    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&, i] {
            const mpfr_prec_t precision = (i % 2) == 0 ? 257 : 769;
            if ((i % 2) == 0) {
                mpfrxx::set_default_precision_bits(precision);
                mpfrxx::set_default_rounding_mode(MPFR_RNDU);
                mpfrxx::set_default_exponent_range(-40, 40);
            } else {
                mpfrxx::set_default_precision_bits(precision);
                mpfrxx::set_default_rounding_mode(MPFR_RNDD);
                mpfrxx::set_default_exponent_range(-80, 80);
            }
            for (int j = 0; j < 2000; ++j) {
                const auto options = mpfrxx::default_options();
                const bool first =
                    options.precision_bits == 257 &&
                    options.rounding_mode == MPFR_RNDU &&
                    options.emin == -40 &&
                    options.emax == 40;
                const bool second =
                    options.precision_bits == 769 &&
                    options.rounding_mode == MPFR_RNDD &&
                    options.emin == -80 &&
                    options.emax == 80;
                if (!first && !second) {
                    ++mismatches;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
}

void require_default_precision_routes_to_mpfr_tls()
{
    const mpfr_prec_t original_default = mpfr_get_default_prec();
    constexpr mpfr_prec_t first_precision = 192;
    constexpr mpfr_prec_t second_precision = 4096;

    mpfrxx::set_default_precision_bits(first_precision);
    if (mpfr_get_default_prec() != first_precision) {
        std::abort();
    }

    mpfr_set_default_prec(second_precision);

    mpfrxx::mpfr_class value;
    if (value.precision() != second_precision ||
        mpfrxx::default_precision_bits() != second_precision) {
        std::abort();
    }

    mpfr_set_default_prec(original_default);
}

void require_parallel_expression_materialization()
{
    const mpfrxx::mpfr_class a("1.5", 160);
    const mpfrxx::mpfr_class b("2.5", 224);
    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            const mpfrxx::mpfr_class value = (a + b) * mpfrxx::mpfr_class("3", 224);
            if (value.precision() != 224 || mpfr_cmp_ui(value.get_mpfr_t(), 12) != 0) {
                ++mismatches;
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
}

void require_parallel_exponent_range_defaults_route_to_mpfr_tls()
{
    std::atomic<int> mismatches{0};
    const mpfr_exp_t old_emin = mpfr_get_emin();
    const mpfr_exp_t old_emax = mpfr_get_emax();
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&, i] {
            for (int j = 0; j < 200; ++j) {
                if (((i + j) % 2) == 0) {
                    mpfrxx::set_default_exponent_range(-40, 40);
                } else {
                    mpfrxx::set_default_exponent_range(-80, 80);
                }
                const mpfr_exp_t expected_bound = ((i + j) % 2) == 0 ? 40 : 80;
                const mpfrxx::mpfr_class a("1.25", 192);
                const mpfrxx::mpfr_class b("2.5", 192);
                const mpfrxx::mpfr_class value = a * b + mpfrxx::mpfr_class("0.875", 192);
                if (value.precision() != 192 ||
                    mpfr_cmp_d(value.get_mpfr_t(), 4.0) != 0 ||
                    mpfr_get_emin() != -expected_bound ||
                    mpfr_get_emax() != expected_bound) {
                    ++mismatches;
                }
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
    if (mpfr_get_emin() != old_emin || mpfr_get_emax() != old_emax) {
        std::abort();
    }
}

} // namespace

int main()
{
    require_wrapper_defaults_are_initialized_per_thread();
    require_default_precision_is_thread_local();
    require_default_options_are_thread_local();
    require_concurrent_default_options_access();
    require_default_precision_routes_to_mpfr_tls();
    require_parallel_expression_materialization();
    require_parallel_exponent_range_defaults_route_to_mpfr_tls();
    return 0;
}
