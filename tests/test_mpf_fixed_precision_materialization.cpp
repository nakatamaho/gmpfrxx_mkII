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

#include <cassert>

int main()
{
    static_assert(gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath);

    gmpxx::set_default_mpf_precision_bits(160);
    const mp_bitcnt_t default_object_precision = gmpxx::mpf_class().precision();

    const auto low = gmpxx::mpf_class::with_precision(96, 1.25);
    const auto high = gmpxx::mpf_class::with_precision(384, 2.5);
    const gmpxx::mpf_class materialized = low + high;

    assert(materialized.precision() == high.precision());
    assert(materialized.precision() > default_object_precision);

    mpf_t expected;
    mpf_init2(expected, high.precision());
    mpf_add(expected, low.mpf_data(), high.mpf_data());
    assert(mpf_cmp(materialized.mpf_data(), expected) == 0);
    mpf_clear(expected);

    auto destination = gmpxx::mpf_class::with_precision(224, 0.0);
    const mp_bitcnt_t destination_precision = destination.precision();
    destination = low + high;
    assert(destination.precision() == destination_precision);

    return 0;
}
