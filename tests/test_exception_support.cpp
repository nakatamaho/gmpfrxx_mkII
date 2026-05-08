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

void test_basic_exception_support()
{
    require_throws<std::invalid_argument>([] {
        throw std::invalid_argument("test");
    });
}

void test_exact_string_constructors()
{
    require_throws<std::invalid_argument>([] {
        (void)gmpxx::mpz_class("not-an-integer");
    });
    require_throws<std::invalid_argument>([] {
        (void)gmpxx::mpq_class("not-a-rational");
    });
    require_throws<std::invalid_argument>([] {
        (void)gmpxx::mpq_class("1/0");
    });
}

void test_mpf_string_constructors_and_assignment()
{
    require_throws<std::invalid_argument>([] {
        (void)gmpxx::mpf_class("not-a-real", 256);
    });

    gmpxx::mpf_class value("1.25", 256);
    const gmpxx::mpf_class before(value);
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
    gmpxx::gmp_randclass rng;
    require_throws<std::invalid_argument>([&] {
        (void)rng.get_z_bits(gmpxx::mpz_class(-1));
    });
    require_throws<std::invalid_argument>([&] {
        (void)rng.get_z_range(gmpxx::mpz_class(0));
    });
}

void test_mpf_domain_exceptions()
{
    require_throws<std::domain_error>([] {
        (void)gmpxx::log(gmpxx::mpf_class(-1, 256));
    });
    assert(gmpxx::pow(gmpxx::mpf_class(0, 256), gmpxx::mpf_class(0, 256)) == gmpxx::mpf_class(1, 256));
    require_throws<std::domain_error>([] {
        (void)gmpxx::pow(gmpxx::mpf_class(0, 256), gmpxx::mpf_class(-1, 256));
    });
    require_throws<std::domain_error>([] {
        (void)gmpxx::gamma(gmpxx::mpf_class(0, 256));
    });
}

void test_mpfc_exception_neutrality()
{
    const gmpxx::mpfc_class value(gmpxx::mpf_class("1.25", 256), gmpxx::mpf_class("-0.5", 256));
    assert(value.real() == gmpxx::mpf_class("1.25", 256));
    assert(value.imag() == gmpxx::mpf_class("-0.5", 256));

    require_throws<std::invalid_argument>([] {
        const gmpxx::mpf_class bad("not-a-real", 256);
        (void)gmpxx::mpfc_class(bad);
    });
}

} // namespace

int main()
{
    test_basic_exception_support();
    test_exact_string_constructors();
    test_mpf_string_constructors_and_assignment();
    test_random_argument_exceptions();
    test_mpf_domain_exceptions();
    test_mpfc_exception_neutrality();
    return 0;
}
