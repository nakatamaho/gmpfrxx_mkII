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
#include <cstdint>
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

    static_assert(std::is_nothrow_move_constructible<gmpxx::mpf_class>::value,
                  "mpf_class move construction must remain noexcept");

    auto movable = gmpxx::mpf_class::with_precision(precision, 5.5);
    alloc_count = 0;
    gmpxx::mpf_class moved(std::move(movable));
    require_alloc_count(1);
    if (moved.precision() != precision || mpf_cmp_d(moved.mpf_data(), 5.5) != 0) {
        std::abort();
    }

    auto copied_from_moved_source = gmpxx::mpf_class::with_precision(precision, 6.5);
    gmpxx::mpf_class copied_from_moved_owner(std::move(copied_from_moved_source));
    gmpxx::mpf_class copied_from_moved(copied_from_moved_source);
    if (copied_from_moved.precision() != precision) {
        std::abort();
    }
    auto assigned_from_moved = gmpxx::mpf_class::with_precision(precision, 3.0);
    assigned_from_moved = copied_from_moved_source;
    if (assigned_from_moved.precision() != precision) {
        std::abort();
    }
    (void)copied_from_moved_owner;

    movable = 7.5;
    if (movable.precision() != precision ||
        mpf_cmp_d(movable.mpf_data(), 7.5) != 0) {
        std::abort();
    }
    auto moved_integral_source = gmpxx::mpf_class::with_precision(precision, 1.0);
    gmpxx::mpf_class moved_integral_sink(std::move(moved_integral_source));
    moved_integral_source = std::uint64_t{9};
    if (moved_integral_source.precision() != precision ||
        mpf_cmp_ui(moved_integral_source.mpf_data(), 9) != 0) {
        std::abort();
    }
    auto moved_string_source = gmpxx::mpf_class::with_precision(precision, 1.0);
    gmpxx::mpf_class moved_string_sink(std::move(moved_string_source));
    moved_string_source = "11.5";
    if (moved_string_source.precision() != precision ||
        mpf_cmp_d(moved_string_source.mpf_data(), 11.5) != 0) {
        std::abort();
    }
    auto moved_expr_source = gmpxx::mpf_class::with_precision(precision, 1.0);
    gmpxx::mpf_class moved_expr_sink(std::move(moved_expr_source));
    moved_expr_source = a + b;
    if (moved_expr_source.precision() != precision ||
        mpf_cmp_d(moved_expr_source.mpf_data(), 4.0) != 0) {
        std::abort();
    }
    (void)moved_integral_sink;
    (void)moved_string_sink;
    (void)moved_expr_sink;

    std::vector<gmpxx::mpf_class> values;
    values.reserve(1);
    values.emplace_back(gmpxx::mpf_class::with_precision(precision, 6.5));
    alloc_count = 0;
    values.reserve(8);
    require_alloc_count(1);
    if (values.front().precision() != precision || mpf_cmp_d(values.front().mpf_data(), 6.5) != 0) {
        std::abort();
    }

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
