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
 */

#ifndef GMPFRXX_MKII_BENCHMARK_MPF_INIT_COUNTER_HPP
#define GMPFRXX_MKII_BENCHMARK_MPF_INIT_COUNTER_HPP

#include <cstdint>
#include <iostream>
#include <gmp.h>

#if defined(GMPFRXX_MKII_BENCHMARK_COUNT_MPF_INIT)

#undef mpf_init
#undef mpf_init2
#undef mpf_clear

namespace benchmark_mpf_init_counter {

struct counters {
    std::uint64_t init = 0;
    std::uint64_t init2 = 0;
    std::uint64_t clear = 0;
};

inline counters &global_counters() {
    static counters value;
    return value;
}

inline void counted_mpf_init(mpf_ptr value) {
    ++global_counters().init;
    __gmpf_init(value);
}

inline void counted_mpf_init2(mpf_ptr value, mp_bitcnt_t precision) {
    ++global_counters().init2;
    __gmpf_init2(value, precision);
}

inline void counted_mpf_clear(mpf_ptr value) {
    ++global_counters().clear;
    __gmpf_clear(value);
}

inline void print(const char *label) {
    const counters snapshot = global_counters();
    std::cout << "MPF_INIT_COUNTS label=" << label
              << " init=" << snapshot.init
              << " init2=" << snapshot.init2
              << " total_init=" << (snapshot.init + snapshot.init2)
              << " clear=" << snapshot.clear << std::endl;
}

} // namespace benchmark_mpf_init_counter

#define mpf_init(value) ::benchmark_mpf_init_counter::counted_mpf_init((value))
#define mpf_init2(value, precision) ::benchmark_mpf_init_counter::counted_mpf_init2((value), (precision))
#define mpf_clear(value) ::benchmark_mpf_init_counter::counted_mpf_clear((value))

#else

namespace benchmark_mpf_init_counter {

inline void print(const char *) {}

} // namespace benchmark_mpf_init_counter

#endif

#endif // GMPFRXX_MKII_BENCHMARK_MPF_INIT_COUNTER_HPP
