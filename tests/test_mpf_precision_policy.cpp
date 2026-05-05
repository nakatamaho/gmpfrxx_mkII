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
    unsetenv("MPFXX_DEFAULT_PREC_BITS");
    gmpxx::reload_default_mpf_precision_bits_from_environment();

    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    gmpxx::mpf_class builtin_default;
    if (builtin_default.precision() < 512) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "320", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 320) {
        std::abort();
    }
    gmpxx::mpf_class env_default;
    if (env_default.precision() < 320) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "0", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "not-a-number", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    gmpxx::set_default_mpf_precision_bits(160);
    gmpxx::mpf_class default_value;
    if (default_value.precision() < 160) {
        std::abort();
    }

    gmpxx::set_default_mpf_precision_bits(0);
    if (gmpxx::default_mpf_precision_bits() != 160) {
        std::abort();
    }

    const auto low = gmpxx::mpf_class::with_precision(80, 1.0);
    const auto high = gmpxx::mpf_class::with_precision(192, 2.0);

    gmpxx::mpf_class materialized = low + high;
    if (materialized.precision() != std::max(low.precision(), high.precision())) {
        std::abort();
    }

    auto destination = gmpxx::mpf_class::with_precision(96);
    const auto destination_precision = destination.precision();
    destination = high + materialized;
    if (destination.precision() != destination_precision) {
        std::abort();
    }

    return 0;
}
