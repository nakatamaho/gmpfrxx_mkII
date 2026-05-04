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
    auto a = gmpxx::mpf_class::with_precision(128, 1.0);
    auto b = gmpxx::mpf_class::with_precision(128, 2.0);
    auto c = gmpxx::mpf_class::with_precision(128, 3.0);

    auto product = b * c;
    auto expr = a + product;
    auto scalar = a + 5;
    auto unary = -a;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(product)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(scalar)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);
    static_assert(!std::is_same_v<decltype(expr), gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename decltype(expr)::result_type, gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename decltype(scalar)::rhs_type::value_type, std::int64_t>);

    return 0;
}
