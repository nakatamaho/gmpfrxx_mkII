/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef GMPFRXX_MKII_ADAPTERS_EDD_REAL_HPP
#define GMPFRXX_MKII_ADAPTERS_EDD_REAL_HPP

#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>
#include <gmpfrxx_mkII/adapters/detail/real_components.hpp>
#include <qd/edd_real.h>

namespace gmpfrxx_mkII::detail {
template <> struct external_mpfr_real_traits<edd_real> {
    static constexpr bool enabled = true;
    static void set(mpfr_t dest, const edd_real& value, mpfr_rnd_t rounding_mode)
    { set_mpfr_from_real_components<edd_real, 2>(dest, value, rounding_mode); }
};
} // namespace gmpfrxx_mkII::detail

#endif
