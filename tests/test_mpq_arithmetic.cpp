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

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <utility>

namespace {

void require_mpq_raw(const gmpxx::mpq_class& value, const mpq_t expected)
{
    if (mpq_equal(value.mpq_data(), expected) == 0) {
        std::abort();
    }
}

void require_mpq_equal(const gmpxx::mpq_class& value, const gmpxx::mpq_class& expected)
{
    if (mpq_equal(value.mpq_data(), expected.mpq_data()) == 0) {
        std::abort();
    }
}

void require_mpz_equal(const gmpxx::mpz_class& value, const gmpxx::mpz_class& expected)
{
    if (mpz_cmp(value.mpz_data(), expected.mpz_data()) != 0) {
        std::abort();
    }
}

void check_binary(const gmpxx::mpq_class& a, const gmpxx::mpq_class& b)
{
    mpq_t ref;
    mpq_init(ref);

    mpq_add(ref, a.mpq_data(), b.mpq_data());
    require_mpq_raw(gmpxx::mpq_class(a + b), ref);

    mpq_sub(ref, a.mpq_data(), b.mpq_data());
    require_mpq_raw(gmpxx::mpq_class(a - b), ref);

    mpq_mul(ref, a.mpq_data(), b.mpq_data());
    require_mpq_raw(gmpxx::mpq_class(a * b), ref);

    if (mpq_sgn(b.mpq_data()) != 0) {
        mpq_div(ref, a.mpq_data(), b.mpq_data());
        require_mpq_raw(gmpxx::mpq_class(a / b), ref);
    }

    mpq_clear(ref);
}

void check_scalar(const gmpxx::mpq_class& a)
{
    mpq_t ref;
    mpq_t scalar;
    mpq_init(ref);
    mpq_init(scalar);
    mpq_set_si(scalar, 5, 1);

    mpq_add(ref, a.mpq_data(), scalar);
    require_mpq_raw(gmpxx::mpq_class(a + 5LL), ref);
    require_mpq_raw(gmpxx::mpq_class(5LL + a), ref);

    mpq_sub(ref, a.mpq_data(), scalar);
    require_mpq_raw(gmpxx::mpq_class(a - 5LL), ref);

    mpq_sub(ref, scalar, a.mpq_data());
    require_mpq_raw(gmpxx::mpq_class(5LL - a), ref);

    mpq_mul(ref, a.mpq_data(), scalar);
    require_mpq_raw(gmpxx::mpq_class(a * 5LL), ref);
    require_mpq_raw(gmpxx::mpq_class(5LL * a), ref);

    mpq_div(ref, a.mpq_data(), scalar);
    require_mpq_raw(gmpxx::mpq_class(a / 5LL), ref);

    mpq_clear(scalar);
    mpq_clear(ref);
}

void check_shift_increment_and_mixed_zq()
{
    gmpxx::mpq_class a("3/8");
    require_mpq_equal((-a) << 4u, gmpxx::mpq_class(-6));
    require_mpq_equal((gmpxx::mpq_class(-6) >> 2u), gmpxx::mpq_class("-3/2"));

    gmpxx::mpq_class shifted("3/8");
    shifted <<= 4u;
    require_mpq_equal(shifted, gmpxx::mpq_class(6));
    shifted >>= 2u;
    require_mpq_equal(shifted, gmpxx::mpq_class("3/2"));

    gmpxx::mpq_class inc("1/2");
    require_mpq_equal(++inc, gmpxx::mpq_class("3/2"));
    require_mpq_equal(inc++, gmpxx::mpq_class("3/2"));
    require_mpq_equal(inc, gmpxx::mpq_class("5/2"));
    require_mpq_equal(--inc, gmpxx::mpq_class("3/2"));
    require_mpq_equal(inc--, gmpxx::mpq_class("3/2"));
    require_mpq_equal(inc, gmpxx::mpq_class("1/2"));

    require_mpq_equal(++gmpxx::mpq_class(7), gmpxx::mpq_class(8));
    require_mpq_equal(++gmpxx::mpq_class(-8), gmpxx::mpq_class(-7));
    require_mpq_equal(--gmpxx::mpq_class(8), gmpxx::mpq_class(7));
    require_mpq_equal(--gmpxx::mpq_class(-7), gmpxx::mpq_class(-8));
    require_mpq_equal(gmpxx::mpq_class(7)++, gmpxx::mpq_class(7));
    require_mpq_equal(gmpxx::mpq_class(-8)--, gmpxx::mpq_class(-8));

    gmpxx::mpz_class b(1);
    gmpxx::mpz_class c(4);
    gmpxx::mpq_class d = (b - c) * gmpxx::mpq_class("2/3");
    require_mpq_equal(d, gmpxx::mpq_class(-2));
    d = (gmpxx::mpq_class("1/2") * d) / -d.get_num();
    require_mpq_equal(d, gmpxx::mpq_class("-1/2"));
}

void check_accessors_nested_compound_and_unary()
{
    {
        gmpxx::mpq_class x("3/6");
        require_mpz_equal(x.get_num(), gmpxx::mpz_class(1));
        require_mpz_equal(x.get_den(), gmpxx::mpz_class(2));
    }

    {
        gmpxx::mpq_class a("1/3");
        gmpxx::mpq_class b("2/5");
        gmpxx::mpq_class c("-7/11");
        gmpxx::mpq_class got = (a + b) * c;
        mpq_t tmp;
        mpq_t ref;
        mpq_init(tmp);
        mpq_init(ref);
        mpq_add(tmp, a.mpq_data(), b.mpq_data());
        mpq_mul(ref, tmp, c.mpq_data());
        require_mpq_raw(got, ref);
        mpq_clear(ref);
        mpq_clear(tmp);
    }

    {
        gmpxx::mpq_class a("1/3");
        gmpxx::mpq_class b("2/5");
        gmpxx::mpq_class c("7/11");
        mpq_t ref;
        mpq_t tmp;
        mpq_init(ref);
        mpq_init(tmp);
        mpq_set(ref, a.mpq_data());
        mpq_add(tmp, b.mpq_data(), c.mpq_data());
        mpq_add(ref, ref, tmp);
        a += b + c;
        require_mpq_raw(a, ref);
        mpq_clear(tmp);
        mpq_clear(ref);
    }

    if (gmpxx::sgn(gmpxx::mpq_class(-gmpxx::mpq_class("1/3"))) >= 0 ||
        gmpxx::sgn(gmpxx::mpq_class(+gmpxx::mpq_class("1/3"))) <= 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpq_class&>() +
                           std::declval<const gmpxx::mpq_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpq_class&>() - 5LL)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpq_class&>() + 0.5)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(5LL * std::declval<const gmpxx::mpq_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(0.5 * std::declval<const gmpxx::mpq_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpq_class&>() << 3u)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() +
                           std::declval<const gmpxx::mpq_class&>())>);
    static_assert(std::is_same_v<
                  typename decltype(std::declval<const gmpxx::mpz_class&>() +
                                    std::declval<const gmpxx::mpq_class&>())::result_type,
                  gmpxx::mpq_class>);
    static_assert(!std::is_same_v<
                  decltype(std::declval<const gmpxx::mpq_class&>() +
                           std::declval<const gmpxx::mpq_class&>()),
                  gmpxx::mpq_class>);

    gmpxx::mpq_class zero(std::int64_t{0});
    gmpxx::mpq_class one(std::int64_t{1});
    gmpxx::mpq_class neg_one(std::int64_t{-1});
    gmpxx::mpq_class third("1/3");
    gmpxx::mpq_class pi_approx("22/7");
    gmpxx::mpq_class large(gmpxx::mpz_class(std::numeric_limits<std::int64_t>::max()),
                           gmpxx::mpz_class(std::int64_t{17}));
    gmpxx::mpq_class neg_large(gmpxx::mpz_class(std::numeric_limits<std::int64_t>::min()),
                               gmpxx::mpz_class(std::int64_t{19}));

    check_binary(zero, one);
    check_binary(one, neg_one);
    check_binary(third, pi_approx);
    check_binary(large, neg_large);
    check_scalar(third);
    check_scalar(neg_large);
    check_shift_increment_and_mixed_zq();
    check_accessors_nested_compound_and_unary();
    require_mpq_equal(third + 0.5, gmpxx::mpq_class("5/6"));

    return 0;
}
