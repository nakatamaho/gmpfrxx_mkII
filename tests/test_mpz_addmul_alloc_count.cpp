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

} // namespace

int main()
{
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    mpfrxx::mpz_class a("1000");
    mpfrxx::mpz_class b("12345678901234567890");
    mpfrxx::mpz_class c("-98765432109876543210");
    mpfrxx::mpz_class expected;

    mpz_set(expected.mpz_data(), a.mpz_data());
    mpz_addmul(expected.mpz_data(), b.mpz_data(), c.mpz_data());
    mpz_realloc2(a.mpz_data(), static_cast<mp_bitcnt_t>(mpz_sizeinbase(expected.mpz_data(), 2) + 64));

    alloc_count = 0;
    a += b * c;
    const int addmul_allocations = alloc_count.load();
    assert(a == expected);
    assert(addmul_allocations == 0);

    mpz_set(expected.mpz_data(), a.mpz_data());
    mpz_submul_ui(expected.mpz_data(), b.mpz_data(), 5UL);
    mpz_realloc2(a.mpz_data(), static_cast<mp_bitcnt_t>(mpz_sizeinbase(expected.mpz_data(), 2) + 64));

    alloc_count = 0;
    a -= b * 5ULL;
    const int submul_ui_allocations = alloc_count.load();
    assert(a == expected);
    assert(submul_ui_allocations == 0);

    return 0;
}
