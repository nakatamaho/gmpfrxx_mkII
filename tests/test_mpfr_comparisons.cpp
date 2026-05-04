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

void test_mpfr_objects_and_exact_operands()
{
    static_assert(std::is_same<equal_expr<mpfrxx::mpfr_class, mpfrxx::mpfr_class>, bool>::value, "");
    static_assert(std::is_same<less_expr<mpfrxx::mpfr_class, mpfrxx::mpq_class>, bool>::value, "");
    static_assert(std::is_same<equal_expr<mpfrxx::mpfr_class, int>, bool>::value, "");

    mpfrxx::mpz_class z1(std::int64_t{1});
    mpfrxx::mpz_class z2(std::int64_t{2});
    mpfrxx::mpq_class three_halves("3/2");
    mpfrxx::mpfr_class f("1.5", 256);

    assert(f > z1);
    assert(f < z2);
    assert(f == three_halves);
    assert(z1 < f);
    assert(three_halves == f);

    check_consistency(f, z1);
    check_consistency(z1, f);
    check_consistency(f, three_halves);
    check_consistency(three_halves, f);
}

void test_mpfr_expression_comparisons()
{
    mpfrxx::mpfr_class one_point_two_five("1.25", 256);
    mpfrxx::mpfr_class two_point_five("2.5", 512);
    mpfrxx::mpz_class two(std::int64_t{2});
    mpfrxx::mpq_class half("1/2");

    assert((one_point_two_five + one_point_two_five) == two_point_five);
    assert((two + half) == two_point_five);
    assert((two_point_five - half) == two);
    assert((two_point_five / two) == one_point_two_five);

    check_consistency(one_point_two_five + one_point_two_five, two_point_five);
    check_consistency(two + half, two_point_five);
}

void test_mpfr_scalar_comparisons()
{
    mpfrxx::mpfr_class f("5.5", 256);
    assert(f == 5.5);
    assert(5.5 == f);
    assert(f > 5);
    assert(5 < f);

    check_consistency(f, 5.5);
    check_consistency(5.5, f);
}

} // namespace

int main()
{
    test_mpfr_objects_and_exact_operands();
    test_mpfr_expression_comparisons();
    test_mpfr_scalar_comparisons();
    return 0;
}
