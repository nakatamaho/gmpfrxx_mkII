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
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {

void require_mpz_raw(const gmpxx::mpz_class& value, const mpz_t expected)
{
    if (mpz_cmp(value.mpz_data(), expected) != 0) {
        std::abort();
    }
}

void require_mpz_equal(const gmpxx::mpz_class& value, const gmpxx::mpz_class& expected)
{
    if (mpz_cmp(value.mpz_data(), expected.mpz_data()) != 0) {
        std::abort();
    }
}

template <typename Function>
void require_domain_error(Function&& function)
{
    try {
        function();
    } catch (const std::domain_error&) {
        return;
    }
    std::abort();
}

void check_binary(const gmpxx::mpz_class& a, const gmpxx::mpz_class& b)
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
        require_mpz_equal(gmpxx::mpz_class(5) / gmpxx::mpz_class(2), gmpxx::mpz_class(2));

        mpz_tdiv_r(ref, a.mpz_data(), b.mpz_data());
        require_mpz_raw(a % b, ref);
        gmpxx::mpz_class mod = a;
        mod %= b;
        require_mpz_raw(mod, ref);
    }

    mpz_clear(ref);
}

void check_division_by_zero_throws()
{
    const gmpxx::mpz_class numerator(42);
    const gmpxx::mpz_class zero(0);

    require_domain_error([&] {
        const gmpxx::mpz_class quotient = numerator / zero;
        (void)quotient;
    });

    require_domain_error([&] {
        gmpxx::mpz_class quotient = numerator;
        quotient /= zero;
    });

    require_domain_error([&] {
        gmpxx::mpz_class quotient = numerator;
        quotient /= 0;
    });

    require_domain_error([&] {
        gmpxx::mpz_class quotient = numerator;
        quotient /= 0.0;
    });
}

void test_mpz_modulo_by_zero_throws()
{
    const gmpxx::mpz_class numerator(42);
    const gmpxx::mpz_class zero(0);

    require_domain_error([&] {
        const gmpxx::mpz_class remainder = numerator % zero;
        (void)remainder;
    });

    require_domain_error([&] {
        gmpxx::mpz_class remainder = numerator;
        remainder %= zero;
    });

    require_domain_error([&] {
        const gmpxx::mpz_class remainder = numerator % 0;
        (void)remainder;
    });

    require_domain_error([&] {
        gmpxx::mpz_class remainder = numerator;
        remainder %= 0;
    });

    require_domain_error([&] {
        const gmpxx::mpz_class remainder = numerator % 0.0;
        (void)remainder;
    });

    require_domain_error([&] {
        gmpxx::mpz_class remainder = numerator;
        remainder %= 0.0;
    });
}

void check_scalar(const gmpxx::mpz_class& a)
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

void check_bitwise_and_shifts()
{
    gmpxx::mpz_class a(0xcafe);
    gmpxx::mpz_class b(0xbeef);

    require_mpz_equal(a & b, gmpxx::mpz_class(0x8aee));
    require_mpz_equal(a | b, gmpxx::mpz_class(0xfeff));
    require_mpz_equal(a ^ b, gmpxx::mpz_class(0x7411));
    require_mpz_equal(a & -0xbeef, gmpxx::mpz_class(0x4010));
    require_mpz_equal(a | -0xbeef, gmpxx::mpz_class(-0x3401));
    require_mpz_equal(a ^ 48879.0, gmpxx::mpz_class(0x7411));
    require_mpz_equal(0xcafeL & b, gmpxx::mpz_class(0x8aee));
    require_mpz_equal(0xcafeL | b, gmpxx::mpz_class(0xfeff));
    require_mpz_equal(0xcafeL ^ b, gmpxx::mpz_class(0x7411));
    require_mpz_equal(~gmpxx::mpz_class(3), gmpxx::mpz_class(-4));
    require_mpz_equal(~(a & b), gmpxx::mpz_class(-0x8aef));
    require_mpz_equal(gmpxx::mpz_class(17) % 2.0, gmpxx::mpz_class(1));
    require_mpz_equal(17.0 % gmpxx::mpz_class(2), gmpxx::mpz_class(1));

    gmpxx::mpz_class compound(0xcafe);
    compound &= b;
    require_mpz_equal(compound, gmpxx::mpz_class(0x8aee));
    compound = 0xcafe;
    compound |= -b;
    require_mpz_equal(compound, gmpxx::mpz_class(-0x3401));
    compound = 0xcafe;
    compound ^= 48879.0;
    require_mpz_equal(compound, gmpxx::mpz_class(0x7411));

    require_mpz_equal((-gmpxx::mpz_class(5)) << 2u, gmpxx::mpz_class(-20));
    require_mpz_equal((gmpxx::mpz_class(5) * gmpxx::mpz_class(-4)) >> 3u, gmpxx::mpz_class(-3));

    gmpxx::mpz_class shifted(3);
    shifted <<= 4u;
    require_mpz_equal(shifted, gmpxx::mpz_class(48));
    shifted = -20;
    shifted >>= 3u;
    require_mpz_equal(shifted, gmpxx::mpz_class(-3));
}

