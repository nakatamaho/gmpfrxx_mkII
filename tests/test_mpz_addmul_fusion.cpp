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

void assert_equal(const mpfrxx::mpz_class& got, const mpz_t expected)
{
    assert(mpz_cmp(got.get_mpz_t(), expected) == 0);
}

void check_addmul(const mpfrxx::mpz_class& a0, const mpfrxx::mpz_class& b, const mpfrxx::mpz_class& c)
{
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_addmul(ref, b.get_mpz_t(), c.get_mpz_t());

    mpfrxx::mpz_class a = a0;
    a += b * c;
    assert_equal(a, ref);
    mpz_clear(ref);
}

void check_submul(const mpfrxx::mpz_class& a0, const mpfrxx::mpz_class& b, const mpfrxx::mpz_class& c)
{
    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_submul(ref, b.get_mpz_t(), c.get_mpz_t());

    mpfrxx::mpz_class a = a0;
    a -= b * c;
    assert_equal(a, ref);
    mpz_clear(ref);
}

template <typename Scalar>
void check_scalar(const mpfrxx::mpz_class& a0, const mpfrxx::mpz_class& b, Scalar scalar)
{
    mpfrxx::mpz_class scalar_z(scalar);
    mpz_t add_ref;
    mpz_t sub_ref;
    mpz_init_set(add_ref, a0.get_mpz_t());
    mpz_init_set(sub_ref, a0.get_mpz_t());
    mpz_addmul(add_ref, b.get_mpz_t(), scalar_z.get_mpz_t());
    mpz_submul(sub_ref, b.get_mpz_t(), scalar_z.get_mpz_t());

    mpfrxx::mpz_class a = a0;
    a += b * scalar;
    assert_equal(a, add_ref);

    a = a0;
    a += scalar * b;
    assert_equal(a, add_ref);

    a = a0;
    a -= b * scalar;
    assert_equal(a, sub_ref);

    a = a0;
    a -= scalar * b;
    assert_equal(a, sub_ref);

    mpz_clear(sub_ref);
    mpz_clear(add_ref);
}

void check_alias_cases()
{
    mpfrxx::mpz_class a0("123456789012345678901234567890");
    mpfrxx::mpz_class b("-98765432109876543210987654321");

    check_addmul(a0, a0, b);
    check_addmul(a0, b, a0);
    check_submul(a0, a0, b);
    check_submul(a0, b, a0);

    mpz_t ref;
    mpz_init_set(ref, a0.get_mpz_t());
    mpz_addmul_ui(ref, a0.get_mpz_t(), 5UL);
    mpfrxx::mpz_class a = a0;
    a += a * 5LL;
    assert_equal(a, ref);
    mpz_clear(ref);
}

void check_non_fused_shapes_still_evaluate()
{
    mpfrxx::mpz_class a0("12345");
    mpfrxx::mpz_class b("23");
    mpfrxx::mpz_class c("-31");
    mpfrxx::mpz_class d("7");

    mpfrxx::mpz_class expected = a0;
    expected += b * c * d;
    mpfrxx::mpz_class a = a0;
    a += b * c * d;
    assert(a == expected);

    expected = a0;
    expected -= (b + c) * d;
    a = a0;
    a -= (b + c) * d;
    assert(a == expected);

    expected = a0;
    expected += -b * c;
    a = a0;
    a += -b * c;
    assert(a == expected);
}

} // namespace

int main()
{
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(std::declval<mpfrxx::mpz_class>() * 5LL)>);
    static_assert(std::is_same_v<typename decltype(std::declval<mpfrxx::mpz_class>() * 5LL)::result_type,
                                 mpfrxx::mpz_class>);

    mpfrxx::mpz_class a("123456789012345678901234567890");
    mpfrxx::mpz_class b("-98765432109876543210987654321");
    mpfrxx::mpz_class c("1122334455667788990011223344556677889900");

    check_addmul(a, b, c);
    check_submul(a, b, c);

    for (std::int64_t s : {std::int64_t{0},
                           std::int64_t{1},
                           std::int64_t{-1},
                           std::int64_t{5},
                           std::int64_t{-5},
                           std::numeric_limits<std::int64_t>::min(),
                           std::numeric_limits<std::int64_t>::max()}) {
        check_scalar(a, b, s);
    }

    for (std::uint64_t s : {std::uint64_t{0},
                            std::uint64_t{1},
                            std::uint64_t{5},
                            std::numeric_limits<std::uint64_t>::max()}) {
        check_scalar(a, b, s);
    }

    check_alias_cases();
    check_non_fused_shapes_still_evaluate();
    return 0;
}
