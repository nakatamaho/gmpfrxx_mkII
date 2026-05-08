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

#ifndef GMPFRXX_MKII_DETAIL_COMMON_TYPE_MACROS_HPP
#define GMPFRXX_MKII_DETAIL_COMMON_TYPE_MACROS_HPP

#define GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, SCALAR) \
    template <>                                                \
    struct common_type<CLASS, SCALAR> {                        \
        using type = CLASS;                                    \
    };                                                         \
    template <>                                                \
    struct common_type<SCALAR, CLASS> {                        \
        using type = CLASS;                                    \
    }

#define GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPES(CLASS)              \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, char);            \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, signed char);     \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned char);   \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, wchar_t);         \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, char16_t);        \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, char32_t);        \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, short);           \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned short);  \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, int);             \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned int);    \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, long);            \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned long);   \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, long long);       \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, unsigned long long); \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, float);           \
    GMPFRXX_MKII_DEFINE_BUILTIN_COMMON_TYPE(CLASS, double)

#endif // GMPFRXX_MKII_DETAIL_COMMON_TYPE_MACROS_HPP
