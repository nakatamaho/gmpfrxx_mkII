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

#include <type_traits>

int main()
{
    mpfrxx::mpz_class z1(2);
    mpfrxx::mpz_class z2(3);
    mpfrxx::mpq_class q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(5));
    auto r = mpfrxx::mpfr_class::with_precision(128, 1.25);

    auto zz = z1 + z2;
    auto zq = z1 + q;
    auto qq = q + q;
    auto zr = z1 + r;
    auto rq = r + q;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zz)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zq)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(qq)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(rq)>);

    static_assert(std::is_same_v<typename decltype(zz)::result_type, gmpxx::mpz_class>);
    static_assert(std::is_same_v<typename decltype(zq)::result_type, gmpxx::mpq_class>);
    static_assert(std::is_same_v<typename decltype(qq)::result_type, gmpxx::mpq_class>);
    static_assert(std::is_same_v<typename decltype(zr)::result_type, mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<typename decltype(rq)::result_type, mpfrxx::mpfr_class>);

    static_assert(!std::is_same_v<decltype(zr), mpfrxx::mpfr_class>);

    return 0;
}