void check_increment_decrement()
{
    gmpxx::mpz_class a(-1);
    require_mpz_equal(++a, gmpxx::mpz_class(0));
    require_mpz_equal(a++, gmpxx::mpz_class(0));
    require_mpz_equal(a, gmpxx::mpz_class(1));
    require_mpz_equal(--a, gmpxx::mpz_class(0));
    require_mpz_equal(a--, gmpxx::mpz_class(0));
    require_mpz_equal(a, gmpxx::mpz_class(-1));

    require_mpz_equal(++gmpxx::mpz_class(7), gmpxx::mpz_class(8));
    require_mpz_equal(++gmpxx::mpz_class(-8), gmpxx::mpz_class(-7));
    require_mpz_equal(--gmpxx::mpz_class(8), gmpxx::mpz_class(7));
    require_mpz_equal(--gmpxx::mpz_class(-7), gmpxx::mpz_class(-8));
    require_mpz_equal(gmpxx::mpz_class(7)++, gmpxx::mpz_class(7));
    require_mpz_equal(gmpxx::mpz_class(-8)--, gmpxx::mpz_class(-8));
}

void check_nested_shapes()
{
    {
        gmpxx::mpz_class a(17), b(-5), c(9), d;
        d = a + b * c;
        require_mpz_equal(d, gmpxx::mpz_class(-28));
        d = a - b * c;
        require_mpz_equal(d, gmpxx::mpz_class(62));
    }
    {
        gmpxx::mpz_class a(-14), b(6), c(-8), e;
        double d = 2.0;
        e = a + b * (c + d);
        require_mpz_equal(e, gmpxx::mpz_class(-50));
        e = a - b * (c + d);
        require_mpz_equal(e, gmpxx::mpz_class(22));
    }
    {
        gmpxx::mpz_class a(23), b(-11), e;
        unsigned int c = 4, d = 7;
        e = a + (b + c) * d;
        require_mpz_equal(e, gmpxx::mpz_class(-26));
        e = a - (b + c) * d;
        require_mpz_equal(e, gmpxx::mpz_class(72));
    }
    {
        gmpxx::mpz_class a(-31), b(12), c(-6), f;
        signed int d = -3, e = 10;
        f = a + (b - d) * (c + e);
        require_mpz_equal(f, gmpxx::mpz_class(29));
        f = a - (b - d) * (c + e);
        require_mpz_equal(f, gmpxx::mpz_class(-91));
    }
    {
        gmpxx::mpz_class a(-7), b(-13), c(29), d;
        d = a * b + c;
        require_mpz_equal(d, gmpxx::mpz_class(120));
        d = a * b - c;
        require_mpz_equal(d, gmpxx::mpz_class(62));
    }
    {
        gmpxx::mpz_class a(19), b(-17), e;
        double c = -3.0, d = 4.0;
        e = c * (a + d) + b;
        require_mpz_equal(e, gmpxx::mpz_class(-86));
        e = c * (a + d) - b;
        require_mpz_equal(e, gmpxx::mpz_class(-52));
    }
    {
        gmpxx::mpz_class a(-4), b(15), c(-9), f;
        unsigned int d = 6, e = 20;
        f = a * (b - d) + (c + e);
        require_mpz_equal(f, gmpxx::mpz_class(-25));
        f = a * (b - d) - (c + e);
        require_mpz_equal(f, gmpxx::mpz_class(-47));
    }
    {
        gmpxx::mpz_class a(8), b(-12), c(5), g;
        double d = -15.0, e = 3.0, f = -21.0;
        g = (a + d) * (b - e) + (c + f);
        require_mpz_equal(g, gmpxx::mpz_class(89));
        g = (a + d) * (b - e) - (c + f);
        require_mpz_equal(g, gmpxx::mpz_class(121));
    }
}

