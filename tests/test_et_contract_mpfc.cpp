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

#include <type_traits>

int main()
{
    auto a = gmpxx::mpfc_class::with_precision(128, 1.0, 2.0);
    auto b = gmpxx::mpfc_class::with_precision(128, 3.0, 4.0);
    auto r = gmpxx::mpf_class::with_precision(128, 5.0);
    gmpxx::mpz_class z(7);
    gmpxx::mpq_class q(gmpxx::mpz_class(3), gmpxx::mpz_class(2));

    auto sum = a + b;
    auto mixed = r + a;
    auto mixed_z_rhs = a + z;
    auto mixed_z_lhs = z + a;
    auto mixed_q_rhs = a + q;
    auto mixed_q_lhs = q + a;
    auto mixed_exact_expr = (z + q) + a;
    auto scalar = a + 2;
    auto unary = -a;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(sum)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed_z_rhs)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed_z_lhs)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed_q_rhs)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed_q_lhs)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed_exact_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(scalar)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);
    static_assert(!std::is_same_v<decltype(sum), gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(sum)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed_z_rhs)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed_z_lhs)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed_q_rhs)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed_q_lhs)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed_exact_expr)::result_type, gmpxx::mpfc_class>);

    return 0;
}
