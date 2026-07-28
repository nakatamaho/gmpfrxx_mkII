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

#ifndef GMPFRXX_MKII_ADAPTERS_DD_REAL_HPP
#define GMPFRXX_MKII_ADAPTERS_DD_REAL_HPP

#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>

#include <gmpfrxx_mkII/adapters/detail/real_components.hpp>

#include <qd/dd_real.h>

namespace gmpfrxx_mkII {
namespace detail {

inline void set_mpfr_from_dd_real_components(mpfr_t dest, const dd_real& value, mpfr_rnd_t rounding_mode)
{
    set_mpfr_from_real_components<dd_real, 2>(dest, value, rounding_mode);
}

template <>
struct external_mpfr_real_traits<dd_real> {
    static constexpr bool enabled = true;

    static void set(mpfr_t dest, const dd_real& value, mpfr_rnd_t rounding_mode)
    {
        set_mpfr_from_dd_real_components(dest, value, rounding_mode);
    }
};


} // namespace detail
} // namespace gmpfrxx_mkII


#endif