void check_integer_helpers()
{
    require_mpz_equal(gmpxx::gcd(gmpxx::mpz_class(6), gmpxx::mpz_class(8)), gmpxx::mpz_class(2));
    require_mpz_equal(gmpxx::gcd(-gmpxx::mpz_class(6), gmpxx::mpz_class(8)), gmpxx::mpz_class(2));
    require_mpz_equal(gmpxx::gcd(static_cast<long>(-6), gmpxx::mpz_class(5) + 3), gmpxx::mpz_class(2));
    require_mpz_equal(gmpxx::lcm(gmpxx::mpz_class(6), gmpxx::mpz_class(8)), gmpxx::mpz_class(24));
    require_mpz_equal(gmpxx::lcm(-gmpxx::mpz_class(6), -gmpxx::mpz_class(8)), gmpxx::mpz_class(24));
    require_mpz_equal(gmpxx::lcm(-6.0, gmpxx::mpz_class(5) + 3), gmpxx::mpz_class(24));

    require_mpz_equal(gmpxx::factorial(gmpxx::mpz_class(3)), gmpxx::mpz_class(6));
    require_mpz_equal(gmpxx::factorial(gmpxx::mpz_class(5) - 1), gmpxx::mpz_class(24));
    require_mpz_equal(gmpxx::mpz_class::factorial(gmpxx::mpz_class(2) * 2), gmpxx::mpz_class(24));
    require_mpz_equal(gmpxx::mpz_class::factorial(3.0f), gmpxx::mpz_class(6));
    require_mpz_equal(gmpxx::primorial(gmpxx::mpz_class(5)), gmpxx::mpz_class(30));
    require_mpz_equal(gmpxx::primorial(gmpxx::mpz_class(2) * 2), gmpxx::mpz_class(6));
    require_mpz_equal(gmpxx::mpz_class::primorial(gmpxx::mpz_class(2) * 2), gmpxx::mpz_class(6));
    require_mpz_equal(gmpxx::mpz_class::primorial(3.0f), gmpxx::mpz_class(6));
    require_mpz_equal(gmpxx::fibonacci(gmpxx::mpz_class(6)), gmpxx::mpz_class(8));
    require_mpz_equal(gmpxx::fibonacci(gmpxx::mpz_class(2) * 2), gmpxx::mpz_class(3));
    require_mpz_equal(gmpxx::fibonacci(-gmpxx::mpz_class(6)), gmpxx::mpz_class(-8));
    require_mpz_equal(gmpxx::mpz_class::fibonacci(gmpxx::mpz_class(2) * 2), gmpxx::mpz_class(3));
    require_mpz_equal(gmpxx::mpz_class::fibonacci(3.0f), gmpxx::mpz_class(2));
}

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() +
                           std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() - 5LL)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() + 0.5)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(2.0 * std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(5LL * std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() &
                           std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() ^ 1.0)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(~std::declval<const gmpxx::mpz_class&>())>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() << 3u)>);
    static_assert(!std::is_same_v<
                  decltype(std::declval<const gmpxx::mpz_class&>() +
                           std::declval<const gmpxx::mpz_class&>()),
                  gmpxx::mpz_class>);

    gmpxx::mpz_class zero(std::int64_t{0});
    gmpxx::mpz_class one(std::int64_t{1});
    gmpxx::mpz_class neg_one(std::int64_t{-1});
    gmpxx::mpz_class max_i(std::numeric_limits<std::int64_t>::max());
    gmpxx::mpz_class min_i(std::numeric_limits<std::int64_t>::min());
    gmpxx::mpz_class max_u(std::numeric_limits<std::uint64_t>::max());
    gmpxx::mpz_class huge("1267650600228229401496703205376");
    gmpxx::mpz_class neg_huge("-100000000000000000000000000000000000001");

    check_binary(zero, one);
    check_binary(one, neg_one);
    check_binary(max_i, neg_one);
    check_binary(min_i, one);
    check_binary(max_u, huge);
    check_binary(neg_huge, max_i);
    check_scalar(huge);
    check_scalar(neg_huge);
    check_division_by_zero_throws();
    test_mpz_modulo_by_zero_throws();
    check_bitwise_and_shifts();
    check_increment_decrement();
    check_integer_helpers();
    check_nested_shapes();

    require_mpz_equal(gmpxx::mpz_class(huge + 0.75), huge);

    return 0;
}
