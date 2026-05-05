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

void require_mpz_si(const gmpxx::mpz_class& value, long expected)
{
    if (mpz_cmp_si(value.mpz_data(), expected) != 0) {
        std::abort();
    }
}

void require_mpz_equal(const gmpxx::mpz_class& value, const gmpxx::mpz_class& expected)
{
    if (mpz_cmp(value.mpz_data(), expected.mpz_data()) != 0) {
        std::abort();
    }
}

void require_mpz_raw(const gmpxx::mpz_class& value, const mpz_t expected)
{
    if (mpz_cmp(value.mpz_data(), expected) != 0) {
        std::abort();
    }
}

void check_upstream_binary_matrix(const gmpxx::mpz_class& a, const gmpxx::mpz_class& b)
{
    mpz_t ref;
    mpz_init(ref);

    mpz_add(ref, a.mpz_data(), b.mpz_data());
    require_mpz_raw(gmpxx::mpz_class(a + b), ref);

    mpz_sub(ref, a.mpz_data(), b.mpz_data());
    require_mpz_raw(gmpxx::mpz_class(a - b), ref);

    mpz_mul(ref, a.mpz_data(), b.mpz_data());
    require_mpz_raw(gmpxx::mpz_class(a * b), ref);

    if (mpz_sgn(b.mpz_data()) != 0) {
        mpz_tdiv_q(ref, a.mpz_data(), b.mpz_data());
        require_mpz_raw(gmpxx::mpz_class(a / b), ref);

        gmpxx::mpz_class mod = a;
        mod %= b;
        mpz_tdiv_r(ref, a.mpz_data(), b.mpz_data());
        require_mpz_raw(mod, ref);
        require_mpz_raw(a % b, ref);
    }

    mpz_clear(ref);
}

void check_upstream_scalar_matrix(const gmpxx::mpz_class& a)
{
    mpz_t ref;
    mpz_init(ref);

    mpz_add_ui(ref, a.mpz_data(), 5);
    require_mpz_raw(gmpxx::mpz_class(a + 5LL), ref);
    require_mpz_raw(gmpxx::mpz_class(5LL + a), ref);

    mpz_sub_ui(ref, a.mpz_data(), 5);
    require_mpz_raw(gmpxx::mpz_class(a - 5LL), ref);

    mpz_ui_sub(ref, 5, a.mpz_data());
    require_mpz_raw(gmpxx::mpz_class(5LL - a), ref);

    mpz_mul_ui(ref, a.mpz_data(), 7);
    require_mpz_raw(gmpxx::mpz_class(a * 7u), ref);
    require_mpz_raw(gmpxx::mpz_class(7u * a), ref);

    mpz_tdiv_q_ui(ref, a.mpz_data(), 3);
    require_mpz_raw(gmpxx::mpz_class(a / 3u), ref);

    mpz_tdiv_r_ui(ref, a.mpz_data(), 3);
    require_mpz_raw(a % 3u, ref);

    mpz_clear(ref);
}

void check_upstream_bitwise_shift_matrix()
{
    gmpxx::mpz_class a(0xcafe);
    gmpxx::mpz_class b(0xbeef);

    require_mpz_equal(a & b, gmpxx::mpz_class(0x8aee));
    require_mpz_equal(a | b, gmpxx::mpz_class(0xfeff));
    require_mpz_equal(a ^ b, gmpxx::mpz_class(0x7411));
    require_mpz_equal(a & -0xbeef, gmpxx::mpz_class(0x4010));
    require_mpz_equal(a | -0xbeef, gmpxx::mpz_class(-0x3401));
    require_mpz_equal(0xcafeL & b, gmpxx::mpz_class(0x8aee));
    require_mpz_equal(0xcafeL | b, gmpxx::mpz_class(0xfeff));
    require_mpz_equal(0xcafeL ^ b, gmpxx::mpz_class(0x7411));
    require_mpz_equal(~gmpxx::mpz_class(3), gmpxx::mpz_class(-4));
    require_mpz_equal(~(a & b), gmpxx::mpz_class(-0x8aef));

    gmpxx::mpz_class shifted = (-gmpxx::mpz_class(5)) << 2u;
    require_mpz_equal(shifted, gmpxx::mpz_class(-20));
    shifted = (gmpxx::mpz_class(5) * gmpxx::mpz_class(-4)) >> 3u;
    require_mpz_equal(shifted, gmpxx::mpz_class(-3));
    shifted = gmpxx::mpz_class(3);
    shifted <<= 4u;
    require_mpz_equal(shifted, gmpxx::mpz_class(48));
    shifted = gmpxx::mpz_class(-20);
    shifted >>= 3u;
    require_mpz_equal(shifted, gmpxx::mpz_class(-3));
}

