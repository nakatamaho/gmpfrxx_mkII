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

#ifndef GMPFRXX_MKII_ADAPTERS_DETAIL_REAL_COMPONENTS_HPP
#define GMPFRXX_MKII_ADAPTERS_DETAIL_REAL_COMPONENTS_HPP

#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>

#include <cstddef>
#include <type_traits>

namespace gmpfrxx_mkII {
namespace detail {

template <typename Component>
inline void set_mpfr_from_component(mpfr_t dest, Component value, mpfr_rnd_t rounding_mode)
{
    using component_type = std::remove_cv_t<std::remove_reference_t<Component>>;
    if constexpr (std::is_same_v<component_type, double>) {
        mpfr_set_d(dest, value, rounding_mode);
    } else {
        mpfr_set_ld(dest, static_cast<long double>(value), rounding_mode);
    }
}

template <typename Real, std::size_t Components>
inline void set_mpfr_from_real_components(mpfr_t dest, const Real& value, mpfr_rnd_t rounding_mode)
{
    mpfr_t component;
    mpfr_init2(component, 64);
    set_mpfr_from_component(dest, value.x[0], rounding_mode);

    for (std::size_t i = 1; i < Components; ++i) {
        set_mpfr_from_component(component, value.x[i], MPFR_RNDN);
        mpfr_add(dest, dest, component, rounding_mode);
    }

    mpfr_clear(component);
}

} // namespace detail
} // namespace gmpfrxx_mkII

#endif
