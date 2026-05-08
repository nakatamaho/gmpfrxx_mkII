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
#include <cstdint>
#include <type_traits>

namespace {

template <typename A, typename B, typename = void>
struct has_common_type : std::false_type {};

template <typename A, typename B>
struct has_common_type<A, B, std::void_t<std::common_type_t<A, B>>> : std::true_type {};

template <typename Expected, typename A, typename B>
void require_common_type()
{
    static_assert(std::is_same_v<std::common_type_t<A, B>, Expected>);
    static_assert(std::is_same_v<std::common_type_t<B, A>, Expected>);
}

template <class A, class B>
auto add_materialized(const A& a, const B& b)
{
    using R = std::common_type_t<A, B>;
    R r = a + b;
    return r;
}

void require_gmp_common_types()
{
    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, gmpxx::mpz_class>();
    require_common_type<gmpxx::mpq_class, gmpxx::mpz_class, gmpxx::mpq_class>();
    require_common_type<gmpxx::mpq_class, gmpxx::mpq_class, gmpxx::mpq_class>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpz_class, gmpxx::mpf_class>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpq_class, gmpxx::mpf_class>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpf_class, gmpxx::mpf_class>();
    require_common_type<gmpxx::mpfc_class, gmpxx::mpz_class, gmpxx::mpfc_class>();
    require_common_type<gmpxx::mpfc_class, gmpxx::mpq_class, gmpxx::mpfc_class>();
    require_common_type<gmpxx::mpfc_class, gmpxx::mpf_class, gmpxx::mpfc_class>();
    require_common_type<gmpxx::mpfc_class, gmpxx::mpfc_class, gmpxx::mpfc_class>();

    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, int>();
    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, std::int64_t>();
    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, std::uint64_t>();
    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, unsigned long long>();
    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, float>();
    require_common_type<gmpxx::mpz_class, gmpxx::mpz_class, double>();
    require_common_type<gmpxx::mpq_class, gmpxx::mpq_class, int>();
    require_common_type<gmpxx::mpq_class, gmpxx::mpq_class, double>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpf_class, int>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpf_class, std::int64_t>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpf_class, std::uint64_t>();
    require_common_type<gmpxx::mpf_class, gmpxx::mpf_class, double>();
    require_common_type<gmpxx::mpfc_class, gmpxx::mpfc_class, int>();
    require_common_type<gmpxx::mpfc_class, gmpxx::mpfc_class, double>();

    static_assert(!has_common_type<gmpxx::mpz_class, bool>::value);
    static_assert(!has_common_type<gmpxx::mpq_class, bool>::value);
    static_assert(!has_common_type<gmpxx::mpf_class, bool>::value);
    static_assert(!has_common_type<gmpxx::mpfc_class, bool>::value);
    static_assert(!has_common_type<gmpxx::mpf_class, long double>::value);
}

void require_mpfr_common_types()
{
    require_common_type<mpfrxx::mpfr_class, gmpxx::mpz_class, mpfrxx::mpfr_class>();
    require_common_type<mpfrxx::mpfr_class, gmpxx::mpq_class, mpfrxx::mpfr_class>();
    require_common_type<mpfrxx::mpfr_class, mpfrxx::mpfr_class, mpfrxx::mpfr_class>();
    require_common_type<mpfrxx::mpc_class, gmpxx::mpz_class, mpfrxx::mpc_class>();
    require_common_type<mpfrxx::mpc_class, gmpxx::mpq_class, mpfrxx::mpc_class>();
    require_common_type<mpfrxx::mpc_class, mpfrxx::mpfr_class, mpfrxx::mpc_class>();
    require_common_type<mpfrxx::mpc_class, mpfrxx::mpc_class, mpfrxx::mpc_class>();
}

void require_forbidden_cross_domain_common_types()
{
    static_assert(!has_common_type<gmpxx::mpf_class, mpfrxx::mpfr_class>::value);
    static_assert(!has_common_type<gmpxx::mpf_class, mpfrxx::mpc_class>::value);
    static_assert(!has_common_type<gmpxx::mpfc_class, mpfrxx::mpfr_class>::value);
    static_assert(!has_common_type<gmpxx::mpfc_class, mpfrxx::mpc_class>::value);
    require_common_type<mpfrxx::mpfr_class, double, mpfrxx::mpfr_class>();
}

void require_expression_common_types()
{
    gmpxx::mpz_class z;
    gmpxx::mpq_class q;
    gmpxx::mpf_class f;

    require_common_type<gmpxx::mpz_class, decltype(-z), gmpxx::mpz_class>();
    require_common_type<gmpxx::mpq_class, decltype(-q), gmpxx::mpq_class>();
    require_common_type<gmpxx::mpf_class, decltype(-f), gmpxx::mpf_class>();

    require_common_type<gmpxx::mpq_class, decltype(-z), gmpxx::mpq_class>();
    require_common_type<gmpxx::mpf_class, decltype(-z), gmpxx::mpf_class>();
    require_common_type<gmpxx::mpf_class, decltype(-q), gmpxx::mpf_class>();

    require_common_type<gmpxx::mpz_class, decltype(-z), decltype(z + z)>();
    require_common_type<gmpxx::mpq_class, decltype(-q), decltype(q + q)>();
    require_common_type<gmpxx::mpf_class, decltype(-f), decltype(f + f)>();

    require_common_type<gmpxx::mpq_class, decltype(-z), decltype(-q)>();
    require_common_type<gmpxx::mpf_class, decltype(-z), decltype(-f)>();
    require_common_type<gmpxx::mpf_class, decltype(-q), decltype(-f)>();
    require_common_type<gmpxx::mpz_class, decltype(-z), int>();
    require_common_type<gmpxx::mpq_class, decltype(-q), int>();
    require_common_type<gmpxx::mpf_class, decltype(-f), double>();
}

void require_materialized_add()
{
    gmpxx::mpz_class z(2);
    gmpxx::mpq_class q(3, 2);
    gmpxx::mpf_class f(4.5);
    gmpxx::mpfc_class fc(f, gmpxx::mpf_class(1.25));
    mpfrxx::mpfr_class r(5.5);
    mpfrxx::mpc_class c(r, mpfrxx::mpfr_class(2.25));

    const auto zq = add_materialized(z, q);
    static_assert(std::is_same_v<std::remove_cv_t<decltype(zq)>, gmpxx::mpq_class>);
    assert(zq == gmpxx::mpq_class(7, 2));

    const auto qf = add_materialized(q, f);
    static_assert(std::is_same_v<std::remove_cv_t<decltype(qf)>, gmpxx::mpf_class>);
    assert(qf == gmpxx::mpf_class(6.0));

    const auto zr = add_materialized(z, r);
    static_assert(std::is_same_v<std::remove_cv_t<decltype(zr)>, mpfrxx::mpfr_class>);
    assert(zr == mpfrxx::mpfr_class(7.5));

    const auto rc = add_materialized(r, c);
    static_assert(std::is_same_v<std::remove_cv_t<decltype(rc)>, mpfrxx::mpc_class>);
    assert(rc.real_to_double() == 11.0);
    assert(rc.imag_to_double() == 2.25);

    const auto ffc = add_materialized(f, fc);
    static_assert(std::is_same_v<std::remove_cv_t<decltype(ffc)>, gmpxx::mpfc_class>);
    assert(ffc.real() == gmpxx::mpf_class(9.0));
    assert(ffc.imag() == gmpxx::mpf_class(1.25));
}

} // namespace

int main()
{
    require_gmp_common_types();
    require_mpfr_common_types();
    require_forbidden_cross_domain_common_types();
    require_expression_common_types();
    require_materialized_add();
    return 0;
}
