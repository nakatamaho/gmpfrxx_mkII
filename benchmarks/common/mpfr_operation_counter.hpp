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

#ifndef GMPFRXX_MKII_BENCHMARK_MPFR_OPERATION_COUNTER_HPP
#define GMPFRXX_MKII_BENCHMARK_MPFR_OPERATION_COUNTER_HPP

#include <cstdint>
#include <iostream>
#include <atomic>
#include <mpfr.h>

#if defined(GMPFRXX_MKII_BENCHMARK_COUNT_MPFR_OPERATIONS)

namespace benchmark_mpfr_operation_counter {
namespace raw {

inline void call_mpfr_init(mpfr_ptr value) {
    mpfr_init(value);
}

inline void call_mpfr_init2(mpfr_ptr value, mpfr_prec_t precision) {
    mpfr_init2(value, precision);
}

inline void call_mpfr_clear(mpfr_ptr value) {
    mpfr_clear(value);
}

inline void call_mpfr_add(mpfr_ptr result, mpfr_srcptr lhs, mpfr_srcptr rhs, mpfr_rnd_t rounding) {
    mpfr_add(result, lhs, rhs, rounding);
}

inline void call_mpfr_mul(mpfr_ptr result, mpfr_srcptr lhs, mpfr_srcptr rhs, mpfr_rnd_t rounding) {
    mpfr_mul(result, lhs, rhs, rounding);
}

} // namespace raw
} // namespace benchmark_mpfr_operation_counter

#undef mpfr_init
#undef mpfr_init2
#undef mpfr_clear
#undef mpfr_add
#undef mpfr_mul

namespace benchmark_mpfr_operation_counter {

struct counters {
    std::atomic<std::uint64_t> init{0};
    std::atomic<std::uint64_t> init2{0};
    std::atomic<std::uint64_t> clear{0};
    std::atomic<std::uint64_t> add{0};
    std::atomic<std::uint64_t> mul{0};
};

struct snapshot {
    std::uint64_t init = 0;
    std::uint64_t init2 = 0;
    std::uint64_t clear = 0;
    std::uint64_t add = 0;
    std::uint64_t mul = 0;
};

inline counters &global_counters() {
    static counters value;
    return value;
}

inline snapshot &kernel_baseline() {
    static snapshot value;
    return value;
}

inline snapshot read_counters() {
    const counters &value = global_counters();
    return snapshot{value.init.load(std::memory_order_relaxed),
                    value.init2.load(std::memory_order_relaxed),
                    value.clear.load(std::memory_order_relaxed),
                    value.add.load(std::memory_order_relaxed),
                    value.mul.load(std::memory_order_relaxed)};
}

inline void counted_mpfr_init(mpfr_ptr value) {
    global_counters().init.fetch_add(1, std::memory_order_relaxed);
    raw::call_mpfr_init(value);
}

inline void counted_mpfr_init2(mpfr_ptr value, mpfr_prec_t precision) {
    global_counters().init2.fetch_add(1, std::memory_order_relaxed);
    raw::call_mpfr_init2(value, precision);
}

inline void counted_mpfr_clear(mpfr_ptr value) {
    global_counters().clear.fetch_add(1, std::memory_order_relaxed);
    raw::call_mpfr_clear(value);
}

inline void counted_mpfr_add(mpfr_ptr result, mpfr_srcptr lhs, mpfr_srcptr rhs, mpfr_rnd_t rounding) {
    global_counters().add.fetch_add(1, std::memory_order_relaxed);
    raw::call_mpfr_add(result, lhs, rhs, rounding);
}

inline void counted_mpfr_mul(mpfr_ptr result, mpfr_srcptr lhs, mpfr_srcptr rhs, mpfr_rnd_t rounding) {
    global_counters().mul.fetch_add(1, std::memory_order_relaxed);
    raw::call_mpfr_mul(result, lhs, rhs, rounding);
}

inline void begin_kernel() {
    kernel_baseline() = read_counters();
}

inline void print(const char *label) {
    const snapshot current = read_counters();
    std::cout << "MPFR_OPERATION_COUNTS label=" << label
              << " init=" << current.init
              << " init2=" << current.init2
              << " total_init=" << (current.init + current.init2)
              << " clear=" << current.clear
              << " add=" << current.add
              << " mul=" << current.mul << std::endl;
}

inline void print_kernel(const char *label) {
    const snapshot start = kernel_baseline();
    const snapshot end = read_counters();
    const std::uint64_t init = end.init - start.init;
    const std::uint64_t init2 = end.init2 - start.init2;
    std::cout << "MPFR_KERNEL_COUNTS label=" << label
              << " init=" << init
              << " init2=" << init2
              << " total_init=" << (init + init2)
              << " clear=" << (end.clear - start.clear)
              << " add=" << (end.add - start.add)
              << " mul=" << (end.mul - start.mul) << std::endl;
}

} // namespace benchmark_mpfr_operation_counter

#define mpfr_init(value) ::benchmark_mpfr_operation_counter::counted_mpfr_init((value))
#define mpfr_init2(value, precision) ::benchmark_mpfr_operation_counter::counted_mpfr_init2((value), (precision))
#define mpfr_clear(value) ::benchmark_mpfr_operation_counter::counted_mpfr_clear((value))
#define mpfr_add(result, lhs, rhs, rounding) ::benchmark_mpfr_operation_counter::counted_mpfr_add((result), (lhs), (rhs), (rounding))
#define mpfr_mul(result, lhs, rhs, rounding) ::benchmark_mpfr_operation_counter::counted_mpfr_mul((result), (lhs), (rhs), (rounding))

#else

namespace benchmark_mpfr_operation_counter {

inline void begin_kernel() {}
inline void print(const char *) {}
inline void print_kernel(const char *) {}

} // namespace benchmark_mpfr_operation_counter

#endif

#endif // GMPFRXX_MKII_BENCHMARK_MPFR_OPERATION_COUNTER_HPP
