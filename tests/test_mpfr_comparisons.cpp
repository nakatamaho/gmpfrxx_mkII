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

void assert_same_mpfr_value(const mpfrxx::mpfr_class& got, mpfr_srcptr expected)
{
    assert(mpfr_cmp(got.mpfr_data(), expected) == 0);
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

    const std::int64_t min_i = std::numeric_limits<std::int64_t>::min();
    const std::uint64_t max_u = std::numeric_limits<std::uint64_t>::max();
    mpfrxx::mpz_class min_z(min_i);
    mpfrxx::mpz_class max_u_z(max_u);
    mpfrxx::mpfr_class min_f(min_z, 256);
    mpfrxx::mpfr_class max_u_f(max_u_z, 256);
    assert(min_f == min_z);
    assert(min_z == min_f);
    assert(max_u_f == max_u_z);
    assert(max_u_z == max_u_f);
    check_consistency(min_f, min_z);
    check_consistency(min_z, min_f);
    check_consistency(max_u_f, max_u_z);
    check_consistency(max_u_z, max_u_f);

    mpfrxx::mpq_class rational("355/113");
    mpfrxx::mpfr_class rational_f(rational, 512);
    assert(rational_f == rational);
    assert(rational == rational_f);
    check_consistency(rational_f, rational);
    check_consistency(rational, rational_f);
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

void test_mpfr_sign_next_and_predicates()
{
    using mpfrxx::mpfr_class;

    static_assert(std::is_same<decltype(mpfrxx::abs(std::declval<const mpfr_class&>())), mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::setsign(std::declval<const mpfr_class&>(), 1)), mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::copysign(std::declval<const mpfr_class&>(),
                                                         std::declval<const mpfr_class&>())),
                               mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::nextabove(std::declval<const mpfr_class&>())), mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::nextbelow(std::declval<const mpfr_class&>())), mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::nexttoward(std::declval<const mpfr_class&>(),
                                                           std::declval<const mpfr_class&>())),
                               mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::signbit(std::declval<const mpfr_class&>())), bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::sgn(std::declval<const mpfr_class&>())), int>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::nan_p(std::declval<const mpfr_class&>())), bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::inf_p(std::declval<const mpfr_class&>())), bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::number_p(std::declval<const mpfr_class&>())), bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::integer_p(std::declval<const mpfr_class&>())), bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::zero_p(std::declval<const mpfr_class&>())), bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::regular_p(std::declval<const mpfr_class&>())), bool>::value, "");

    const mpfr_class negative("-3.5", 192);
    const mpfr_class positive("3.5", 224);
    const mpfr_class zero("0.0", 160);
    const mpfr_class integer("4.0", 192);

    mpfr_t expected;
    mpfr_init2(expected, negative.precision());

    mpfr_abs(expected, negative.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    mpfr_class got_abs = mpfrxx::abs(negative);
    assert(got_abs.precision() == negative.precision());
    assert_same_mpfr_value(got_abs, expected);

    mpfr_set_prec(expected, positive.precision());
    mpfr_setsign(expected, positive.mpfr_data(), 1, mpfrxx::mpfr_class::default_rounding());
    mpfr_class got_setsign = mpfrxx::setsign(positive, 1);
    assert(got_setsign.precision() == positive.precision());
    assert_same_mpfr_value(got_setsign, expected);

    mpfr_set_prec(expected, positive.precision());
    mpfr_copysign(expected, positive.mpfr_data(), negative.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    mpfr_class got_copysign = mpfrxx::copysign(positive, negative);
    assert(got_copysign.precision() == positive.precision());
    assert_same_mpfr_value(got_copysign, expected);

    mpfr_set_prec(expected, positive.precision());
    mpfr_set(expected, positive.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    mpfr_nextabove(expected);
    mpfr_class got_nextabove = mpfrxx::nextabove(positive);
    assert(got_nextabove.precision() == positive.precision());
    assert_same_mpfr_value(got_nextabove, expected);

    mpfr_set(expected, positive.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    mpfr_nextbelow(expected);
    mpfr_class got_nextbelow = mpfrxx::nextbelow(positive);
    assert(got_nextbelow.precision() == positive.precision());
    assert_same_mpfr_value(got_nextbelow, expected);

    mpfr_set(expected, positive.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    mpfr_nexttoward(expected, negative.mpfr_data());
    mpfr_class got_nexttoward = mpfrxx::nexttoward(positive, negative);
    assert(got_nexttoward.precision() == positive.precision());
    assert_same_mpfr_value(got_nexttoward, expected);

    mpfr_clear(expected);

    mpfr_class nan = mpfr_class::with_precision(128);
    mpfr_set_nan(nan.mpfr_data());
    mpfr_class inf = mpfr_class::with_precision(128);
    mpfr_set_inf(inf.mpfr_data(), 1);

    assert(mpfrxx::signbit(negative));
    assert(!mpfrxx::signbit(positive));
    assert(mpfrxx::sgn(negative) < 0);
    assert(mpfrxx::sgn(positive) > 0);
    assert(mpfrxx::zero_p(zero));
    assert(!mpfrxx::regular_p(zero));
    assert(mpfrxx::regular_p(positive));
    assert(mpfrxx::integer_p(integer));
    assert(!mpfrxx::integer_p(positive));
    assert(mpfrxx::number_p(positive));
    assert(!mpfrxx::number_p(inf));
    assert(mpfrxx::inf_p(inf));
    assert(mpfrxx::nan_p(nan));

    const auto expr = positive + negative;
    assert(mpfrxx::zero_p(expr));
}

void test_mpfr_comparison_helpers()
{
    using mpfrxx::mpfr_class;

    static_assert(std::is_same<decltype(mpfrxx::cmpabs(std::declval<const mpfr_class&>(),
                                                       std::declval<const mpfr_class&>())),
                               int>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::cmpabs_ui(std::declval<const mpfr_class&>(), 3ul)),
                               int>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::reldiff(std::declval<const mpfr_class&>(),
                                                        std::declval<const mpfr_class&>())),
                               mpfr_class>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::eq(std::declval<const mpfr_class&>(),
                                                   std::declval<const mpfr_class&>(),
                                                   64ul)),
                               bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::greater_p(std::declval<const mpfr_class&>(),
                                                          std::declval<const mpfr_class&>())),
                               bool>::value, "");
    static_assert(std::is_same<decltype(mpfrxx::equal_p(std::declval<const mpfr_class&>(),
                                                        std::declval<const mpfr_class&>())),
                               bool>::value, "");

    const mpfr_class negative("-4.0", 192);
    const mpfr_class positive("3.0", 224);
    const mpfr_class also_positive("3.0", 160);
    const mpfr_class larger("5.0", 192);

    assert(mpfrxx::cmpabs(negative, positive) == mpfr_cmpabs(negative.mpfr_data(), positive.mpfr_data()));
    assert(mpfrxx::cmpabs_ui(negative, 4ul) == mpfr_cmpabs_ui(negative.mpfr_data(), 4ul));
    assert(mpfrxx::eq(positive, also_positive, 32ul) == (mpfr_eq(positive.mpfr_data(), also_positive.mpfr_data(), 32ul) != 0));

    mpfr_t expected;
    mpfr_init2(expected, std::max(positive.precision(), larger.precision()));
    mpfr_reldiff(expected, positive.mpfr_data(), larger.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    const mpfr_class got_reldiff = mpfrxx::reldiff(positive, larger);
    assert(got_reldiff.precision() == std::max(positive.precision(), larger.precision()));
    assert_same_mpfr_value(got_reldiff, expected);
    mpfr_clear(expected);

    assert(mpfrxx::greater_p(larger, positive));
    assert(mpfrxx::greaterequal_p(larger, positive));
    assert(mpfrxx::greaterequal_p(positive, also_positive));
    assert(mpfrxx::less_p(positive, larger));
    assert(mpfrxx::lessequal_p(positive, larger));
    assert(mpfrxx::lessequal_p(positive, also_positive));
    assert(mpfrxx::lessgreater_p(positive, larger));
    assert(mpfrxx::equal_p(positive, also_positive));

    mpfr_class nan = mpfr_class::with_precision(128);
    mpfr_set_nan(nan.mpfr_data());
    assert(mpfrxx::unordered_p(nan, positive));
    assert(mpfrxx::unordered_p(positive, nan));
}

} // namespace

int main()
{
    test_mpfr_objects_and_exact_operands();
    test_mpfr_expression_comparisons();
    test_mpfr_scalar_comparisons();
    test_mpfr_sign_next_and_predicates();
    test_mpfr_comparison_helpers();
    return 0;
}
