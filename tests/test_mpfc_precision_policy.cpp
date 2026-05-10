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

#include <algorithm>
#include <atomic>
#include <cassert>
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

void assert_close_mpf(const gmpxx::mpf_class& got, const gmpxx::mpf_class& expected)
{
    gmpxx::mpf_class tolerance(1, std::max(got.precision(), expected.precision()));
    mpf_div_2exp(tolerance.mpf_data(), tolerance.mpf_data(), 200);
    assert(gmpxx::abs(got - expected) <= tolerance);
}

void assert_close_mpfc(const gmpxx::mpfc_class& got, const gmpxx::mpfc_class& expected)
{
    assert_close_mpf(got.real(), expected.real());
    assert_close_mpf(got.imag(), expected.imag());
}

} // namespace

int main()
{
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    auto low = gmpxx::mpfc_class::with_precision(96, 128);
    auto high = gmpxx::mpfc_class::with_precision(192, 224);

    gmpxx::mpfc_class materialized = low + high;
    if (materialized.real_precision() != high.real_precision() ||
        materialized.imag_precision() != high.imag_precision()) {
        std::abort();
    }

    gmpxx::mpfc_class scalar_materialized = low + 1;
    if (scalar_materialized.real_precision() != low.real_precision() ||
        scalar_materialized.imag_precision() != low.imag_precision()) {
        std::abort();
    }

    auto dst = gmpxx::mpfc_class::with_precision(80, 88);
    const auto dst_real_precision = dst.real_precision();
    const auto dst_imag_precision = dst.imag_precision();
    dst = low + high;
    if (dst.real_precision() != dst_real_precision ||
        dst.imag_precision() != dst_imag_precision) {
        std::abort();
    }

    auto real = gmpxx::mpf_class::with_precision(160, 2.0);
    gmpxx::mpfc_class mixed = real + low;
    if (mixed.real_precision() != real.precision() ||
        mixed.imag_precision() != std::max(real.precision(), low.imag_precision())) {
        std::abort();
    }

    auto a = gmpxx::mpfc_class::with_precision(256, 1.25, 2.5);
    auto b = gmpxx::mpfc_class::with_precision(256, -0.5, 4.0);
    auto c = gmpxx::mpfc_class::with_precision(256, 3.0, -1.25);
    auto sum_dst = gmpxx::mpfc_class::with_precision(256);

    alloc_count = 0;
    sum_dst = a + b;
    const int binary_add_allocations = alloc_count.load();
    assert(sum_dst == gmpxx::mpfc_class::with_precision(256, 0.75, 6.5));
    assert(binary_add_allocations == 0);

    alloc_count = 0;
    sum_dst = a + b + c;
    const int add_chain_allocations = alloc_count.load();
    assert(sum_dst == gmpxx::mpfc_class::with_precision(256, 3.75, 5.25));
    assert(add_chain_allocations == 0);

    auto mul_dst = gmpxx::mpfc_class::with_precision(256);
    alloc_count = 0;
    mul_dst = a * b;
    const int binary_mul_allocations = alloc_count.load();
    assert(mul_dst == gmpxx::mpfc_class::with_precision(256, -10.625, 3.75));
    assert(binary_mul_allocations == 1);

    auto div_expected_real = gmpxx::mpf_class::with_precision(256, 15.0);
    mpf_div_ui(div_expected_real.mpf_data(), div_expected_real.mpf_data(), 26);
    auto div_expected_imag = gmpxx::mpf_class::with_precision(256, -5.0);
    mpf_div_ui(div_expected_imag.mpf_data(), div_expected_imag.mpf_data(), 13);
    gmpxx::mpfc_class div_expected(div_expected_real, div_expected_imag);

    auto div_dst = gmpxx::mpfc_class::with_precision(256);
    alloc_count = 0;
    div_dst = a / b;
    const int binary_div_allocations = alloc_count.load();
    assert_close_mpfc(div_dst, div_expected);
    assert(binary_div_allocations == 2);

    auto alias_mul = a;
    alias_mul = alias_mul * b;
    assert(alias_mul == mul_dst);

    auto alias_div = a;
    alias_div = alias_div / b;
    assert_close_mpfc(alias_div, div_expected);

    auto add_assign = a;
    alloc_count = 0;
    add_assign += b;
    const int add_assign_allocations = alloc_count.load();
    assert(add_assign == gmpxx::mpfc_class::with_precision(256, 0.75, 6.5));
    assert(add_assign_allocations == 0);

    auto sub_assign = a;
    alloc_count = 0;
    sub_assign -= b;
    const int sub_assign_allocations = alloc_count.load();
    assert(sub_assign == gmpxx::mpfc_class::with_precision(256, 1.75, -1.5));
    assert(sub_assign_allocations == 0);

    auto mul_assign = a;
    alloc_count = 0;
    mul_assign *= b;
    const int mul_assign_allocations = alloc_count.load();
    assert(mul_assign == mul_dst);
    assert(mul_assign_allocations == 3);

    auto div_assign = a;
    alloc_count = 0;
    div_assign /= b;
    const int div_assign_allocations = alloc_count.load();
    assert_close_mpfc(div_assign, div_expected);
    assert(div_assign_allocations == 4);

    return 0;
}
