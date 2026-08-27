/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
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
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <cassert>

#include <gmpxx_mkII.h>

int main()
{
    using gmpxx::mpf_class;
    using gmpxx::mpfc_class;

    /*
     * 3,000,000,000 is representable by mp_bitcnt_t on 64-bit MinGW and
     * exercises the LLP64 case where the corresponding binary exponent does
     * not fit in signed long.  The shift is limb-aligned, so GMP changes the
     * MPF exponent metadata without materializing billions of zero bits.
     */
    const mp_bitcnt_t large_shift = static_cast<mp_bitcnt_t>(3000000000ULL);
    mpf_class large = mpf_class::with_precision(256, 1.0);
    mpf_mul_2exp(large.mpf_data(), large.mpf_data(), large_shift);
    const mpf_class one = mpf_class::with_precision(256, 1.0);

    assert(gmpfrxx_mkII::detail::mpfc_compare_abs_for_division(one.mpf_data(), large.mpf_data()) < 0);
    assert(gmpfrxx_mkII::detail::mpfc_compare_abs_for_division(large.mpf_data(), one.mpf_data()) > 0);

    const mpfc_class denominator(large, one);
    const mpfc_class numerator = denominator;
    const mpfc_class quotient = numerator / denominator;
    assert(quotient.real() == 1);
    assert(quotient.imag() == 0);
    return 0;
}
