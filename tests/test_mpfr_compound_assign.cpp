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
#include <cstdint>
#include <limits>

namespace {

void apply_ref(mpfr_t lhs, const mpfr_t rhs, char op, mpfr_rnd_t rnd)
{
    if (op == '+') {
        mpfr_add(lhs, lhs, rhs, rnd);
    } else if (op == '-') {
        mpfr_sub(lhs, lhs, rhs, rnd);
    } else if (op == '*') {
        mpfr_mul(lhs, lhs, rhs, rnd);
    } else {
        mpfr_div(lhs, lhs, rhs, rnd);
    }
}

void assert_equal(const mpfrxx::mpfr_class& got, const mpfr_t ref, mpfr_prec_t precision)
{
    assert(got.get_prec() == precision);
    assert(mpfr_cmp(got.mpfr_data(), ref) == 0);
}

template <typename Rhs>
void check_rhs(char op, const Rhs& rhs)
{
    mpfrxx::mpfr_class a("7.5", 256);
    const mpfr_prec_t precision = a.get_prec();
    const mpfr_rnd_t rnd = mpfrxx::mpfr_class::default_rounding();
    mpfrxx::mpfr_class rhs_value(rhs, precision);

    mpfr_t ref;
    mpfr_init2(ref, precision);
    mpfr_set(ref, a.mpfr_data(), rnd);
    apply_ref(ref, rhs_value.mpfr_data(), op, rnd);

    if (op == '+') {
        a += rhs;
    } else if (op == '-') {
        a -= rhs;
    } else if (op == '*') {
        a *= rhs;
    } else {
        a /= rhs;
    }

    assert_equal(a, ref, precision);
    mpfr_clear(ref);
}

void check_exact_rhs()
{
    mpfrxx::mpfr_class a("7.5", 256);
    const mpfr_prec_t precision = a.get_prec();
    mpfrxx::mpz_class z(std::int64_t{2});
    mpfrxx::mpq_class q("5/2");

    a += z;
    assert(a == mpfrxx::mpfr_class("9.5", precision));
    assert(a.get_prec() == precision);

    a -= q;
    assert(a == mpfrxx::mpfr_class("7.0", precision));
    assert(a.get_prec() == precision);
}

void check_self_alias_and_expression_rhs()
{
    const mpfr_rnd_t rnd = mpfrxx::mpfr_class::default_rounding();
    mpfrxx::mpfr_class a("3.5", 256);
    const mpfr_prec_t precision = a.get_prec();
    mpfr_t ref;
    mpfr_init2(ref, precision);
    mpfr_mul(ref, a.mpfr_data(), a.mpfr_data(), rnd);
    a *= a;
    assert_equal(a, ref, precision);
    mpfr_clear(ref);

    mpfrxx::mpfr_class x("3.5", 256);
    mpfrxx::mpfr_class y("1.25", 1024);
    mpfr_t rhs;
    mpfr_init2(rhs, x.get_prec());
    mpfr_add(rhs, x.mpfr_data(), y.mpfr_data(), rnd);
    mpfr_init2(ref, x.get_prec());
    mpfr_set(ref, x.mpfr_data(), rnd);
    mpfr_add(ref, ref, rhs, rnd);
    x += x + y;
    assert_equal(x, ref, precision);
    mpfr_clear(ref);
    mpfr_clear(rhs);
}

void check_compound_assignment_expression_fast_path()
{
    const mpfr_rnd_t rnd = mpfrxx::mpfr_class::default_rounding();
    const mpfr_prec_t precision = 256;
    const mpfrxx::mpfr_class b("1.25", 384);
    const mpfrxx::mpfr_class c("2.5", 512);

    mpfr_t product;
    mpfr_t ref;
    mpfr_init2(product, precision);
    mpfr_init2(ref, precision);
    mpfr_mul(product, b.mpfr_data(), c.mpfr_data(), rnd);

    {
        mpfrxx::mpfr_class a("7.5", precision);
        mpfr_set(ref, a.mpfr_data(), rnd);
        mpfr_add(ref, ref, product, rnd);
        a += b * c;
        assert_equal(a, ref, precision);
    }

    {
        mpfrxx::mpfr_class a("7.5", precision);
        mpfr_set(ref, a.mpfr_data(), rnd);
        mpfr_sub(ref, ref, product, rnd);
        a -= b * c;
        assert_equal(a, ref, precision);
    }

    {
        mpfrxx::mpfr_class a("7.5", precision);
        mpfr_set(ref, a.mpfr_data(), rnd);
        mpfr_mul(ref, ref, product, rnd);
        a *= b * c;
        assert_equal(a, ref, precision);
    }

    {
        mpfrxx::mpfr_class a("7.5", precision);
        mpfr_set(ref, a.mpfr_data(), rnd);
        mpfr_div(ref, ref, product, rnd);
        a /= b * c;
        assert_equal(a, ref, precision);
    }

    mpfr_clear(ref);
    mpfr_clear(product);
}

void check_exact_lhs_mpfr_rhs()
{
    mpfrxx::mpfr_class implicit = 3.375;
    assert(implicit == mpfrxx::mpfr_class("3.375"));

    mpfrxx::mpz_class z(18);
    mpfrxx::mpfr_class r(13);
    z += r;
    assert(z == mpfrxx::mpfr_class(31));
    z -= r;
    assert(z == mpfrxx::mpfr_class(18));
    z *= r;
    assert(z == mpfrxx::mpfr_class(234));
    z /= r;
    assert(z == mpfrxx::mpfr_class(18));

    mpfrxx::mpq_class q(7, 2);
    q += r;
    assert(q == mpfrxx::mpfr_class("16.5"));
    q -= r;
    assert(q == mpfrxx::mpfr_class("3.5"));
    q *= r;
    assert(q == mpfrxx::mpfr_class("45.5"));
    q /= r;
    assert(q == mpfrxx::mpfr_class("3.5"));
}

} // namespace

int main()
{
    for (char op : {'+', '-', '*', '/'}) {
        check_rhs(op, mpfrxx::mpfr_class("2.5", 1024));
        check_rhs(op, 5LL);
        check_rhs(op, 2.0);
        check_rhs(op, std::numeric_limits<std::int64_t>::min());
        check_rhs(op, std::numeric_limits<std::int64_t>::max());
        check_rhs(op, std::numeric_limits<std::uint64_t>::max());
    }
    check_rhs('+', 0);
    check_rhs('-', 0);
    check_rhs('*', 0);
    check_exact_rhs();
    check_self_alias_and_expression_rhs();
    check_compound_assignment_expression_fast_path();
    check_exact_lhs_mpfr_rhs();
    return 0;
}
