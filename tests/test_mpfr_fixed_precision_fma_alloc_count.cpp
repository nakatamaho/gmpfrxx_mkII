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
    static_assert(gmpfrxx_mkII::detail::build_options::enable_mpfr_fma);
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    constexpr mpfr_prec_t precision = 256;
    auto move_dst = mpfrxx::mpfr_class::with_precision(precision, -1.0);
    auto move_src = mpfrxx::mpfr_class::with_precision(precision, 3.5);
    alloc_count = 0;
    move_dst = std::move(move_src);
    assert(alloc_count.load() == 0);
    assert(move_dst.precision() == precision);
    assert(mpfr_cmp_d(move_dst.mpfr_data(), 3.5) == 0);

    const mpfr_rnd_t rnd = mpfrxx::mpfr_class::default_rounding();
    const auto x = mpfrxx::mpfr_class::with_precision(precision, 1.25);
    const auto y = mpfrxx::mpfr_class::with_precision(precision, 2.5);
    auto add_acc = mpfrxx::mpfr_class::with_precision(precision, 7.5);
    auto sub_acc = mpfrxx::mpfr_class::with_precision(precision, 7.5);

    mpfr_t expected_add;
    mpfr_t expected_sub;
    mpfr_t negated_x;
    mpfr_init2(expected_add, add_acc.precision());
    mpfr_init2(expected_sub, sub_acc.precision());
    mpfr_init2(negated_x, x.precision());
    mpfr_fma(expected_add, x.mpfr_data(), y.mpfr_data(), add_acc.mpfr_data(), rnd);
    mpfr_neg(negated_x, x.mpfr_data(), MPFR_RNDN);
    mpfr_fma(expected_sub, negated_x, y.mpfr_data(), sub_acc.mpfr_data(), rnd);

    add_acc += x * y;
    sub_acc -= x * y;
    assert(mpfr_cmp(add_acc.mpfr_data(), expected_add) == 0);
    assert(mpfr_cmp(sub_acc.mpfr_data(), expected_sub) == 0);

    alloc_count = 0;
    add_acc += x * y;
    sub_acc -= x * y;
    const int steady_state_allocations = alloc_count.load();
    assert(steady_state_allocations == 0);

    mpfr_clear(negated_x);
    mpfr_clear(expected_sub);
    mpfr_clear(expected_add);

    return 0;
}
