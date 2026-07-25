#ifndef GMPFRXX_MKII_ADAPTERS_QD_COMPLEX_HPP
#define GMPFRXX_MKII_ADAPTERS_QD_COMPLEX_HPP

#include <gmpfrxx_mkII/adapters/qd_real.hpp>
#include <gmpfrxx_mkII/adapters/detail/complex_components.hpp>

#include <type_traits>
#include <utility>

namespace gmpfrxx_mkII {
namespace detail {

template <typename Complex>
struct external_mpc_complex_traits<
    Complex,
    std::enable_if_t<
        std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(std::declval<const Complex&>().real())>>, qd_real> &&
        std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(std::declval<const Complex&>().imag())>>, qd_real>>> {
    static constexpr bool enabled = true;

    static void set(mpc_t dest, const Complex& value, mpc_rnd_t rounding_mode)
    {
        set_mpc_from_external_real_components(
            dest, value.real(), value.imag(), rounding_mode);
    }
};

} // namespace detail
} // namespace gmpfrxx_mkII

#endif
