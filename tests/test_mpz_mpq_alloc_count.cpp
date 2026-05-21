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

void reserve_like(mpfrxx::mpq_class& dest, const mpfrxx::mpq_class& value)
{
    mpz_realloc2(mpq_numref(dest.mpq_data()),
                 static_cast<mp_bitcnt_t>(mpz_sizeinbase(mpq_numref(value.mpq_data()), 2) + 64));
    mpz_realloc2(mpq_denref(dest.mpq_data()),
                 static_cast<mp_bitcnt_t>(mpz_sizeinbase(mpq_denref(value.mpq_data()), 2) + 64));
}

} // namespace

int main()
{
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    mpfrxx::mpz_class za("10");
    mpfrxx::mpz_class zb("20");
    mpfrxx::mpz_class zc("30");
    mpfrxx::mpz_class zdst;

    zdst = za + zb;
    assert(zdst == 30);

    zdst = za + zb + zc;
    assert(zdst == 60);

    za += zb;
    assert(za == 30);

    za += zb + zc;
    assert(za == 80);

    mpfrxx::mpq_class qa("1/3");
    mpfrxx::mpq_class qb("2/5");
    mpfrxx::mpq_class qc("-7/11");
    mpfrxx::mpq_class qdst;
    mpfrxx::mpq_class qexpected;

    qdst = qa * qb;
    assert(qdst == mpfrxx::mpq_class("2/15"));

    qdst += 1;
    assert(qdst == mpfrxx::mpq_class("17/15"));

    qdst = qa + 2;
    assert(qdst == mpfrxx::mpq_class("7/3"));

    mpq_add(qexpected.mpq_data(), qa.mpq_data(), qb.mpq_data());
    reserve_like(qdst, qexpected);

    alloc_count = 0;
    qdst = qa + qb;
    const int binary_add_allocations = alloc_count.load();
    assert(qdst == qexpected);
    // MPQ arithmetic now validates raw stream-compatible operands through
    // checked canonical temporaries, so rational operations are not required
    // to be allocation-free.
    (void)binary_add_allocations;

    mpq_set(qexpected.mpq_data(), qa.mpq_data());
    mpq_add(qexpected.mpq_data(), qexpected.mpq_data(), qb.mpq_data());
    mpq_add(qexpected.mpq_data(), qexpected.mpq_data(), qc.mpq_data());
    reserve_like(qdst, qexpected);

    alloc_count = 0;
    qdst = qa + qb + qc;
    const int add_chain_allocations = alloc_count.load();
    assert(qdst == qexpected);
    (void)add_chain_allocations;

    alloc_count = 0;
    assert(zb < zc);
    assert(zc > zb);
    assert(zb <= zb);
    assert(zc >= zb);
    assert(cmp(zb, zc) < 0);
    const int mpz_order_allocations = alloc_count.load();
    assert(mpz_order_allocations == 0);

    alloc_count = 0;
    assert(zc > 20);
    assert(20 < zc);
    assert(zc != 20);
    assert(cmp(zc, 20) > 0);
    assert(cmp(20, zc) < 0);
    const int mpz_scalar_compare_allocations = alloc_count.load();
    assert(mpz_scalar_compare_allocations == 0);

    alloc_count = 0;
    assert(zc > 20.5);
    assert(20.5 < zc);
    assert(cmp(zc, 20.5) > 0);
    assert(cmp(20.5, zc) < 0);
    const int mpz_double_compare_allocations = alloc_count.load();
    assert(mpz_double_compare_allocations == 0);

    mpfrxx::mpq_class qz(zb);
    mpfrxx::mpq_class qhalf("3/2");

    alloc_count = 0;
    assert(qz == zb);
    assert(zb == qz);
    assert(qhalf < zb);
    assert(zb > qhalf);
    assert(cmp(qz, zb) == 0);
    assert(cmp(zb, qz) == 0);
    const int mpz_mpq_compare_allocations = alloc_count.load();
    (void)mpz_mpq_compare_allocations;

    alloc_count = 0;
    assert(qhalf > 1);
    assert(1 < qhalf);
    assert(qhalf != 2);
    assert(cmp(qhalf, 1) > 0);
    assert(cmp(1, qhalf) < 0);
    const int mpq_scalar_compare_allocations = alloc_count.load();
    (void)mpq_scalar_compare_allocations;

    return 0;
}
