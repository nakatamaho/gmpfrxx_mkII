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

void require_default_visible_to_threads()
{
    constexpr mpfr_prec_t precision = 333;
    mpfrxx::set_default_precision_bits(precision);
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);

    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            mpfrxx::mpfr_class value;
            if (value.precision() != precision) {
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

void require_concurrent_default_options_access()
{
    std::atomic<bool> stop{false};
    std::atomic<int> mismatches{0};
    mpfrxx::set_default_precision_bits(257);
    mpfrxx::set_default_rounding_mode(MPFR_RNDU);
    mpfrxx::set_default_exponent_range(-40, 40);
    std::thread writer([&] {
        for (int i = 0; i < 2000; ++i) {
            if ((i % 2) == 0) {
                mpfrxx::set_default_exponent_range(-40, 40);
            } else {
                mpfrxx::set_default_exponent_range(-80, 80);
            }
        }
        stop.store(true, std::memory_order_release);
    });

    std::vector<std::thread> readers;
    for (int i = 0; i < 8; ++i) {
        readers.emplace_back([&] {
            while (!stop.load(std::memory_order_acquire)) {
                const auto options = mpfrxx::default_options();
                const bool first =
                    options.precision_bits == 257 &&
                    options.rounding_mode == MPFR_RNDU &&
                    options.emin == -40 &&
                    options.emax == 40;
                const bool second =
                    options.precision_bits == 257 &&
                    options.rounding_mode == MPFR_RNDU &&
                    options.emin == -80 &&
                    options.emax == 80;
                if (!first && !second) {
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
}

void require_isolation_from_mpfr_global_default()
{
    const mpfr_prec_t original_mpfr_default = mpfr_get_default_prec();
    constexpr mpfr_prec_t wrapper_precision = 192;
    constexpr mpfr_prec_t mpfr_global_precision = 4096;

    mpfrxx::set_default_precision_bits(wrapper_precision);
    mpfr_set_default_prec(mpfr_global_precision);

    mpfrxx::mpfr_class value;
    if (value.precision() != wrapper_precision) {
        std::abort();
    }

    mpfr_set_default_prec(original_mpfr_default);
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

} // namespace

int main()
{
    require_default_visible_to_threads();
    require_concurrent_default_options_access();
    require_isolation_from_mpfr_global_default();
    require_parallel_expression_materialization();
    return 0;
}
