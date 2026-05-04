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
    auto z = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(128, 3.0, 4.0);
    auto r = mpfrxx::mpfr_class::with_precision(128, 5.0);
    mpfrxx::mpz_class exact_z(7);
    mpfrxx::mpq_class exact_q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    auto zw = z + w;
    auto zr = z + r;
    auto rz = r + z;
    auto zexact = z + exact_z;
    auto qz = exact_q + z;
    auto scalar = z + 3;
    auto unary = -z;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zw)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(rz)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zexact)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(qz)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(scalar)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);

    static_assert(std::is_same_v<typename decltype(zw)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(zr)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(rz)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(zexact)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(qz)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(scalar)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(unary)::result_type, mpfrxx::mpc_class>);

    static_assert(!std::is_same_v<decltype(zw), mpfrxx::mpc_class>);

    return 0;
}
