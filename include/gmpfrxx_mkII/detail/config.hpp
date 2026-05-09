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

#ifndef GMPFRXX_MKII_DETAIL_CONFIG_HPP
#define GMPFRXX_MKII_DETAIL_CONFIG_HPP

#include <ostream>

#if __has_include(<gmpfrxx_mkII/detail/version.hpp>)
#include <gmpfrxx_mkII/detail/version.hpp>
#else
#define GMPFRXX_MKII_VERSION "0.0.1"
#define GMPFRXX_MKII_GIT_COMMIT_HASH "unknown"

namespace gmpfrxx_mkII {
namespace detail {

inline constexpr const char* version = GMPFRXX_MKII_VERSION;
inline constexpr const char* git_commit_hash = GMPFRXX_MKII_GIT_COMMIT_HASH;

} // namespace detail
} // namespace gmpfrxx_mkII
#endif

#if __has_include(<gmpfrxx_mkII/detail/build_config.hpp>)
#include <gmpfrxx_mkII/detail/build_config.hpp>
#else
#define GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P 0
#define GMPFRXX_MKII_MPFR_BUILDOPT_TLS 0
#define GMPFRXX_MKII_MPC_HAS_BUILDOPT_TLS_P 0
#define GMPFRXX_MKII_MPC_BUILDOPT_TLS 0
#endif

namespace gmpfrxx_mkII {
namespace detail {

struct build_options {
#ifdef GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
    static constexpr bool assume_fixed_precision_fastpath = true;
#else
    static constexpr bool assume_fixed_precision_fastpath = false;
#endif
    static constexpr bool mpfr_has_buildopt_tls_p = GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P != 0;
    static constexpr bool mpfr_buildopt_tls = GMPFRXX_MKII_MPFR_BUILDOPT_TLS != 0;
    static constexpr bool mpc_has_buildopt_tls_p = GMPFRXX_MKII_MPC_HAS_BUILDOPT_TLS_P != 0;
    static constexpr bool mpc_buildopt_tls = GMPFRXX_MKII_MPC_BUILDOPT_TLS != 0;
};

} // namespace detail
} // namespace gmpfrxx_mkII

namespace gmpxx {

inline constexpr const char* version() noexcept
{
    return ::gmpfrxx_mkII::detail::version;
}

inline std::ostream& print_version(std::ostream& out)
{
    return out << version();
}

inline constexpr const char* git_commit_hash() noexcept
{
    return ::gmpfrxx_mkII::detail::git_commit_hash;
}

inline std::ostream& print_git_commit_hash(std::ostream& out)
{
    return out << git_commit_hash();
}

} // namespace gmpxx

namespace mpfrxx {

inline constexpr const char* version() noexcept
{
    return ::gmpfrxx_mkII::detail::version;
}

inline std::ostream& print_version(std::ostream& out)
{
    return out << version();
}

inline constexpr const char* git_commit_hash() noexcept
{
    return ::gmpfrxx_mkII::detail::git_commit_hash;
}

inline std::ostream& print_git_commit_hash(std::ostream& out)
{
    return out << git_commit_hash();
}

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_CONFIG_HPP
