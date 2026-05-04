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

#include <cstdint>
#include <cstdlib>
#include <limits>

namespace {

void require_equal_to_integer(const mpfrxx::mpfr_class& value, const gmpxx::mpz_class& expected)
{
    if (mpfr_cmp_z(value.mpfr_data(), expected.mpz_data()) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    const auto zero = mpfrxx::mpfr_class::with_precision(128);

    const auto uint64_max = std::numeric_limits<std::uint64_t>::max();
    mpfrxx::mpfr_class unsigned_result = zero + uint64_max;
    require_equal_to_integer(unsigned_result, gmpxx::mpz_class(uint64_max));

    const auto int64_min = std::numeric_limits<std::int64_t>::min();
    mpfrxx::mpfr_class signed_result = zero + int64_min;
    require_equal_to_integer(signed_result, gmpxx::mpz_class(int64_min));

    const auto int64_max = std::numeric_limits<std::int64_t>::max();
    signed_result = int64_max + zero;
    require_equal_to_integer(signed_result, gmpxx::mpz_class(int64_max));

    return 0;
}