void check_upstream_nested_product_shapes()
{
    gmpxx::mpz_class a(17), b(-5), c(9), d;
    d = a + b * c;
    require_mpz_equal(d, gmpxx::mpz_class(-28));
    d = a - b * c;
    require_mpz_equal(d, gmpxx::mpz_class(62));

    gmpxx::mpz_class e;
    e = gmpxx::mpz_class(-14) + gmpxx::mpz_class(6) * (gmpxx::mpz_class(-8) + 2);
    require_mpz_equal(e, gmpxx::mpz_class(-50));
    e = gmpxx::mpz_class(23) + (gmpxx::mpz_class(-11) + 4u) * 7u;
    require_mpz_equal(e, gmpxx::mpz_class(-26));
    e = (gmpxx::mpz_class(8) + -15) * (gmpxx::mpz_class(-12) - 3) +
        (gmpxx::mpz_class(5) + -21);
    require_mpz_equal(e, gmpxx::mpz_class(89));
}

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() &
                           std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() |
                           std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() ^
                           std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(~std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() << 2u)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() >> 2u)>);
    static_assert(!std::is_same_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() &
                           std::declval<const gmpxx::mpz_class&>()),
                  gmpxx::mpz_class>);

    gmpxx::mpz_class a(7);
    gmpxx::mpz_class b(-3);

    gmpxx::mpz_class sum = a + b;
    require_mpz_si(sum, 4);

    sum = a - b;
    require_mpz_si(sum, 10);

    sum = a * b;
    require_mpz_si(sum, -21);

    gmpxx::mpz_class nested = (a + b) * (a - b);
    require_mpz_si(nested, 40);

    gmpxx::mpz_class zero(std::int64_t{0});
    gmpxx::mpz_class one(std::int64_t{1});
    gmpxx::mpz_class neg_one(std::int64_t{-1});
    gmpxx::mpz_class max_i(std::numeric_limits<std::int64_t>::max());
    gmpxx::mpz_class min_i(std::numeric_limits<std::int64_t>::min());
    gmpxx::mpz_class max_u(std::numeric_limits<std::uint64_t>::max());
    gmpxx::mpz_class huge("1267650600228229401496703205376");
    gmpxx::mpz_class neg_huge("-100000000000000000000000000000000000001");

    check_upstream_binary_matrix(zero, one);
    check_upstream_binary_matrix(one, neg_one);
    check_upstream_binary_matrix(max_i, neg_one);
    check_upstream_binary_matrix(min_i, one);
    check_upstream_binary_matrix(max_u, huge);
    check_upstream_binary_matrix(neg_huge, max_i);
    check_upstream_scalar_matrix(huge);
    check_upstream_scalar_matrix(neg_huge);
    check_upstream_bitwise_shift_matrix();
    check_upstream_nested_product_shapes();

    require_mpz_equal(gmpxx::gcd(gmpxx::mpz_class(6), gmpxx::mpz_class(8)), gmpxx::mpz_class(2));
    require_mpz_equal(gmpxx::lcm(gmpxx::mpz_class(-6), gmpxx::mpz_class(-8)), gmpxx::mpz_class(24));
    require_mpz_equal(gmpxx::factorial(gmpxx::mpz_class(5) - 1), gmpxx::mpz_class(24));
    require_mpz_equal(gmpxx::primorial(gmpxx::mpz_class(5)), gmpxx::mpz_class(30));
    require_mpz_equal(gmpxx::fibonacci(gmpxx::mpz_class(6)), gmpxx::mpz_class(8));
    require_mpz_equal(gmpxx::abs(-huge), huge);
    if (gmpxx::sgn(-huge) != -1) {
        std::abort();
    }

    return 0;
}
