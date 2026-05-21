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

#ifndef GMPFRXX_MKII_DETAIL_GMP_DEFAULT_CONTEXT_HPP
#define GMPFRXX_MKII_DETAIL_GMP_DEFAULT_CONTEXT_HPP

#include <gmpfrxx_mkII/detail/config.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>

#include <gmp.h>

extern "C" {

struct gmpxx_mkII_default_context_v1 {
    std::uint32_t abi_version;
    std::uint32_t struct_size;
    std::uint64_t mpf_precision_bits;
};

GMPXX_MKII_API void gmpxx_mkII_get_current_default_context_v1(
    gmpxx_mkII_default_context_v1* out) noexcept;

GMPXX_MKII_API void gmpxx_mkII_set_thread_default_context_v1(
    const gmpxx_mkII_default_context_v1* context) noexcept;

GMPXX_MKII_API void gmpxx_mkII_reset_thread_default_context_v1() noexcept;

GMPXX_MKII_API const void* gmpxx_mkII_default_context_provider_token_v1() noexcept;

GMPXX_MKII_API int gmpxx_mkII_default_context_mode_v1() noexcept;

} // extern "C"

namespace gmpfrxx_mkII {
namespace detail {

inline constexpr std::uint32_t gmp_default_context_abi_version = 1;

inline mp_bitcnt_t builtin_default_mpf_precision_bits() noexcept
{
    return 512;
}

inline bool parse_mpf_precision_bits(const char* text, std::uint64_t& out) noexcept
{
    if (text == nullptr || *text == '\0' || *text == '-') {
        return false;
    }

    std::uint64_t value = 0;
    for (const unsigned char* current = reinterpret_cast<const unsigned char*>(text);
         *current != '\0';
         ++current) {
        if (*current < static_cast<unsigned char>('0') ||
            *current > static_cast<unsigned char>('9')) {
            return false;
        }

        const std::uint64_t digit = static_cast<std::uint64_t>(*current - static_cast<unsigned char>('0'));
        if (value > (std::numeric_limits<std::uint64_t>::max() - digit) / 10U) {
            return false;
        }
        value = (value * 10U) + digit;
    }

    if (value == 0) {
        return false;
    }

    out = value;
    return true;
}

inline bool checked_uint64_to_mp_bitcnt(std::uint64_t value, mp_bitcnt_t& out) noexcept
{
    if (value == 0 ||
        value > static_cast<std::uint64_t>(std::numeric_limits<mp_bitcnt_t>::max())) {
        return false;
    }
    out = static_cast<mp_bitcnt_t>(value);
    return true;
}

inline mp_bitcnt_t checked_uint64_to_mp_bitcnt_or_abort(std::uint64_t value) noexcept
{
    mp_bitcnt_t result = 0;
    if (!checked_uint64_to_mp_bitcnt(value, result)) {
        std::fprintf(stderr, "gmpfrxx_mkII: invalid MPF default precision in provider context\n");
        std::abort();
    }
    return result;
}

inline void warn_invalid_mpf_precision_environment(const char* text) noexcept
{
    std::fprintf(stderr,
                 "gmpfrxx_mkII: ignoring invalid MPF default precision environment value '%s'; using 512-bit default\n",
                 text == nullptr ? "" : text);
}

inline mp_bitcnt_t read_default_mpf_precision_from_environment() noexcept
{
    const char* text = std::getenv("GMPXX_MKII_DEFAULT_MPF_PREC_BITS");
    if (text == nullptr) {
        text = std::getenv("GMPFRXX_MKII_DEFAULT_MPF_PREC_BITS");
    }
    if (text == nullptr) {
        text = std::getenv("MPFXX_DEFAULT_PREC_BITS");
    }
    if (text == nullptr) {
        return builtin_default_mpf_precision_bits();
    }

    std::uint64_t parsed = 0;
    mp_bitcnt_t precision = 0;
    if (!parse_mpf_precision_bits(text, parsed) ||
        !checked_uint64_to_mp_bitcnt(parsed, precision)) {
        warn_invalid_mpf_precision_environment(text);
        return builtin_default_mpf_precision_bits();
    }
    return precision;
}

inline mp_bitcnt_t frozen_env_default_mpf_precision_bits() noexcept
{
    // The environment is intentionally treated as immutable after first use.
    static const mp_bitcnt_t value = read_default_mpf_precision_from_environment();
    return value;
}

inline void validate_default_context_or_abort(const gmpxx_mkII_default_context_v1& context) noexcept
{
    if (context.abi_version != gmp_default_context_abi_version ||
        context.struct_size != sizeof(gmpxx_mkII_default_context_v1)) {
        std::fprintf(stderr,
                     "gmpfrxx_mkII: invalid MPF default context provider ABI version or size\n");
        std::abort();
    }
    (void)checked_uint64_to_mp_bitcnt_or_abort(context.mpf_precision_bits);
}

inline mp_bitcnt_t default_mpf_precision_bits() noexcept
{
    if constexpr (build_options::gmp_default_context_mode == GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER) {
        gmpxx_mkII_default_context_v1 raw{};
        raw.abi_version = gmp_default_context_abi_version;
        raw.struct_size = sizeof(raw);

        gmpxx_mkII_get_current_default_context_v1(&raw);
        validate_default_context_or_abort(raw);
        return checked_uint64_to_mp_bitcnt_or_abort(raw.mpf_precision_bits);
    } else {
        return frozen_env_default_mpf_precision_bits();
    }
}

inline void set_default_mpf_precision_bits(mp_bitcnt_t precision) noexcept
{
    if (precision == 0) {
        return;
    }

    if constexpr (build_options::gmp_default_context_mode == GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER) {
        gmpxx_mkII_default_context_v1 raw{};
        raw.abi_version = gmp_default_context_abi_version;
        raw.struct_size = sizeof(raw);
        raw.mpf_precision_bits = static_cast<std::uint64_t>(precision);
        validate_default_context_or_abort(raw);
        gmpxx_mkII_set_thread_default_context_v1(&raw);
    } else {
        // Frozen-env mode has no mutable header-owned storage.  This compatibility
        // API is intentionally a no-op; default construction keeps using the frozen
        // environment-derived value.
        (void)precision;
    }
}

inline void reload_default_mpf_precision_bits_from_environment() noexcept
{
    if constexpr (build_options::gmp_default_context_mode == GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER) {
        gmpxx_mkII_reset_thread_default_context_v1();
    } else {
        // Runtime environment mutation is unsupported in frozen-env mode.
        (void)frozen_env_default_mpf_precision_bits();
    }
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

inline mp_bitcnt_t builtin_default_mpf_precision_bits() noexcept
{
    return ::gmpfrxx_mkII::detail::builtin_default_mpf_precision_bits();
}

inline mp_bitcnt_t default_mpf_precision_bits() noexcept
{
    return ::gmpfrxx_mkII::detail::default_mpf_precision_bits();
}

inline void set_default_mpf_precision_bits(mp_bitcnt_t precision) noexcept
{
    // In frozen-env builds this compatibility setter is intentionally a no-op;
    // mutable per-thread MPF defaults require the external-provider mode.
    ::gmpfrxx_mkII::detail::set_default_mpf_precision_bits(precision);
}

inline void reload_default_mpf_precision_bits_from_environment() noexcept
{
    ::gmpfrxx_mkII::detail::reload_default_mpf_precision_bits_from_environment();
}

inline void unsafe_set_gmp_global_mpf_precision_bits(mp_bitcnt_t precision) noexcept
{
    if (precision != 0) {
        mpf_set_default_prec(precision);
    }
}

inline mp_bitcnt_t unsafe_get_gmp_global_mpf_precision_bits() noexcept
{
    return mpf_get_default_prec();
}

#if GMPXX_MKII_DEFAULT_CONTEXT_MODE == GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER
class default_mpf_precision_guard {
public:
    explicit default_mpf_precision_guard(mp_bitcnt_t precision_bits) noexcept
        : old_precision_bits_(default_mpf_precision_bits())
    {
        set_default_mpf_precision_bits(precision_bits);
    }

    ~default_mpf_precision_guard()
    {
        set_default_mpf_precision_bits(old_precision_bits_);
    }

    default_mpf_precision_guard(const default_mpf_precision_guard&) = delete;
    default_mpf_precision_guard& operator=(const default_mpf_precision_guard&) = delete;

private:
    mp_bitcnt_t old_precision_bits_;
};
#endif

} // namespace gmpxx

#endif // GMPFRXX_MKII_DETAIL_GMP_DEFAULT_CONTEXT_HPP
