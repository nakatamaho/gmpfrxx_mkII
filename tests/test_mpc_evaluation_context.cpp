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

#include <mpcxx_mkII.h>

#include <cassert>
#include <stdexcept>
#include <string>

namespace {

void require_mpc_equal(const mpfrxx::mpc_class& got, mpc_srcptr expected)
{
    assert(mpfr_cmp(mpc_realref(got.mpc_data()), mpc_realref(expected)) == 0);
    assert(mpfr_cmp(mpc_imagref(got.mpc_data()), mpc_imagref(expected)) == 0);
}

void check_context_compound_rounding_overrides_default_rounding()
{
    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDD, MPFR_RNDD);

    auto acc = mpfrxx::mpc_class::with_precision(8, 8, 1.0, 0.0);
    auto x = mpfrxx::mpc_class::with_precision(64, 64, 0.00390625, 0.0);
    auto y = mpfrxx::mpc_class::with_precision(64, 64, 1.0, 0.0);

    const mpc_rnd_t context_rnd = MPC_RND(MPFR_RNDU, MPFR_RNDU);
    const mpc_rnd_t default_rnd = MPC_RND(MPFR_RNDD, MPFR_RNDD);

    mpc_t expected_context;
    mpc_t expected_default;
    mpc_t product;
    mpc_init3(expected_context, acc.real_precision(), acc.imag_precision());
    mpc_init3(expected_default, acc.real_precision(), acc.imag_precision());
    mpc_init3(product, acc.real_precision(), acc.imag_precision());

    mpc_set(expected_context, acc.mpc_data(), context_rnd);
    mpc_mul(product, x.mpc_data(), y.mpc_data(), context_rnd);
    mpc_add(expected_context, expected_context, product, context_rnd);

    mpc_set(expected_default, acc.mpc_data(), default_rnd);
    mpc_mul(product, x.mpc_data(), y.mpc_data(), default_rnd);
    mpc_add(expected_default, expected_default, product, default_rnd);

    assert(mpfr_cmp(mpc_realref(expected_context), mpc_realref(expected_default)) != 0);

    const mpfrxx::mpc_evaluation_context context{
        acc.real_precision(),
        acc.imag_precision(),
        context_rnd,
    };
    auto acc_context = mpfrxx::with_context(acc, context);
    acc_context += x * y;

    assert(&acc_context.value() == &acc);
    assert(acc_context.context().real_precision == acc.real_precision());
    assert(acc_context.context().imag_precision == acc.imag_precision());
    assert(MPC_RND_RE(acc_context.context().rounding_mode) == MPFR_RNDU);
    assert(MPC_RND_IM(acc_context.context().rounding_mode) == MPFR_RNDU);
    require_mpc_equal(acc, expected_context);
    assert(mpfr_cmp(mpc_realref(acc.mpc_data()), mpc_realref(expected_default)) != 0);

    mpc_clear(product);
    mpc_clear(expected_default);
    mpc_clear(expected_context);

    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDN, MPFR_RNDN);
}

void check_context_assignment_uses_context_rounding()
{
    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDD, MPFR_RNDD);

    auto product_value = mpfrxx::mpc_class::with_precision(8, 8);
    auto x = mpfrxx::mpc_class::with_precision(64, 64, 1.00390625, 0.0);
    auto y = mpfrxx::mpc_class::with_precision(64, 64, 1.00390625, 0.0);

    const mpc_rnd_t context_rnd = MPC_RND(MPFR_RNDU, MPFR_RNDU);
    const mpc_rnd_t default_rnd = MPC_RND(MPFR_RNDD, MPFR_RNDD);

    mpc_t expected_context;
    mpc_t expected_default;
    mpc_init3(expected_context, product_value.real_precision(), product_value.imag_precision());
    mpc_init3(expected_default, product_value.real_precision(), product_value.imag_precision());
    mpc_mul(expected_context, x.mpc_data(), y.mpc_data(), context_rnd);
    mpc_mul(expected_default, x.mpc_data(), y.mpc_data(), default_rnd);

    assert(mpfr_cmp(mpc_realref(expected_context), mpc_realref(expected_default)) != 0);

    auto product_context = mpfrxx::with_context(
        product_value,
        product_value.real_precision(),
        product_value.imag_precision(),
        context_rnd);
    product_context = x * y;

    require_mpc_equal(product_value, expected_context);
    assert(mpfr_cmp(mpc_realref(product_value.mpc_data()), mpc_realref(expected_default)) != 0);

    mpc_clear(expected_default);
    mpc_clear(expected_context);

    mpfrxx::set_default_mpc_rounding_mode(MPFR_RNDN, MPFR_RNDN);
}

void check_context_precision_must_match_target()
{
#if !defined(GMPFRXX_MKII_FAST_FIXED_PREC)
    auto z = mpfrxx::mpc_class::with_precision(8, 16);
    bool threw = false;
    try {
        (void)mpfrxx::with_context(z, mpfrxx::mpc_evaluation_context{16, 16, MPC_RNDNN});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
#endif
}

void check_context_precision_must_be_valid()
{
    auto z = mpfrxx::mpc_class::with_precision(8, 16);
    bool threw_invalid_precision = false;
    try {
        (void)mpfrxx::with_context(z, mpfrxx::mpc_evaluation_context{0, 16, MPC_RNDNN});
    } catch (const std::invalid_argument& exception) {
        threw_invalid_precision = std::string(exception.what()) == "invalid MPFR precision";
    }
    assert(threw_invalid_precision);
}

void check_asymmetric_rounding_overload()
{
    auto z = mpfrxx::mpc_class::with_precision(8, 8, 1.0, 0.0);
    auto x = mpfrxx::mpc_class::with_precision(64, 64, 0.00390625, 0.00390625);
    auto context = mpfrxx::with_context(
        z,
        z.real_precision(),
        z.imag_precision(),
        MPFR_RNDU,
        MPFR_RNDD);
    context += x;

    mpc_t expected;
    mpc_init3(expected, z.real_precision(), z.imag_precision());
    mpc_set_d_d(expected, 1.0, 0.0, MPC_RNDNN);
    mpc_add(expected, expected, x.mpc_data(), MPC_RND(MPFR_RNDU, MPFR_RNDD));
    require_mpc_equal(z, expected);
    mpc_clear(expected);
}

} // namespace

int main()
{
    check_context_compound_rounding_overrides_default_rounding();
    check_context_assignment_uses_context_rounding();
    check_context_precision_must_match_target();
    check_context_precision_must_be_valid();
    check_asymmetric_rounding_overload();
    return 0;
}
