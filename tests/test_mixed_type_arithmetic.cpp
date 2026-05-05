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

#include <algorithm>
#include <cfloat>
#include <cstdlib>
#include <type_traits>
#include <utility>

namespace {

void require_mpf_equal(const gmpxx::mpf_class& got, const mpf_t expected)
{
    if (mpf_cmp(got.mpf_data(), expected) != 0) {
        std::abort();
    }
}

void require_mpfr_equal(const mpfrxx::mpfr_class& got, const mpfr_t expected)
{
    if (mpfr_cmp(got.mpfr_data(), expected) != 0) {
        std::abort();
    }
}

void check_mpf_matrix()
{
    gmpxx::mpf_class f = gmpxx::mpf_class::with_precision(160, 1.25);
    gmpxx::mpz_class z("123456789012345678901234567890");
    gmpxx::mpq_class q("22/7");

    {
        gmpxx::mpf_class got = f + z;
        mpf_t ref;
        mpf_t tmp;
        mpf_init2(ref, got.precision());
        mpf_init2(tmp, got.precision());
        mpf_set_z(tmp, z.mpz_data());
        mpf_add(ref, f.mpf_data(), tmp);
        require_mpf_equal(got, ref);
        if (got.precision() != f.precision()) {
            std::abort();
        }
        mpf_clear(tmp);
        mpf_clear(ref);
    }

    {
        gmpxx::mpf_class got = q * f;
        mpf_t ref;
        mpf_t tmp;
        mpf_init2(ref, got.precision());
        mpf_init2(tmp, got.precision());
        mpf_set_q(tmp, q.mpq_data());
        mpf_mul(ref, tmp, f.mpf_data());
        require_mpf_equal(got, ref);
        mpf_clear(tmp);
        mpf_clear(ref);
    }

    {
        gmpxx::mpf_class lhs = gmpxx::mpf_class::with_precision(128, 1.5);
        gmpxx::mpq_class rhs("9/4");
        if (gmpxx::mpf_class(lhs + rhs) != gmpxx::mpf_class::with_precision(128, 3.75)) {
            std::abort();
        }
        if (gmpxx::mpf_class(lhs - rhs) != gmpxx::mpf_class::with_precision(128, -0.75)) {
            std::abort();
        }
        if (gmpxx::mpf_class(lhs * -rhs) != gmpxx::mpf_class::with_precision(128, -3.375)) {
            std::abort();
        }
        if (gmpxx::mpf_class(lhs / gmpxx::mpq_class(-2)) !=
            gmpxx::mpf_class::with_precision(128, -0.75)) {
            std::abort();
        }
    }

    {
        gmpxx::mpf_class got = (f + z) * 0.5;
        mpf_t ref;
        mpf_t tmp;
        mpf_t half;
        mpf_init2(ref, got.precision());
        mpf_init2(tmp, got.precision());
        mpf_init2(half, std::max(got.precision(), static_cast<mp_bitcnt_t>(DBL_MANT_DIG)));
        mpf_set_z(tmp, z.mpz_data());
        mpf_add(tmp, f.mpf_data(), tmp);
        mpf_set_d(half, 0.5);
        mpf_mul(ref, tmp, half);
        require_mpf_equal(got, ref);
        mpf_clear(half);
        mpf_clear(tmp);
        mpf_clear(ref);
    }

    {
        gmpxx::mpf_class shifted = (-gmpxx::mpf_class(3)) >> 2u;
        mpf_t ref;
        mpf_init2(ref, shifted.precision());
        mpf_set_si(ref, -3);
        mpf_div_2exp(ref, ref, 2);
        require_mpf_equal(shifted, ref);
        mpf_clear(ref);
    }
}

void check_mpfr_matrix()
{
    mpfrxx::mpfr_class r = mpfrxx::mpfr_class::with_precision(160, 1.25);
    mpfrxx::mpz_class z("123456789012345678901234567890");
    mpfrxx::mpq_class q("22/7");

    {
        mpfrxx::mpfr_class got = r + z;
        mpfr_t ref;
        mpfr_t tmp;
        mpfr_init2(ref, got.precision());
        mpfr_init2(tmp, got.precision());
        mpfr_set_z(tmp, z.mpz_data(), MPFR_RNDN);
        mpfr_add(ref, r.mpfr_data(), tmp, MPFR_RNDN);
        require_mpfr_equal(got, ref);
        if (got.precision() != r.precision()) {
            std::abort();
        }
        mpfr_clear(tmp);
        mpfr_clear(ref);
    }

    {
        mpfrxx::mpfr_class got = q * r;
        mpfr_t ref;
        mpfr_t tmp;
        mpfr_init2(ref, got.precision());
        mpfr_init2(tmp, got.precision());
        mpfr_set_q(tmp, q.mpq_data(), MPFR_RNDN);
        mpfr_mul(ref, tmp, r.mpfr_data(), MPFR_RNDN);
        require_mpfr_equal(got, ref);
        mpfr_clear(tmp);
        mpfr_clear(ref);
    }

    {
        mpfrxx::mpfr_class lhs = mpfrxx::mpfr_class::with_precision(128, 1.5);
        mpfrxx::mpq_class rhs("9/4");
        if (mpfrxx::mpfr_class(lhs + rhs) != mpfrxx::mpfr_class::with_precision(128, 3.75)) {
            std::abort();
        }
        if (mpfrxx::mpfr_class(lhs - rhs) != mpfrxx::mpfr_class::with_precision(128, -0.75)) {
            std::abort();
        }
        if (mpfrxx::mpfr_class(lhs * -rhs) != mpfrxx::mpfr_class::with_precision(128, -3.375)) {
            std::abort();
        }
        if (mpfrxx::mpfr_class(lhs / mpfrxx::mpq_class(-2)) !=
            mpfrxx::mpfr_class::with_precision(128, -0.75)) {
            std::abort();
        }
    }

    {
        mpfrxx::mpfr_class got = (r + z) * 0.5;
        mpfr_t ref;
        mpfr_t tmp;
        mpfr_t half;
        mpfr_init2(ref, got.precision());
        mpfr_init2(tmp, got.precision());
        mpfr_init2(half, std::max(got.precision(), static_cast<mpfr_prec_t>(DBL_MANT_DIG)));
        mpfr_set_z(tmp, z.mpz_data(), MPFR_RNDN);
        mpfr_add(tmp, r.mpfr_data(), tmp, MPFR_RNDN);
        mpfr_set_d(half, 0.5, MPFR_RNDN);
        mpfr_mul(ref, tmp, half, MPFR_RNDN);
        require_mpfr_equal(got, ref);
        mpfr_clear(half);
        mpfr_clear(tmp);
        mpfr_clear(ref);
    }

    {
        mpfrxx::mpfr_class materialized = (z + q) + r;
        mpfr_t ref;
        mpfr_t tmp;
        mpfr_init2(ref, materialized.precision());
        mpfr_init2(tmp, materialized.precision());
        mpfr_set_z(ref, z.mpz_data(), MPFR_RNDN);
        mpfr_set_q(tmp, q.mpq_data(), MPFR_RNDN);
        mpfr_add(ref, ref, tmp, MPFR_RNDN);
        mpfr_add(ref, ref, r.mpfr_data(), MPFR_RNDN);
        require_mpfr_equal(materialized, ref);
        mpfr_clear(tmp);
        mpfr_clear(ref);
    }
}

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpf_class&>() +
                           std::declval<const gmpxx::mpz_class&>())>);
    static_assert(std::is_same_v<
                  typename decltype(std::declval<const gmpxx::mpf_class&>() +
                                    std::declval<const gmpxx::mpq_class&>())::result_type,
                  gmpxx::mpf_class>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype((std::declval<const gmpxx::mpz_class&>() +
                            std::declval<const gmpxx::mpq_class&>()) *
                           std::declval<const gmpxx::mpf_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpf_class&>() << 2u)>);
    static_assert(!std::is_same_v<
                  decltype(std::declval<const gmpxx::mpf_class&>() +
                           std::declval<const gmpxx::mpz_class&>()),
                  gmpxx::mpf_class>);

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const mpfrxx::mpfr_class&>() +
                           std::declval<const mpfrxx::mpz_class&>())>);
    static_assert(std::is_same_v<
                  typename decltype(std::declval<const mpfrxx::mpfr_class&>() +
                                    std::declval<const mpfrxx::mpq_class&>())::result_type,
                  mpfrxx::mpfr_class>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype((std::declval<const mpfrxx::mpz_class&>() +
                            std::declval<const mpfrxx::mpq_class&>()) *
                           std::declval<const mpfrxx::mpfr_class&>())>);
    static_assert(!std::is_same_v<
                  decltype(std::declval<const mpfrxx::mpfr_class&>() +
                           std::declval<const mpfrxx::mpz_class&>()),
                  mpfrxx::mpfr_class>);

    check_mpf_matrix();
    check_mpfr_matrix();

    return 0;
}
