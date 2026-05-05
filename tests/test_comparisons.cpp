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
#include <type_traits>

namespace {

template <typename L, typename R>
using equal_expr = decltype(std::declval<const L&>() == std::declval<const R&>());

template <typename L, typename R>
using less_expr = decltype(std::declval<const L&>() < std::declval<const R&>());

template <typename L, typename R>
void check_consistency(const L& lhs, const R& rhs)
{
    const int r = cmp(lhs, rhs);
    assert((lhs == rhs) == (r == 0));
    assert((lhs != rhs) == (r != 0));
    assert((lhs < rhs) == (r < 0));
    assert((lhs <= rhs) == (r <= 0));
    assert((lhs > rhs) == (r > 0));
    assert((lhs >= rhs) == (r >= 0));
}

void test_exact_and_mpf_objects()
{
    static_assert(std::is_same<equal_expr<gmpxx::mpz_class, gmpxx::mpz_class>, bool>::value, "");
    static_assert(std::is_same<less_expr<gmpxx::mpq_class, gmpxx::mpz_class>, bool>::value, "");
    static_assert(std::is_same<equal_expr<gmpxx::mpf_class, gmpxx::mpq_class>, bool>::value, "");

    gmpxx::mpz_class z0(std::int64_t{0});
    gmpxx::mpz_class z1(std::int64_t{1});
    gmpxx::mpz_class z2(std::int64_t{2});
    gmpxx::mpq_class half("1/2");
    gmpxx::mpq_class also_half("2/4");
    gmpxx::mpq_class three_halves("3/2");

    assert(half == also_half);
    assert(z0 < half);
    assert(z1 > half);
    assert(z2 > three_halves);

    gmpxx::mpf_class f("1.5", 256);
    assert(f > z1);
    assert(f < z2);
    assert(f == three_halves);

    check_consistency(z1, z2);
    check_consistency(half, also_half);
    check_consistency(z1, half);
    check_consistency(f, z1);
    check_consistency(f, three_halves);
}

void test_expression_comparisons()
{
    gmpxx::mpz_class two(std::int64_t{2});
    gmpxx::mpz_class three(std::int64_t{3});
    gmpxx::mpz_class five(std::int64_t{5});
    assert((two + three) == five);
    assert((two * three) > five);

    gmpxx::mpq_class half("1/2");
    gmpxx::mpq_class third("1/3");
    gmpxx::mpq_class five_sixths("5/6");
    assert((half + third) == five_sixths);

    gmpxx::mpf_class one_point_two_five("1.25", 256);
    gmpxx::mpf_class two_point_five("2.5", 512);
    assert((one_point_two_five + one_point_two_five) == two_point_five);
    assert((two + half) == two_point_five);
    assert((two_point_five - half) == two);

    check_consistency(two + three, five);
    check_consistency(half + third, five_sixths);
    check_consistency(one_point_two_five + one_point_two_five, two_point_five);
}

void test_scalar_comparisons()
{
    gmpxx::mpz_class z(std::int64_t{5});
    assert(z == 5);
    assert(5 == z);
    assert(z < 6);
    assert(6 > z);

    gmpxx::mpq_class q("5/2");
    assert(q > 2);
    assert(2 < q);
    assert(q == 2.5);

    gmpxx::mpf_class f("5.5", 256);
    assert(f == 5.5);
    assert(5.5 == f);
    assert(f > 5);
    assert(5 < f);

    check_consistency(z, 5);
    check_consistency(5, z);
    check_consistency(q, 2.5);
    check_consistency(2.5, q);
    check_consistency(f, 5.5);
    check_consistency(5.5, f);

    const std::int64_t min_i = std::numeric_limits<std::int64_t>::min();
    const std::int64_t max_i = std::numeric_limits<std::int64_t>::max();
    const std::uint64_t max_u = std::numeric_limits<std::uint64_t>::max();
    gmpxx::mpz_class min_z(min_i);
    gmpxx::mpz_class max_z(max_i);
    gmpxx::mpz_class max_u_z(max_u);
    assert(min_z == min_i);
    assert(min_i == min_z);
    assert(max_z == max_i);
    assert(max_i == max_z);
    assert(max_u_z == max_u);
    assert(max_u == max_u_z);
    check_consistency(min_z, min_i);
    check_consistency(max_i, max_z);
    check_consistency(max_u_z, max_u);

#if defined(__SIZEOF_INT128__)
    const __int128_t wide =
        static_cast<__int128_t>(0x0123456789ABCDEFULL) *
        static_cast<__int128_t>(0x0FEDCBA987654321ULL);
    const __uint128_t unsigned_wide =
        static_cast<__uint128_t>(0xFEDCBA9876543210ULL) *
        static_cast<__uint128_t>(0xFFFFFFFFFFFFFFFFULL);
    gmpxx::mpz_class wide_z(wide);
    gmpxx::mpq_class unsigned_wide_q{gmpxx::mpz_class(unsigned_wide)};
    assert(wide_z == wide);
    assert(wide == wide_z);
    assert(unsigned_wide_q == unsigned_wide);
    assert(unsigned_wide == unsigned_wide_q);
    check_consistency(wide_z, wide);
    check_consistency(wide, wide_z);
    check_consistency(unsigned_wide_q, unsigned_wide);
    check_consistency(unsigned_wide, unsigned_wide_q);
#endif
}

} // namespace

int main()
{
    test_exact_and_mpf_objects();
    test_expression_comparisons();
    test_scalar_comparisons();
    return 0;
}
