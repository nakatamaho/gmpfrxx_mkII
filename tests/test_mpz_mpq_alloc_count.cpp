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
    assert(binary_add_allocations == 0);

    mpq_set(qexpected.mpq_data(), qa.mpq_data());
    mpq_add(qexpected.mpq_data(), qexpected.mpq_data(), qb.mpq_data());
    mpq_add(qexpected.mpq_data(), qexpected.mpq_data(), qc.mpq_data());
    reserve_like(qdst, qexpected);

    alloc_count = 0;
    qdst = qa + qb + qc;
    const int add_chain_allocations = alloc_count.load();
    assert(qdst == qexpected);
    assert(add_chain_allocations == 0);

    return 0;
}
