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

#include <cstdlib>

namespace {

void require_mpq_si(const gmpxx::mpq_class& value, long numerator, unsigned long denominator)
{
    if (mpq_cmp_si(value.mpq_data(), numerator, denominator) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    gmpxx::mpz_class one(1);
    gmpxx::mpz_class two(2);
    gmpxx::mpz_class three(3);

    gmpxx::mpq_class half(one, two);
    gmpxx::mpq_class third(one, three);

    gmpxx::mpq_class result = half + third;
    require_mpq_si(result, 5, 6);

    result = half - third;
    require_mpq_si(result, 1, 6);

    result = half * third;
    require_mpq_si(result, 1, 6);

    result = half / third;
    require_mpq_si(result, 3, 2);

    result = gmpxx::mpz_class(2) + half;
    require_mpq_si(result, 5, 2);

    return 0;
}
