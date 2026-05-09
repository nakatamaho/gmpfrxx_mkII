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

#include <gmpxx_mkII.h>

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>

namespace {

void require_default_precision_is_user_managed_global()
{
    gmpxx::set_default_mpf_precision_bits(777);

    std::mutex mutex;
    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            std::lock_guard<std::mutex> lock(mutex);
            if (gmpxx::default_mpf_precision_bits() < 777) {
                ++mismatches;
            }
            gmpxx::set_default_mpf_precision_bits(333);
            gmpxx::mpf_class value;
            const mp_bitcnt_t effective = gmpxx::default_mpf_precision_bits();
            if (effective < 333 || mpf_get_default_prec() != effective ||
                value.precision() < effective) {
                ++mismatches;
            }
            gmpxx::set_default_mpf_precision_bits(777);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    if (mismatches.load() != 0) {
        std::abort();
    }
    if (gmpxx::default_mpf_precision_bits() < 777) {
        std::abort();
    }
}

void require_parallel_default_precision_mutation_is_user_managed()
{
    std::mutex mutex;
    std::atomic<int> mismatches{0};
    gmpxx::set_default_mpf_precision_bits(641);

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&, i] {
            const mp_bitcnt_t first = static_cast<mp_bitcnt_t>(257 + i);
            const mp_bitcnt_t second = static_cast<mp_bitcnt_t>(769 + i);
            for (int j = 0; j < 2000; ++j) {
                const mp_bitcnt_t expected = (j % 2) == 0 ? first : second;
                std::lock_guard<std::mutex> lock(mutex);
                gmpxx::set_default_mpf_precision_bits(expected);
                const mp_bitcnt_t observed = gmpxx::default_mpf_precision_bits();
                if (observed < expected || mpf_get_default_prec() != observed) {
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

void require_routes_to_gmp_global_default()
{
    const mp_bitcnt_t original_gmp_default = mpf_get_default_prec();
    constexpr mp_bitcnt_t gmp_global_precision = 4096;

    gmpxx::set_default_mpf_precision_bits(192);
    if (mpf_get_default_prec() < 192) {
        std::abort();
    }

    mpf_set_default_prec(gmp_global_precision);

    gmpxx::mpf_class value;
    if (gmpxx::default_mpf_precision_bits() != gmp_global_precision ||
        value.precision() < gmp_global_precision) {
        std::abort();
    }

    mpf_set_default_prec(original_gmp_default);
}

void require_parallel_expression_materialization()
{
    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&] {
            const gmpxx::mpf_class a("1.5", 160);
            const gmpxx::mpf_class b("2.5", 224);
            const gmpxx::mpf_class value = (a + b) * gmpxx::mpf_class("3", 224);
            if (value.precision() < 224 || value.get_str() != "12") {
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
    require_default_precision_is_user_managed_global();
    require_parallel_default_precision_mutation_is_user_managed();
    require_routes_to_gmp_global_default();
    require_parallel_expression_materialization();
    return 0;
}
