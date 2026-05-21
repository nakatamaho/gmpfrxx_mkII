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

#include <cstdint>
#include <type_traits>

int main()
{
    auto x = mpfrxx::mpfr_class::with_precision(128);

    auto signed_expr = x + 3;
    auto unsigned_expr = x + std::uint64_t{4};
    auto float_expr = x + 1.25f;
    auto double_expr = 2.5 + x;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(signed_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unsigned_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(float_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(double_expr)>);

    static_assert(std::is_same_v<typename decltype(signed_expr)::result_type, mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<typename decltype(signed_expr)::rhs_type::value_type, std::int64_t>);
    static_assert(std::is_same_v<typename decltype(unsigned_expr)::rhs_type::value_type, std::uint64_t>);
    static_assert(std::is_same_v<typename decltype(float_expr)::rhs_type::value_type, double>);
    static_assert(std::is_same_v<typename decltype(double_expr)::lhs_type::value_type, double>);

    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<bool>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<wchar_t>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<char16_t>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<char32_t>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<long double>);

#if defined(__SIZEOF_INT128__)
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<__int128>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<unsigned __int128>);
#endif

    return 0;
}
