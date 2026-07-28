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
#include <type_traits>
#include <utility>

#if !defined(mpfr_float128) && defined(__SIZEOF_FLOAT128__) && \
    (defined(__GNUC__) || defined(__clang__))
#define mpfr_float128 __float128
#endif

#if defined(mpfr_float128) && !defined(MPFR_WANT_FLOAT128)
#define MPFR_WANT_FLOAT128
#endif

#include <mpfrxx_mkII.h>

#if LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
#define GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY80 1
#else
#define GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY80 0
#endif

#if defined(__FLT64X_MANT_DIG__) && defined(__FLT64X_MAX_EXP__) && \
    __FLT64X_MANT_DIG__ == 64 && __FLT64X_MAX_EXP__ == 16384 && \
    LDBL_MANT_DIG >= 64 && LDBL_MAX_EXP >= 16384
#define GMPFRXX_MKII_ADAPTERS_HAVE_FLOAT64X_BINARY80 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_FLOAT64X_BINARY80 0
#endif

#if LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
#define GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128 1
#else
#define GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128 0
#endif

#if defined(__FLT128_MANT_DIG__) && defined(__FLT128_MAX_EXP__) && \
    __FLT128_MANT_DIG__ == 113 && __FLT128_MAX_EXP__ == 16384
#define GMPFRXX_MKII_ADAPTERS_HAVE_ISO_FLOAT128 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_ISO_FLOAT128 0
#endif

#if defined(__SIZEOF_FLOAT128__) && (defined(__GNUC__) || defined(__clang__))
#define GMPFRXX_MKII_ADAPTERS_HAVE_GNU_FLOAT128 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_GNU_FLOAT128 0
#endif

#if defined(mpfr_float128)
#define GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128 0
#endif

#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY80 || \
    GMPFRXX_MKII_ADAPTERS_HAVE_FLOAT64X_BINARY80
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY80 0
#endif

#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128 || \
    GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128 1
#else
#define GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128 0
#endif

#if GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128
extern "C" {
__MPFR_DECLSPEC int mpfr_set_float128(mpfr_ptr, mpfr_float128, mpfr_rnd_t);
__MPFR_DECLSPEC mpfr_float128 mpfr_get_float128(mpfr_srcptr, mpfr_rnd_t);
}
#endif

namespace gmpfrxx_mkII {
namespace detail {

template <typename>
struct binary_import_dependent_false : std::false_type {};

template <typename Native>
struct binary80_native_traits {
    using native_type = std::remove_cv_t<std::remove_reference_t<Native>>;

    static constexpr bool enabled =
#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY80
        std::is_same_v<native_type, long double> ||
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_FLOAT64X_BINARY80
        std::is_same_v<native_type, _Float64x> ||
#endif
        false;

    static void set(mpfr_t dest, native_type value, mpfr_rnd_t rounding_mode)
    {
        static_assert(enabled, "unsupported native type for binary80 import");
        mpfr_set_ld(dest, static_cast<long double>(value), rounding_mode);
    }
};

template <typename Native>
inline constexpr bool is_binary80_native_v =
    binary80_native_traits<std::remove_cv_t<std::remove_reference_t<Native>>>::enabled;

template <typename Native>
struct binary128_native_traits {
    using native_type = std::remove_cv_t<std::remove_reference_t<Native>>;

    static constexpr bool enabled =
#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128
        std::is_same_v<native_type, long double> ||
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128
        std::is_same_v<native_type, mpfr_float128> ||
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_ISO_FLOAT128
        std::is_same_v<native_type, _Float128> ||
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_GNU_FLOAT128
        std::is_same_v<native_type, __float128> ||
#endif
        false;

