#ifndef GMPFRXX_MKII_ADAPTERS_BINARY80_COMPLEX_HPP
#define GMPFRXX_MKII_ADAPTERS_BINARY80_COMPLEX_HPP

#include <gmpfrxx_mkII/adapters/binary80_real.hpp>
#include <gmpfrxx_mkII/adapters/detail/complex_components.hpp>

#include <complex>

#if defined(__FLT64X_MANT_DIG__) && __FLT64X_MANT_DIG__ == 64

namespace gmpfrxx_mkII {
namespace detail {

template <>
struct external_mpc_complex_traits<std::complex<_Float64x>> {
    static constexpr bool enabled = true;

    static void set(mpc_t dest,
                    const std::complex<_Float64x>& value,
                    mpc_rnd_t rounding_mode)
    {
        const adapters::binary80_real real(static_cast<long double>(value.real()));
        const adapters::binary80_real imag(static_cast<long double>(value.imag()));
        set_mpc_from_external_real_components(dest, real, imag, rounding_mode);
    }
};

} // namespace detail
} // namespace gmpfrxx_mkII

#endif

#endif
