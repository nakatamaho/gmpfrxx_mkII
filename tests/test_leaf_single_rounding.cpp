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

#include <gmpfrxx_mkII.h>

#include <cstdlib>

namespace {

void require_mpfr_leaf_lhs_single_rounding()
{
    using mpfrxx::mpfr_class;

    auto a = mpfr_class::with_precision(200);
    {
        auto one = mpfr_class::with_precision(200, 1.0);
        auto tiny = mpfr_class::with_precision(200, 1.0);
        mpfr_div_2ui(tiny.get_mpfr_t(), tiny.get_mpfr_t(), 80, MPFR_RNDN);
        mpfr_add(a.get_mpfr_t(), one.get_mpfr_t(), tiny.get_mpfr_t(), MPFR_RNDN);
    }

    const auto b = mpfr_class::with_precision(53, 1.0);
    const auto c = mpfr_class::with_precision(53, 1.0);

    auto ref = mpfr_class::with_precision(53);
    {
        auto product = mpfr_class::with_precision(53);
        mpfr_mul(product.get_mpfr_t(), b.get_mpfr_t(), c.get_mpfr_t(), MPFR_RNDN);
        mpfr_sub(ref.get_mpfr_t(), a.get_mpfr_t(), product.get_mpfr_t(), MPFR_RNDN);
    }

    auto got = mpfr_class::with_precision(53);
    got = a - (b * c);
    if (mpfr_cmp(got.get_mpfr_t(), ref.get_mpfr_t()) != 0) {
        std::abort();
    }

    auto mirror = mpfr_class::with_precision(53);
    mirror = (b * c) - a;
    mpfr_neg(mirror.get_mpfr_t(), mirror.get_mpfr_t(), MPFR_RNDN);
    if (mpfr_cmp(got.get_mpfr_t(), mirror.get_mpfr_t()) != 0) {
        std::abort();
    }
}

void require_mpf_leaf_lhs_single_rounding()
{
    using gmpxx::mpf_class;

    auto a = mpf_class::with_precision(200);
    {
        auto one = mpf_class::with_precision(200, 1.0);
        auto tiny = mpf_class::with_precision(200, 1.0);
        mpf_div_2exp(tiny.get_mpf_t(), tiny.get_mpf_t(), 80);
        mpf_add(a.get_mpf_t(), one.get_mpf_t(), tiny.get_mpf_t());
    }

    const auto b = mpf_class::with_precision(53, 1.0);
    const auto c = mpf_class::with_precision(53, 1.0);

    auto ref = mpf_class::with_precision(53);
    {
        auto product = mpf_class::with_precision(53);
        mpf_mul(product.get_mpf_t(), b.get_mpf_t(), c.get_mpf_t());
        mpf_sub(ref.get_mpf_t(), a.get_mpf_t(), product.get_mpf_t());
    }

    auto got = mpf_class::with_precision(53);
    got = a - (b * c);
    if (mpf_cmp(got.get_mpf_t(), ref.get_mpf_t()) != 0) {
        std::abort();
    }

    auto mirror = mpf_class::with_precision(53);
    mirror = (b * c) - a;
    mpf_neg(mirror.get_mpf_t(), mirror.get_mpf_t());
    if (mpf_cmp(got.get_mpf_t(), mirror.get_mpf_t()) != 0) {
        std::abort();
    }
}

void require_mpc_leaf_lhs_single_rounding()
{
    using mpfrxx::mpc_class;

    auto a = mpc_class::with_precision(200, 200);
    mpc_set_ui_ui(a.mpc_data(), 1, 0, MPC_RNDNN);
    mpfr_t tiny;
    mpfr_init2(tiny, 200);
    mpfr_set_ui(tiny, 1, MPFR_RNDN);
    mpfr_div_2ui(tiny, tiny, 80, MPFR_RNDN);
    mpfr_add(mpc_realref(a.mpc_data()), mpc_realref(a.mpc_data()), tiny, MPFR_RNDN);
    mpfr_clear(tiny);

    const auto b = mpc_class::with_precision(53, 53, 1.0, 0.0);
    const auto c = mpc_class::with_precision(53, 53, 1.0, 0.0);

    auto ref = mpc_class::with_precision(53, 53);
    {
        auto product = mpc_class::with_precision(53, 53);
        mpc_mul(product.mpc_data(), b.mpc_data(), c.mpc_data(), MPC_RNDNN);
        mpc_sub(ref.mpc_data(), a.mpc_data(), product.mpc_data(), MPC_RNDNN);
    }

    auto got = mpc_class::with_precision(53, 53);
    got = a - (b * c);
    if (mpc_cmp(got.mpc_data(), ref.mpc_data()) != 0) {
        std::abort();
    }

    auto mirror = mpc_class::with_precision(53, 53);
    mirror = (b * c) - a;
    mpc_neg(mirror.mpc_data(), mirror.mpc_data(), MPC_RNDNN);
    if (mpc_cmp(got.mpc_data(), mirror.mpc_data()) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    require_mpfr_leaf_lhs_single_rounding();
    require_mpf_leaf_lhs_single_rounding();
    require_mpc_leaf_lhs_single_rounding();
    return 0;
}
