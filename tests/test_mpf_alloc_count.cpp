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
#include <cstdio>
#include <cstdlib>

namespace {

std::atomic<int> alloc_count{0};

void* count_alloc(std::size_t n)
{
    ++alloc_count;
    return std::malloc(n);
}

void* count_realloc(void* p, std::size_t, std::size_t n)
{
    return std::realloc(p, n);
}

void count_free(void* p, std::size_t)
{
    std::free(p);
}

void require_alloc_count(int expected)
{
    const int actual = alloc_count.load();
    if (actual != expected) {
        std::fprintf(stderr, "expected %d allocations, got %d\n", expected, actual);
        std::abort();
    }
}

} // namespace

int main()
{
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    constexpr mp_bitcnt_t precision = 256;
    const auto a = gmpxx::mpf_class::with_precision(precision, 1.5);
    const auto b = gmpxx::mpf_class::with_precision(precision, 2.5);
    const auto c = gmpxx::mpf_class::with_precision(precision, 3.5);
    const auto d = gmpxx::mpf_class::with_precision(precision, 4.5);
    auto dst = gmpxx::mpf_class::with_precision(precision);

    alloc_count = 0;
    dst = a + b;
    require_alloc_count(0);

    alloc_count = 0;
    dst = a + b + c;
    require_alloc_count(0);

    alloc_count = 0;
    dst = a + b + c + d;
    require_alloc_count(0);

    alloc_count = 0;
    dst = (a + b) * (c + d);
    require_alloc_count(1);

    alloc_count = 0;
    dst += a * b;
    require_alloc_count(1);

    return 0;
}
