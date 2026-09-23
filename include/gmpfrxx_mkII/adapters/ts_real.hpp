/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef GMPFRXX_MKII_ADAPTERS_TS_REAL_HPP
#define GMPFRXX_MKII_ADAPTERS_TS_REAL_HPP

#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>
#include <gmpfrxx_mkII/adapters/detail/real_components.hpp>
#include <qd/ts_real.h>

namespace gmpfrxx_mkII::detail {

template <>
struct external_mpfr_real_traits<ts_real> {
    static constexpr bool enabled = true;
    static void set(mpfr_t dest, const ts_real& value, mpfr_rnd_t rounding_mode)
    {
        set_mpfr_from_real_components<ts_real, 3>(dest, value, rounding_mode);
    }
};

} // namespace gmpfrxx_mkII::detail

#endif
