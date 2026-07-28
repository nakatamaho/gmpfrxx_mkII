#ifndef GMPFRXX_MKII_ADAPTERS_DETAIL_COMPLEX_COMPONENTS_HPP
#define GMPFRXX_MKII_ADAPTERS_DETAIL_COMPLEX_COMPONENTS_HPP

#include <mpcxx_mkII.h>

#include <type_traits>

namespace gmpfrxx_mkII {
namespace detail {

template <typename Real>
inline void set_mpc_from_external_real_components(mpc_t dest,
                                                   const Real& real,
                                                   const Real& imag,
                                                   mpc_rnd_t rounding_mode)
{
    using real_type = std::remove_cv_t<std::remove_reference_t<Real>>;
    static_assert(is_external_mpfr_real_v<real_type>,
                  "complex adapters require an accepted P2A real embedding");
    external_mpfr_real_traits<real_type>::set(
        mpc_realref(dest), real, MPC_RND_RE(rounding_mode));
    external_mpfr_real_traits<real_type>::set(
        mpc_imagref(dest), imag, MPC_RND_IM(rounding_mode));
    mpc_check_component_ranges(dest, rounding_mode);
}

} // namespace detail
} // namespace gmpfrxx_mkII

#endif
