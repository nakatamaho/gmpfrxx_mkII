#ifndef GMPFRXX_MKII_ADAPTERS_MPFC_COMPLEX_HPP
#define GMPFRXX_MKII_ADAPTERS_MPFC_COMPLEX_HPP

#include <gmpfrxx_mkII.h>
#include <gmpfrxx_mkII/adapters/detail/complex_components.hpp>
#include <gmpfrxx_mkII/adapters/mpf_real.hpp>

namespace gmpfrxx_mkII {
namespace detail {

template <>
struct external_mpc_complex_traits<gmpxx::mpfc_class> {
    static constexpr bool enabled = true;

    static void set(mpc_t dest,
                    const gmpxx::mpfc_class& value,
                    mpc_rnd_t rounding_mode)
    {
        set_mpc_from_external_real_components(
            dest, value.real(), value.imag(), rounding_mode);
    }
};

} // namespace detail
} // namespace gmpfrxx_mkII

#endif
