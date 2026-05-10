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
#include <cassert>
#include <cstdlib>
#include <utility>

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

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath);
    static_assert(gmpfrxx_mkII::detail::build_options::enable_gmp_fma);
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    constexpr mp_bitcnt_t precision = 256;
    auto move_dst = gmpxx::mpf_class::with_precision(128, -1.0);
    auto move_src = gmpxx::mpf_class::with_precision(precision, 3.5);
    alloc_count = 0;
    move_dst = std::move(move_src);
    assert(alloc_count.load() == 0);
    assert(move_dst.precision() == precision);
    assert(mpf_cmp_d(move_dst.mpf_data(), 3.5) == 0);

    const auto x = gmpxx::mpf_class::with_precision(precision, 1.25);
    const auto y = gmpxx::mpf_class::with_precision(precision, 2.5);
    auto add_acc = gmpxx::mpf_class::with_precision(precision, 7.5);
    auto sub_acc = gmpxx::mpf_class::with_precision(precision, 7.5);

    mpf_t expected_add;
    mpf_t expected_sub;
    mpf_t product;
    mpf_init2(expected_add, add_acc.precision());
    mpf_init2(expected_sub, sub_acc.precision());
    mpf_init2(product, add_acc.precision());
    mpf_set(expected_add, add_acc.mpf_data());
    mpf_set(expected_sub, sub_acc.mpf_data());
    mpf_mul(product, x.mpf_data(), y.mpf_data());
    mpf_add(expected_add, expected_add, product);
    mpf_sub(expected_sub, expected_sub, product);

    add_acc += x * y;
    sub_acc -= x * y;
    assert(mpf_cmp(add_acc.mpf_data(), expected_add) == 0);
    assert(mpf_cmp(sub_acc.mpf_data(), expected_sub) == 0);

    alloc_count = 0;
    add_acc += x * y;
    sub_acc -= x * y;
    const int steady_state_allocations = alloc_count.load();
    assert(steady_state_allocations == 0);

    mpf_clear(product);
    mpf_clear(expected_sub);
    mpf_clear(expected_add);

    return 0;
}
