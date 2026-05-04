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
#include <cstdlib>

int main()
{
    auto low = gmpxx::mpfc_class::with_precision(96, 128);
    auto high = gmpxx::mpfc_class::with_precision(192, 224);

    gmpxx::mpfc_class materialized = low + high;
    if (materialized.real_precision() != high.real_precision() ||
        materialized.imag_precision() != high.imag_precision()) {
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

    return 0;
}
