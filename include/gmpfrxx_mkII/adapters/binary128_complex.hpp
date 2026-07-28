#ifndef GMPFRXX_MKII_ADAPTERS_BINARY128_COMPLEX_HPP
#define GMPFRXX_MKII_ADAPTERS_BINARY128_COMPLEX_HPP

#include <gmpfrxx_mkII/adapters/binary128_real.hpp>
#include <gmpfrxx_mkII/adapters/detail/complex_components.hpp>

namespace gmpfrxx_mkII {
namespace detail {

template <typename Native>
struct external_mpc_complex_traits<
    gmpfrxx_mkII::adapters::binary128_complex<Native>,
    std::enable_if_t<binary128_native_traits<Native>::enabled>> {
    static constexpr bool enabled = true;

    static void set(mpc_t dest,
                    const gmpfrxx_mkII::adapters::binary128_complex<Native>& value,
                    mpc_rnd_t rounding_mode)
    {
        set_mpc_from_external_real_components(
            dest, value.real(), value.imag(), rounding_mode);
    }
};

} // namespace detail
} // namespace gmpfrxx_mkII

#endif
