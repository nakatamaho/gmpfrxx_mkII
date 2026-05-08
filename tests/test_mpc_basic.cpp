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
#include <mpcxx_mkII.h>

#include <cmath>
#include <cstdlib>
#include <type_traits>
#include <utility>

namespace {

template <typename Lhs, typename Rhs, typename = void>
struct has_less_than : std::false_type {};

template <typename Lhs, typename Rhs>
struct has_less_than<Lhs, Rhs, std::void_t<decltype(std::declval<Lhs>() < std::declval<Rhs>())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_real_member_accessor : std::false_type {};

template <typename T>
struct has_real_member_accessor<T, std::void_t<decltype(std::declval<T&>().real())>>
    : std::true_type {};

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(160, 3.0, 4.0);
    auto r = mpfrxx::mpfr_class::with_precision(160, 5.0);
    mpfrxx::mpz_class exact_z(7);
    mpfrxx::mpq_class exact_q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    mpfrxx::mpfr_class ctor_real = mpfrxx::mpfr_class::with_precision(192, 1.25);
    mpfrxx::mpfr_class ctor_imag = mpfrxx::mpfr_class::with_precision(224, -2.5);
    mpfrxx::mpc_class constructed(ctor_real, ctor_imag);
    static_assert(!has_real_member_accessor<mpfrxx::mpc_class>::value,
                  "mpc_class must not expose mutable real()/imag() component accessors");
    require_close(constructed.real_to_double(), 1.25);
    require_close(constructed.imag_to_double(), -2.5);
    if (constructed.real_precision() != 192 || constructed.imag_precision() != 224) {
        std::abort();
    }

    mpfrxx::mpc_class result = z + w;
    require_close(result.real_to_double(), 4.0);
    require_close(result.imag_to_double(), 6.0);
    if (result.precision() != 160) {
        std::abort();
    }

    result = z * w;
    require_close(result.real_to_double(), -5.0);
    require_close(result.imag_to_double(), 10.0);

    result = z + r - exact_z + exact_q + 2;
    require_close(result.real_to_double(), 1.5);
    require_close(result.imag_to_double(), 2.0);

    result = -z;
    require_close(result.real_to_double(), -1.0);
    require_close(result.imag_to_double(), -2.0);

    auto zero = mpfrxx::mpc_class::with_precision(160, 192, 0.0, 0.0);
    auto real_three = mpfrxx::mpc_class::with_precision(160, 192, 3.0, 0.0);
    auto complex_three = mpfrxx::mpc_class::with_precision(160, 192, 3.0, 1.0);
    auto same_three = mpfrxx::mpc_class::with_precision(224, 256, 3.0, 0.0);
    auto mpfr_three = mpfrxx::mpfr_class::with_precision(224, 3.0);
    if (!(zero == 0) || !(0 == zero) || zero != 0) {
        std::abort();
    }
    if (!(real_three == same_three) || real_three != same_three) {
        std::abort();
    }
    if (!(real_three == mpfr_three) || !(mpfr_three == real_three)) {
        std::abort();
    }
    mpfrxx::mpz_class exact_three(3);
    if (!(real_three == exact_three) || !(exact_three == real_three)) {
        std::abort();
    }
    mpfrxx::mpq_class exact_three_q(mpfrxx::mpz_class(6), mpfrxx::mpz_class(2));
    if (!(real_three == exact_three_q) || !(exact_three_q == real_three)) {
        std::abort();
    }
    if (!(real_three == 3) || !(3 == real_three) || !(real_three == 3.0) || !(3.0 == real_three)) {
        std::abort();
    }
    if (!(complex_three != 3) || !(3 != complex_three) || !(complex_three != mpfr_three)) {
        std::abort();
    }
    static_assert(!has_less_than<mpfrxx::mpc_class, mpfrxx::mpc_class>::value,
                  "mpc_class intentionally has no ordering comparison");
    static_assert(!has_less_than<mpfrxx::mpc_class, int>::value,
                  "mpc_class intentionally has no scalar ordering comparison");
    static_assert(!has_less_than<int, mpfrxx::mpc_class>::value,
                  "mpc_class intentionally has no scalar ordering comparison");

    mpfrxx::mpc_class compound = mpfrxx::mpc_class::with_precision(192, 224, 1.0, 2.0);
    compound += w;
    require_close(compound.real_to_double(), 4.0);
    require_close(compound.imag_to_double(), 6.0);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound -= r;
    require_close(compound.real_to_double(), -1.0);
    require_close(compound.imag_to_double(), 6.0);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound *= mpfrxx::mpc_class::with_precision(160, 2.0, -1.0);
    require_close(compound.real_to_double(), 4.0);
    require_close(compound.imag_to_double(), 13.0);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound /= mpfrxx::mpfr_class::with_precision(160, 2.0);
    require_close(compound.real_to_double(), 2.0);
    require_close(compound.imag_to_double(), 6.5);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound += z * w;
    require_close(compound.real_to_double(), -3.0);
    require_close(compound.imag_to_double(), 16.5);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    return 0;
}
