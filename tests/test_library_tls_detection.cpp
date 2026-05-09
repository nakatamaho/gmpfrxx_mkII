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

#include <gmpfrxx_mkII/detail/config.hpp>

#include <cstdlib>

#include <mpc.h>
#include <mpfr.h>

int main()
{
#if GMPFRXX_MKII_MPFR_HAS_BUILDOPT_TLS_P
    if (!::gmpfrxx_mkII::detail::build_options::mpfr_has_buildopt_tls_p) {
        std::abort();
    }
    if (::gmpfrxx_mkII::detail::build_options::mpfr_buildopt_tls !=
        (mpfr_buildopt_tls_p() != 0)) {
        std::abort();
    }
#else
    if (::gmpfrxx_mkII::detail::build_options::mpfr_has_buildopt_tls_p) {
        std::abort();
    }
#endif

#if GMPFRXX_MKII_MPC_HAS_BUILDOPT_TLS_P
    if (!::gmpfrxx_mkII::detail::build_options::mpc_has_buildopt_tls_p) {
        std::abort();
    }
    if (::gmpfrxx_mkII::detail::build_options::mpc_buildopt_tls !=
        (mpc_buildopt_tls_p() != 0)) {
        std::abort();
    }
#else
    if (::gmpfrxx_mkII::detail::build_options::mpc_has_buildopt_tls_p) {
        std::abort();
    }
#endif

    return 0;
}
