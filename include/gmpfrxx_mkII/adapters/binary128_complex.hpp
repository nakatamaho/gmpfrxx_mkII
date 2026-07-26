#ifndef GMPFRXX_MKII_ADAPTERS_BINARY128_COMPLEX_HPP
#define GMPFRXX_MKII_ADAPTERS_BINARY128_COMPLEX_HPP

#include <gmpfrxx_mkII/adapters/binary128_real.hpp>
#include <gmpfrxx_mkII/adapters/detail/complex_components.hpp>

#include <complex>

#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128

namespace gmpfrxx_mkII {
namespace detail {

template <>
struct external_mpc_complex_traits<std::complex<_Float128>> {
    static constexpr bool enabled = true;

    static void set(mpc_t dest,
                    const std::complex<_Float128>& value,
                    mpc_rnd_t rounding_mode)
    {
        using native_type = adapters::binary128_real::value_type;
        const adapters::binary128_real real(static_cast<native_type>(value.real()));
        const adapters::binary128_real imag(static_cast<native_type>(value.imag()));
        set_mpc_from_external_real_components(dest, real, imag, rounding_mode);
    }
};

} // namespace detail
} // namespace gmpfrxx_mkII

#endif

#endif
