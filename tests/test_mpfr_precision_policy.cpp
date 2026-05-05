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

#include <algorithm>
#include <cstdlib>
#include <type_traits>
#include <utility>

namespace {

void require_mpfr_equal(const mpfrxx::mpfr_class& got, mpfr_srcptr expected)
{
    if (mpfr_cmp(got.mpfr_data(), expected) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    static_assert(std::is_same<decltype(std::declval<mpfrxx::mpfr_class&>().set_prec(
                                    std::declval<mpfr_prec_t>())),
                               void>::value,
                  "");

    const auto low = mpfrxx::mpfr_class::with_precision(80);
    const auto high = mpfrxx::mpfr_class::with_precision(192);

    mpfrxx::mpfr_class materialized = low + high;
    if (materialized.precision() != 192) {
        std::abort();
    }

    auto destination = mpfrxx::mpfr_class::with_precision(96);
    destination = high + materialized;
    if (destination.precision() != 96) {
        std::abort();
    }

    mpfrxx::mpfr_class unary = -high;
    if (unary.precision() != 192) {
        std::abort();
    }

    {
        mpfrxx::mpfr_class a("1.0", 64);
        mpfrxx::mpfr_class b("2.0", 1024);
        mpfrxx::mpfr_class r = a + b;
        if (r.get_prec() != std::max(a.get_prec(), b.get_prec())) {
            std::abort();
        }

        mpfrxx::mpfr_class assigned("0.0", 128);
        const mpfr_prec_t old_prec = assigned.get_prec();
        assigned = a + b;
        mpfr_t ref;
        mpfr_init2(ref, old_prec);
        mpfr_add(ref, a.mpfr_data(), b.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
        if (assigned.get_prec() != old_prec) {
            std::abort();
        }
        require_mpfr_equal(assigned, ref);
        mpfr_clear(ref);

        auto evaluated = (a + b).eval();
        if (evaluated.get_prec() != std::max(a.get_prec(), b.get_prec())) {
            std::abort();
        }
    }

    {
        mpfrxx::mpfr_class a("1.0", 64);
        mpfrxx::mpfr_class b("2.0", 1024);
        mpfrxx::mpfr_class r("1.0", 128);
        const mpfr_prec_t old_prec = r.get_prec();

        mpfr_t ref;
        mpfr_init2(ref, old_prec);
        mpfr_add(ref, a.mpfr_data(), b.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
        mpfr_add(ref, r.mpfr_data(), ref, mpfrxx::mpfr_class::default_rounding());

        r += a + b;

        if (r.get_prec() != old_prec) {
            std::abort();
        }
        require_mpfr_equal(r, ref);
        mpfr_clear(ref);
    }

    {
        mpfrxx::mpfr_class value("1.234567890123456789", 256);
        mpfr_t ref;
        mpfr_init2(ref, value.get_prec());
        mpfr_set(ref, value.mpfr_data(), mpfrxx::mpfr_class::default_rounding());

        value.set_prec(64);
        mpfr_prec_round(ref, 64, mpfrxx::mpfr_class::default_rounding());

        if (value.get_prec() != mpfr_get_prec(ref)) {
            std::abort();
        }
        require_mpfr_equal(value, ref);
        mpfr_clear(ref);
    }

    return 0;
}
