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

#include <cmath>
#include <cstdlib>

namespace {

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(160, 3.0, 4.0);
    auto r = mpfrxx::mpfr_class::with_precision(160, 5.0);
    mpfrxx::mpz_class exact_z(7);
    mpfrxx::mpq_class exact_q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    mpfrxx::mpc_class result = z + w;
    require_close(result.real_to_double(), 4.0);
    require_close(result.imag_to_double(), 6.0);
    if (result.precision() != 160) {
        std::abort();
    }

    result = z * w;
    require_close(result.real_to_double(), -5.0);
    require_close(result.imag_to_double(), 10.0);

    result = z + r - exact_z + exact_q + 2;
    require_close(result.real_to_double(), 1.5);
    require_close(result.imag_to_double(), 2.0);

    result = -z;
    require_close(result.real_to_double(), -1.0);
    require_close(result.imag_to_double(), -2.0);

    return 0;
}
