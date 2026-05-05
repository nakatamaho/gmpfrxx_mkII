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

#include <cassert>
#include <type_traits>
#include <utility>

namespace {

template <typename T>
using is_pos_expr = std::is_same<typename T::op_type, gmpfrxx_mkII::detail::pos_op>;

void assert_equal(const gmpxx::mpf_class& lhs, const gmpxx::mpf_class& rhs)
{
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

void assert_equal(const mpfrxx::mpfr_class& lhs, const mpfrxx::mpfr_class& rhs)
{
    assert(mpfr_equal_p(lhs.mpfr_data(), rhs.mpfr_data()) != 0);
}

void check_unary_mpz_forms()
{
    gmpxx::mpz_class a(-17);
    gmpxx::mpz_class b(+a);
    assert(b == gmpxx::mpz_class(-17));

    gmpxx::mpz_class c(42);
    gmpxx::mpz_class d;
    d = -c;
    assert(d == gmpxx::mpz_class(-42));

    gmpxx::mpz_class e(-9);
    gmpxx::mpz_class f;
    f = ~e;
    assert(f == gmpxx::mpz_class(8));

    gmpxx::mpz_class g(-(-a));
    assert(g == gmpxx::mpz_class(-17));

    d = -(-(-c));
    assert(d == gmpxx::mpz_class(-42));
}

void check_unary_mpq_forms()
{
    gmpxx::mpq_class a("-7/11");
    gmpxx::mpq_class b(+a);
    assert(b == gmpxx::mpq_class("-7/11"));

    gmpxx::mpq_class c("13/5");
    gmpxx::mpq_class d;
    d = -c;
    assert(d == gmpxx::mpq_class("-13/5"));

    gmpxx::mpq_class e(-(-a));
    assert(e == gmpxx::mpq_class("-7/11"));

    d = -(-(-c));
    assert(d == gmpxx::mpq_class("-13/5"));
}

void check_unary_mpf_forms()
{
    gmpxx::mpf_class a("-19.25", static_cast<mp_bitcnt_t>(192));
    gmpxx::mpf_class b(+a);
    assert_equal(b, gmpxx::mpf_class("-19.25", b.get_prec()));

    gmpxx::mpf_class c("6.5", static_cast<mp_bitcnt_t>(256));
    gmpxx::mpf_class d;
    d = -c;
    assert_equal(d, gmpxx::mpf_class("-6.5", d.get_prec()));

    gmpxx::mpf_class e(-(-a));
    assert_equal(e, gmpxx::mpf_class("-19.25", e.get_prec()));

    d = -(-(-c));
    assert_equal(d, gmpxx::mpf_class("-6.5", d.get_prec()));
}

void check_unary_mpfr_forms()
{
    mpfrxx::mpfr_class a("-19.25", static_cast<mpfr_prec_t>(192));
    mpfrxx::mpfr_class b(+a);
    assert_equal(b, mpfrxx::mpfr_class("-19.25", b.precision()));

    mpfrxx::mpfr_class c("6.5", static_cast<mpfr_prec_t>(256));
    mpfrxx::mpfr_class d;
    d = -c;
    assert_equal(d, mpfrxx::mpfr_class("-6.5", d.precision()));

    mpfrxx::mpfr_class e(-(-a));
    assert_equal(e, mpfrxx::mpfr_class("-19.25", e.precision()));

    d = -(-(-c));
    assert_equal(d, mpfrxx::mpfr_class("-6.5", d.precision()));
}

void check_mpf_expression_assignment()
{
    gmpxx::mpf_class a("1.25", 256);
    gmpxx::mpf_class b("2.5", 512);

    gmpxx::mpf_class r1 = -(-a);
    gmpxx::mpf_class r2 = a;
    assert_equal(r1, r2);

    gmpxx::mpf_class r3 = -(-(a + b));
    gmpxx::mpf_class r4 = a + b;
    assert_equal(r3, r4);
    assert(r3.get_prec() == r4.get_prec());

    gmpxx::mpf_class r("0", 128);
    const mp_bitcnt_t old_prec = r.get_prec();
    r = -(-(a + b));

    gmpxx::mpf_class ref(0.0, old_prec);
    mpf_add(ref.get_mpf_t(), a.get_mpf_t(), b.get_mpf_t());

    assert(r.get_prec() == old_prec);
    assert_equal(r, ref);

    r = gmpxx::mpf_class("1.0", 128);
    mpf_set(ref.get_mpf_t(), r.get_mpf_t());
    mpf_add(ref.get_mpf_t(), ref.get_mpf_t(), a.get_mpf_t());

    r += -(-a);
    assert(r.get_prec() == old_prec);
    assert_equal(r, ref);
}

void check_mpfr_expression_assignment()
{
    mpfrxx::mpfr_class a("1.25", 256);
    mpfrxx::mpfr_class b("2.5", 512);

    mpfrxx::mpfr_class r1 = -(-a);
    mpfrxx::mpfr_class r2 = a;
    assert_equal(r1, r2);

    mpfrxx::mpfr_class r3 = -(-(a + b));
    mpfrxx::mpfr_class r4 = a + b;
    assert_equal(r3, r4);
    assert(r3.precision() == r4.precision());

    mpfrxx::mpfr_class r("0", 128);
    const mpfr_prec_t old_prec = r.precision();
    r = -(-(a + b));

    mpfrxx::mpfr_class ref("0", old_prec);
    mpfr_add(ref.mpfr_data(), a.mpfr_data(), b.mpfr_data(), MPFR_RNDN);

    assert(r.precision() == old_prec);
    assert_equal(r, ref);

    r = mpfrxx::mpfr_class("1.0", 128);
    mpfr_set(ref.mpfr_data(), r.mpfr_data(), MPFR_RNDN);
    mpfr_add(ref.mpfr_data(), ref.mpfr_data(), a.mpfr_data(), MPFR_RNDN);

    r += -(-a);
    assert(r.precision() == old_prec);
    assert_equal(r, ref);
}

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(-(-std::declval<const gmpxx::mpf_class&>()))>);
    static_assert(is_pos_expr<decltype(-(-std::declval<const gmpxx::mpf_class&>()))>::value);
    static_assert(is_pos_expr<decltype(-(-std::declval<const mpfrxx::mpfr_class&>()))>::value);

    check_unary_mpz_forms();
    check_unary_mpq_forms();
    check_unary_mpf_forms();
    check_unary_mpfr_forms();
    check_mpf_expression_assignment();
    check_mpfr_expression_assignment();

    return 0;
}
