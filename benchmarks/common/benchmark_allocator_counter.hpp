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

#ifndef GMPFRXX_MKII_BENCHMARK_ALLOCATOR_COUNTER_HPP
#define GMPFRXX_MKII_BENCHMARK_ALLOCATOR_COUNTER_HPP

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <gmp.h>

namespace benchmark_allocator_counter {

struct counters {
    std::atomic<std::uint64_t> alloc_calls{0};
    std::atomic<std::uint64_t> realloc_calls{0};
    std::atomic<std::uint64_t> free_calls{0};
    std::atomic<std::uint64_t> alloc_bytes{0};
    std::atomic<std::uint64_t> realloc_old_bytes{0};
    std::atomic<std::uint64_t> realloc_new_bytes{0};
    std::atomic<std::uint64_t> free_bytes{0};
};

struct snapshot {
    std::uint64_t alloc_calls = 0;
    std::uint64_t realloc_calls = 0;
    std::uint64_t free_calls = 0;
    std::uint64_t alloc_bytes = 0;
    std::uint64_t realloc_old_bytes = 0;
    std::uint64_t realloc_new_bytes = 0;
    std::uint64_t free_bytes = 0;
};

inline counters& global_counters()
{
    static counters value;
    return value;
}

inline snapshot& kernel_baseline()
{
    static snapshot value;
    return value;
}

inline bool& installed_flag()
{
    static bool value = false;
    return value;
}

inline snapshot read_counters()
{
    const counters& value = global_counters();
    return snapshot{value.alloc_calls.load(std::memory_order_relaxed),
                    value.realloc_calls.load(std::memory_order_relaxed),
                    value.free_calls.load(std::memory_order_relaxed),
                    value.alloc_bytes.load(std::memory_order_relaxed),
                    value.realloc_old_bytes.load(std::memory_order_relaxed),
                    value.realloc_new_bytes.load(std::memory_order_relaxed),
                    value.free_bytes.load(std::memory_order_relaxed)};
}

inline void* counted_alloc(std::size_t size)
{
    global_counters().alloc_calls.fetch_add(1, std::memory_order_relaxed);
    global_counters().alloc_bytes.fetch_add(static_cast<std::uint64_t>(size), std::memory_order_relaxed);
    return std::malloc(size);
}

inline void* counted_realloc(void* pointer, std::size_t old_size, std::size_t new_size)
{
    global_counters().realloc_calls.fetch_add(1, std::memory_order_relaxed);
    global_counters().realloc_old_bytes.fetch_add(static_cast<std::uint64_t>(old_size),
                                                  std::memory_order_relaxed);
    global_counters().realloc_new_bytes.fetch_add(static_cast<std::uint64_t>(new_size),
                                                  std::memory_order_relaxed);
    return std::realloc(pointer, new_size);
}

inline void counted_free(void* pointer, std::size_t size)
{
    global_counters().free_calls.fetch_add(1, std::memory_order_relaxed);
    global_counters().free_bytes.fetch_add(static_cast<std::uint64_t>(size), std::memory_order_relaxed);
    std::free(pointer);
}

inline void install()
{
#ifdef GMPFRXX_MKII_BENCHMARK_DISABLE_ALLOCATOR_COUNTER
    return;
#else
    bool& installed = installed_flag();
    if (!installed) {
        mp_set_memory_functions(counted_alloc, counted_realloc, counted_free);
        installed = true;
    }
#endif
}

inline void begin_kernel()
{
    kernel_baseline() = read_counters();
}

inline void print_kernel(const char* label)
{
    if (!installed_flag()) {
        return;
    }
    const snapshot start = kernel_baseline();
    const snapshot end = read_counters();
    std::cout << "BENCH_ALLOC_COUNTS label=" << label
              << " alloc=" << (end.alloc_calls - start.alloc_calls)
              << " realloc=" << (end.realloc_calls - start.realloc_calls)
              << " free=" << (end.free_calls - start.free_calls)
              << " alloc_bytes=" << (end.alloc_bytes - start.alloc_bytes)
              << " realloc_old_bytes=" << (end.realloc_old_bytes - start.realloc_old_bytes)
              << " realloc_new_bytes=" << (end.realloc_new_bytes - start.realloc_new_bytes)
              << " free_bytes=" << (end.free_bytes - start.free_bytes)
              << std::endl;
}

} // namespace benchmark_allocator_counter

#endif // GMPFRXX_MKII_BENCHMARK_ALLOCATOR_COUNTER_HPP
