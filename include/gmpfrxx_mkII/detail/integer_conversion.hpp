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

#ifndef GMPFRXX_MKII_DETAIL_INTEGER_CONVERSION_HPP
#define GMPFRXX_MKII_DETAIL_INTEGER_CONVERSION_HPP

#include <cstdint>
#include <type_traits>

namespace gmpfrxx_mkII {
namespace detail {

template <typename T>
struct is_supported_expression_integral
    : std::bool_constant<std::is_integral_v<T> &&
                         !std::is_same_v<std::remove_cv_t<T>, bool> &&
                         !std::is_same_v<std::remove_cv_t<T>, wchar_t> &&
                         !std::is_same_v<std::remove_cv_t<T>, char16_t> &&
                         !std::is_same_v<std::remove_cv_t<T>, char32_t> &&
                         (sizeof(T) <= sizeof(std::uint64_t))> {};

template <typename T>
inline constexpr bool is_supported_expression_integral_v =
    is_supported_expression_integral<std::remove_cv_t<T>>::value;

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_INTEGER_CONVERSION_HPP
