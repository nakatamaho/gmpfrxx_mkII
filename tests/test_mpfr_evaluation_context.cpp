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
#include <stdexcept>

namespace {

void check_context_rounding_overrides_default_rounding()
{
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);

    mpfrxx::mpfr_class acc("1", 8);
    mpfrxx::mpfr_class x("0.00390625", 8);
    mpfrxx::mpfr_class y("1", 8);

    mpfr_t expected_up;
    mpfr_t expected_down;
    mpfr_t product;
    mpfr_init2(expected_up, acc.precision());
    mpfr_init2(expected_down, acc.precision());
    mpfr_init2(product, acc.precision());

    mpfr_mul(product, x.mpfr_data(), y.mpfr_data(), MPFR_RNDU);
    mpfr_set_ui(expected_up, 1, MPFR_RNDN);
    mpfr_add(expected_up, expected_up, product, MPFR_RNDU);

    mpfr_mul(product, x.mpfr_data(), y.mpfr_data(), MPFR_RNDD);
    mpfr_set_ui(expected_down, 1, MPFR_RNDN);
    mpfr_add(expected_down, expected_down, product, MPFR_RNDD);

    assert(mpfr_cmp(expected_up, expected_down) != 0);

    mpfrxx::set_default_rounding_mode(MPFR_RNDD);
    const mpfrxx::evaluation_context context{
        acc.precision(),
        MPFR_RNDU,
    };
    auto acc_context = mpfrxx::with_context(acc, context);
    acc_context += x * y;

    assert(&acc_context.value() == &acc);
    assert(acc_context.context().precision == acc.precision());
    assert(acc_context.context().rounding_mode == MPFR_RNDU);
    assert(mpfr_cmp(acc.mpfr_data(), expected_up) == 0);
    assert(mpfr_cmp(acc.mpfr_data(), expected_down) != 0);

    mpfr_clear(product);
    mpfr_clear(expected_down);
    mpfr_clear(expected_up);

    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
}

void check_context_precision_must_match_target()
{
    mpfrxx::mpfr_class acc("1", 8);
    bool threw = false;
    try {
        (void)mpfrxx::with_context(acc, mpfrxx::evaluation_context{16, MPFR_RNDN});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

void check_context_assignment_uses_context_rounding()
{
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);

    mpfrxx::mpfr_class product("0", 8);
    mpfrxx::mpfr_class x("1.00390625", 64);
    mpfrxx::mpfr_class y("1.00390625", 64);

    mpfr_t expected_up;
    mpfr_t expected_down;
    mpfr_init2(expected_up, product.precision());
    mpfr_init2(expected_down, product.precision());
    mpfr_mul(expected_up, x.mpfr_data(), y.mpfr_data(), MPFR_RNDU);
    mpfr_mul(expected_down, x.mpfr_data(), y.mpfr_data(), MPFR_RNDD);

    assert(mpfr_cmp(expected_up, expected_down) != 0);

    mpfrxx::set_default_rounding_mode(MPFR_RNDD);
    auto product_context = mpfrxx::with_context(
        product,
        mpfrxx::evaluation_context{
            product.precision(),
            MPFR_RNDU,
        });
    product_context = x * y;

    assert(mpfr_cmp(product.mpfr_data(), expected_up) == 0);
    assert(mpfr_cmp(product.mpfr_data(), expected_down) != 0);

    mpfr_clear(expected_down);
    mpfr_clear(expected_up);

    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
}

} // namespace

int main()
{
    check_context_rounding_overrides_default_rounding();
    check_context_precision_must_match_target();
    check_context_assignment_uses_context_rounding();
    return 0;
}