    static void set(mpfr_t dest, native_type value, mpfr_rnd_t rounding_mode)
    {
        if constexpr (!enabled) {
            static_assert(binary_import_dependent_false<native_type>::value,
                          "unsupported native type for binary128 import");
        } else {
#if GMPFRXX_MKII_ADAPTERS_LDBL_IS_BINARY128
            if constexpr (std::is_same_v<native_type, long double>) {
                mpfr_set_ld(dest, value, rounding_mode);
            } else
#endif
            {
#if GMPFRXX_MKII_ADAPTERS_HAVE_MPFR_FLOAT128
                mpfr_set_float128(dest, static_cast<mpfr_float128>(value), rounding_mode);
#else
                static_assert(binary_import_dependent_false<native_type>::value,
                              "binary128 native type requires MPFR float128 support");
#endif
            }
        }
    }
};

template <typename Native>
inline constexpr bool is_binary128_native_v =
    binary128_native_traits<std::remove_cv_t<std::remove_reference_t<Native>>>::enabled;

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpfrxx_mkII {
namespace adapters {

template <typename Native>
class binary80_real {
public:
    using value_type = std::remove_cv_t<std::remove_reference_t<Native>>;

    value_type value{};

    constexpr binary80_real() = default;
    constexpr explicit binary80_real(value_type input) : value(input) {}
};

template <typename Native>
binary80_real(Native) -> binary80_real<std::remove_cv_t<std::remove_reference_t<Native>>>;

template <typename Native>
using binary80_source = binary80_real<Native>;

template <typename Native>
using binary80 = binary80_real<Native>;

template <typename Native>
class binary80_complex {
public:
    using value_type = std::remove_cv_t<std::remove_reference_t<Native>>;
    using real_type = binary80_real<value_type>;

    constexpr binary80_complex() = default;
    constexpr binary80_complex(real_type real_part, real_type imag_part)
        : real_(real_part), imag_(imag_part)
    {
    }
    constexpr binary80_complex(value_type real_part, value_type imag_part)
        : real_(real_part), imag_(imag_part)
    {
    }

    constexpr real_type& real() { return real_; }
    constexpr const real_type& real() const { return real_; }
    constexpr real_type& imag() { return imag_; }
    constexpr const real_type& imag() const { return imag_; }

private:
    real_type real_{};
    real_type imag_{};
};

template <typename Native>
binary80_complex(binary80_real<Native>, binary80_real<Native>) -> binary80_complex<Native>;

template <typename Native>
binary80_complex(Native, Native) -> binary80_complex<std::remove_cv_t<std::remove_reference_t<Native>>>;

template <typename Native>
using binary80_complex_source = binary80_complex<Native>;

template <typename Native>
class binary128_real {
public:
    using value_type = std::remove_cv_t<std::remove_reference_t<Native>>;

    value_type value{};

    constexpr binary128_real() = default;
    constexpr explicit binary128_real(value_type input) : value(input) {}
};

template <typename Native>
binary128_real(Native) -> binary128_real<std::remove_cv_t<std::remove_reference_t<Native>>>;

template <typename Native>
using binary128_source = binary128_real<Native>;

template <typename Native>
using binary128 = binary128_real<Native>;

template <typename Native>
class binary128_complex {
public:
    using value_type = std::remove_cv_t<std::remove_reference_t<Native>>;
    using real_type = binary128_real<value_type>;

    constexpr binary128_complex() = default;
    constexpr binary128_complex(real_type real_part, real_type imag_part)
        : real_(real_part), imag_(imag_part)
    {
    }
    constexpr binary128_complex(value_type real_part, value_type imag_part)
        : real_(real_part), imag_(imag_part)
    {
    }

    constexpr real_type& real() { return real_; }
    constexpr const real_type& real() const { return real_; }
    constexpr real_type& imag() { return imag_; }
    constexpr const real_type& imag() const { return imag_; }

private:
    real_type real_{};
    real_type imag_{};
};

template <typename Native>
binary128_complex(binary128_real<Native>, binary128_real<Native>) -> binary128_complex<Native>;

template <typename Native>
binary128_complex(Native, Native) -> binary128_complex<std::remove_cv_t<std::remove_reference_t<Native>>>;

template <typename Native>
using binary128_complex_source = binary128_complex<Native>;

template <typename Native>
constexpr auto make_binary80_source(Native value)
{
    return binary80_real<std::remove_cv_t<std::remove_reference_t<Native>>>(value);
}

template <typename Native>
constexpr auto make_binary80_complex_source(Native real, Native imag)
{
    using value_type = std::remove_cv_t<std::remove_reference_t<Native>>;
    return binary80_complex<value_type>(real, imag);
}

template <typename Native>
constexpr auto make_binary128_source(Native value)
{
    return binary128_real<std::remove_cv_t<std::remove_reference_t<Native>>>(value);
}

template <typename Native>
constexpr auto make_binary128_complex_source(Native real, Native imag)
{
    using value_type = std::remove_cv_t<std::remove_reference_t<Native>>;
    return binary128_complex<value_type>(real, imag);
}

} // namespace adapters
} // namespace gmpfrxx_mkII

namespace gmpfrxx_mkII {
namespace detail {

template <typename Native>
inline void set_mpfr_from_binary80(
    mpfr_t dest,
    const gmpfrxx_mkII::adapters::binary80_real<Native>& value,
    mpfr_rnd_t rounding_mode)
{
    binary80_native_traits<Native>::set(dest, value.value, rounding_mode);
}

template <typename Native>
inline void set_mpfr_from_binary128(
    mpfr_t dest,
    const gmpfrxx_mkII::adapters::binary128_real<Native>& value,
    mpfr_rnd_t rounding_mode)
{
    binary128_native_traits<Native>::set(dest, value.value, rounding_mode);
}

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_ADAPTERS_DETAIL_BINARY_FLOAT_HPP
