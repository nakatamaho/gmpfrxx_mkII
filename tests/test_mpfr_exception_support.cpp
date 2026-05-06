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

#include <cassert>
#include <stdexcept>

namespace {

template <typename Exception, typename Fn>
void require_throws(Fn fn)
{
    bool caught = false;
    try {
        fn();
    } catch (const Exception&) {
        caught = true;
    }
    assert(caught);
}

void test_exact_alias_string_constructors()
{
    require_throws<std::invalid_argument>([] {
        (void)mpfrxx::mpz_class("not-an-integer");
    });
    require_throws<std::invalid_argument>([] {
        (void)mpfrxx::mpq_class("not-a-rational");
    });
    require_throws<std::invalid_argument>([] {
        (void)mpfrxx::mpq_class("1/0");
    });
}

void test_mpfr_string_constructors_and_assignment()
{
    require_throws<std::invalid_argument>([] {
        (void)mpfrxx::mpfr_class("not-a-real", 256);
    });

    mpfrxx::mpfr_class value("1.25", 256);
    const mpfrxx::mpfr_class before(value);
    require_throws<std::invalid_argument>([&] {
        value.set("not-a-real");
    });
    assert(value == before);

    require_throws<std::invalid_argument>([&] {
        value = "not-a-real";
    });
    assert(value == before);
}

void test_random_argument_exceptions()
{
    mpfrxx::gmp_randclass rng;
    require_throws<std::invalid_argument>([&] {
        (void)rng.get_z_bits(mpfrxx::mpz_class(-1));
    });
    require_throws<std::invalid_argument>([&] {
        (void)rng.get_z_range(mpfrxx::mpz_class(0));
    });
}

void test_mpc_exception_neutrality()
{
    const mpfrxx::mpc_class z = mpfrxx::mpc_class::with_precision(160, 1.0, -2.0);
    assert(z.real_to_double() == 1.0);
    assert(z.imag_to_double() == -2.0);

    require_throws<std::invalid_argument>([] {
        const mpfrxx::mpfr_class bad("not-a-real", 160);
        (void)(mpfrxx::mpc_class::with_precision(160) + bad);
    });
}

} // namespace

int main()
{
    test_exact_alias_string_constructors();
    test_mpfr_string_constructors_and_assignment();
    test_random_argument_exceptions();
    test_mpc_exception_neutrality();
    return 0;
}
