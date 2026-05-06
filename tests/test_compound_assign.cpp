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
#include <cstdint>
#include <limits>

namespace {

void apply_ref(mpf_t lhs, const mpf_t rhs, char op)
{
    if (op == '+') {
        mpf_add(lhs, lhs, rhs);
    } else if (op == '-') {
        mpf_sub(lhs, lhs, rhs);
    } else if (op == '*') {
        mpf_mul(lhs, lhs, rhs);
    } else {
        mpf_div(lhs, lhs, rhs);
    }
}

void assert_equal(const gmpxx::mpf_class& got, const mpf_t ref, mp_bitcnt_t precision)
{
    assert(got.get_prec() == precision);
    assert(mpf_cmp(got.get_mpf_t(), ref) == 0);
}

template <typename Rhs>
void check_rhs(char op, const Rhs& rhs)
{
    gmpxx::mpf_class a("7.5", 256);
    const mp_bitcnt_t precision = a.get_prec();
    gmpxx::mpf_class rhs_value(rhs, precision);

    mpf_t ref;
    mpf_init2(ref, precision);
    mpf_set(ref, a.get_mpf_t());
    apply_ref(ref, rhs_value.get_mpf_t(), op);

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
    mpf_clear(ref);
}

void check_exact_rhs()
{
    gmpxx::mpf_class a("7.5", 256);
    const mp_bitcnt_t precision = a.get_prec();
    gmpxx::mpz_class z(std::int64_t{2});
    gmpxx::mpq_class q("5/2");

    a += z;
    assert(a == gmpxx::mpf_class("9.5", precision));
    assert(a.get_prec() == precision);

    a -= q;
    assert(a == gmpxx::mpf_class("7.0", precision));
    assert(a.get_prec() == precision);
}

void check_self_alias_and_expression_rhs()
{
    gmpxx::mpf_class a("3.5", 256);
    const mp_bitcnt_t precision = a.get_prec();
    mpf_t ref;
    mpf_init2(ref, precision);
    mpf_mul(ref, a.get_mpf_t(), a.get_mpf_t());
    a *= a;
    assert_equal(a, ref, precision);
    mpf_clear(ref);

    gmpxx::mpf_class x("3.5", 256);
    gmpxx::mpf_class y("1.25", 1024);
    mpf_t rhs;
    mpf_init2(rhs, x.get_prec());
    mpf_add(rhs, x.get_mpf_t(), y.get_mpf_t());
    mpf_init2(ref, x.get_prec());
    mpf_set(ref, x.get_mpf_t());
    mpf_add(ref, ref, rhs);
    x += x + y;
    assert_equal(x, ref, precision);
    mpf_clear(ref);
    mpf_clear(rhs);
}

void check_zq_lhs_mpf_rhs()
{
    gmpxx::mpf_class f("13.0", 256);
    gmpxx::mpz_class z = 18;
    gmpxx::mpq_class q(7, 2);

    assert((z += f) == 31);
    assert((z -= f) == 18);
    assert((z *= f) == 234);
    assert((z /= f) == 18);

    assert((q += f) == gmpxx::mpq_class(33, 2));
    assert((q -= f) == gmpxx::mpq_class(7, 2));
    assert((q *= f) == gmpxx::mpq_class(91, 2));
    assert((q /= f) == gmpxx::mpq_class(7, 2));

    assert((z += -f) == 5);
    assert((z -= -f) == 18);
    assert((q += -f) == gmpxx::mpq_class(-19, 2));
    assert((q -= -f) == gmpxx::mpq_class(7, 2));
}

} // namespace

int main()
{
    for (char op : {'+', '-', '*', '/'}) {
        check_rhs(op, gmpxx::mpf_class("2.5", 1024));
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
    check_zq_lhs_mpf_rhs();
    return 0;
}
