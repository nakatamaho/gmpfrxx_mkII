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
#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <vector>

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

void require_value(const mpfrxx::mpc_class& value, double real, double imag)
{
    if (mpfr_cmp_d(mpc_realref(value.mpc_data()), real) != 0 ||
        mpfr_cmp_d(mpc_imagref(value.mpc_data()), imag) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    static_assert(gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath);
    static_assert(std::is_nothrow_move_constructible<mpfrxx::mpc_class>::value,
                  "mpc_class move construction must remain noexcept");

    constexpr mpfr_prec_t real_precision = 160;
    constexpr mpfr_prec_t imag_precision = 192;

    auto movable = mpfrxx::mpc_class::with_precision(real_precision, imag_precision, 1.5, -2.5);
    alloc_count = 0;
    mpfrxx::mpc_class moved(std::move(movable));
    require_alloc_count(0);
    if (moved.real_precision() != real_precision || moved.imag_precision() != imag_precision) {
        std::abort();
    }
    require_value(moved, 1.5, -2.5);

    auto move_dst = mpfrxx::mpc_class::with_precision(96, 128, -1.0, -1.0);
    auto move_src = mpfrxx::mpc_class::with_precision(real_precision, imag_precision, 2.5, -3.5);
    alloc_count = 0;
    move_dst = std::move(move_src);
    require_alloc_count(0);
    if (move_dst.real_precision() != real_precision || move_dst.imag_precision() != imag_precision) {
        std::abort();
    }
    require_value(move_dst, 2.5, -3.5);

    std::vector<mpfrxx::mpc_class> values;
    values.reserve(1);
    values.emplace_back(mpfrxx::mpc_class::with_precision(real_precision, imag_precision, 3.5, 4.5));
    alloc_count = 0;
    values.reserve(8);
    require_alloc_count(0);
    if (values.front().real_precision() != real_precision ||
        values.front().imag_precision() != imag_precision) {
        std::abort();
    }
    require_value(values.front(), 3.5, 4.5);

    return 0;
}
