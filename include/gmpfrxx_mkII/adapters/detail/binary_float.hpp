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

#ifndef GMPFRXX_MKII_ADAPTERS_DETAIL_BINARY_FLOAT_HPP
#define GMPFRXX_MKII_ADAPTERS_DETAIL_BINARY_FLOAT_HPP

#include <cfloat>

#if !defined(mpfr_float128) && defined(__SIZEOF_FLOAT128__) && \
    (defined(__GNUC__) || defined(__clang__))
#define mpfr_float128 __float128
#endif

#if defined(mpfr_float128) && !defined(MPFR_WANT_FLOAT128)
#define MPFR_WANT_FLOAT128
#endif

#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>

#if defined(mpfr_float128)
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128 0
#endif

#if LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80 0
#endif

#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128
extern "C" {
__MPFR_DECLSPEC int mpfr_set_float128(mpfr_ptr, mpfr_float128, mpfr_rnd_t);
}
#endif

namespace gmpfrxx_mkII {
namespace adapters {

#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80

class binary80_real {
public:
    using value_type = long double;

    value_type value;

    constexpr binary80_real() : value(0.0L) {}
    constexpr explicit binary80_real(value_type input) : value(input) {}
};

using binary80 = binary80_real;

class binary80_complex {
public:
    binary80_complex() = default;
    binary80_complex(binary80_real real_part, binary80_real imag_part)
        : real_(real_part), imag_(imag_part)
    {
    }

    binary80_real& real() { return real_; }
    const binary80_real& real() const { return real_; }
    binary80_real& imag() { return imag_; }
    const binary80_real& imag() const { return imag_; }

private:
    binary80_real real_;
    binary80_real imag_;
};

#endif // GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80

#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128

class binary128_real {
public:
    using value_type = mpfr_float128;

    value_type value;

    binary128_real() : value(static_cast<value_type>(0.0)) {}
    explicit binary128_real(value_type input) : value(input) {}
};

using binary128 = binary128_real;

class binary128_complex {
public:
    binary128_complex() = default;
    binary128_complex(binary128_real real_part, binary128_real imag_part)
        : real_(real_part), imag_(imag_part)
    {
    }

    binary128_real& real() { return real_; }
    const binary128_real& real() const { return real_; }
    binary128_real& imag() { return imag_; }
    const binary128_real& imag() const { return imag_; }

private:
    binary128_real real_;
    binary128_real imag_;
};

#endif // GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128

} // namespace adapters
} // namespace gmpfrxx_mkII

namespace gmpfrxx_mkII {
namespace detail {

#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80

inline void set_mpfr_from_binary80(mpfr_t dest,
                                   const gmpfrxx_mkII::adapters::binary80_real& value,
                                   mpfr_rnd_t rounding_mode)
{
    mpfr_set_ld(dest, value.value, rounding_mode);
}

#endif // GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80

#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128

inline void set_mpfr_from_binary128(mpfr_t dest,
                                    const gmpfrxx_mkII::adapters::binary128_real& value,
                                    mpfr_rnd_t rounding_mode)
{
    mpfr_set_float128(dest, value.value, rounding_mode);
}

#endif // GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_ADAPTERS_DETAIL_BINARY_FLOAT_HPP
