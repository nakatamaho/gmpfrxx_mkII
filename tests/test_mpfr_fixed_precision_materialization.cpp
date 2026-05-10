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

#include <cassert>

int main()
{
    static_assert(gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath);
    static_assert(gmpfrxx_mkII::detail::build_options::enable_mpfr_fma);

    mpfrxx::set_default_precision_bits(160);
    const mpfr_prec_t default_object_precision = mpfrxx::mpfr_class().precision();
    const mpfr_rnd_t rnd = mpfrxx::mpfr_class::default_rounding();

    const auto low = mpfrxx::mpfr_class::with_precision(96, 1.25);
    const auto high = mpfrxx::mpfr_class::with_precision(384, 2.5);
    const mpfrxx::mpfr_class materialized = low + high;

    assert(materialized.precision() == high.precision());
    assert(materialized.precision() > default_object_precision);

    const mpfrxx::mpfr_class scalar_materialized = low + 1;
    assert(scalar_materialized.precision() == low.precision());

    mpfr_t expected;
    mpfr_init2(expected, high.precision());
    mpfr_add(expected, low.mpfr_data(), high.mpfr_data(), rnd);
    assert(mpfr_cmp(materialized.mpfr_data(), expected) == 0);
    mpfr_clear(expected);

    const mpfrxx::mpfr_class left_a("1.00000011920928955078125", 96);
    const mpfrxx::mpfr_class left_b("1.00000011920928955078125", 160);
    const mpfrxx::mpfr_class right_a("1.00000011920928955078125", 224);
    const mpfrxx::mpfr_class right_b("1.00000011920928955078125", 384);
    const mpfr_prec_t fused_precision = right_b.precision();

    const mpfrxx::mpfr_class fused_add = left_a * left_b + right_a * right_b;
    const mpfrxx::mpfr_class fused_sub = left_a * left_b - right_a * right_b;
    assert(fused_add.precision() == fused_precision);
    assert(fused_sub.precision() == fused_precision);

    mpfr_t expected_fmma;
    mpfr_t expected_fmms;
    mpfr_init2(expected_fmma, fused_precision);
    mpfr_init2(expected_fmms, fused_precision);
    mpfr_fmma(expected_fmma,
              left_a.mpfr_data(),
              left_b.mpfr_data(),
              right_a.mpfr_data(),
              right_b.mpfr_data(),
              rnd);
    mpfr_fmms(expected_fmms,
              left_a.mpfr_data(),
              left_b.mpfr_data(),
              right_a.mpfr_data(),
              right_b.mpfr_data(),
              rnd);
    assert(mpfr_cmp(fused_add.mpfr_data(), expected_fmma) == 0);
    assert(mpfr_cmp(fused_sub.mpfr_data(), expected_fmms) == 0);
    mpfr_clear(expected_fmms);
    mpfr_clear(expected_fmma);

    auto destination = mpfrxx::mpfr_class::with_precision(224, 0.0);
    const mpfr_prec_t destination_precision = destination.precision();
    destination = low + high;
    assert(destination.precision() == destination_precision);

    mpfrxx::gmp_randclass random_state(gmp_randinit_default);
    const mpfrxx::mpfr_class random_value(random_state.get_fr(288));
    assert(random_value.precision() == 288);

    return 0;
}
