#include <gmpfrxx_mkII/adapters/single_real.hpp>
#include <qd/qd_config.h>

#include <cmath>
#include <cstddef>
#include <type_traits>

namespace {
template <typename Component>
void set_expected_component(mpfr_t dest, Component value, mpfr_rnd_t rounding_mode)
{
    using component_type = std::remove_cv_t<std::remove_reference_t<Component>>;
    if constexpr (std::is_same_v<component_type, double>) {
        mpfr_set_d(dest, value, rounding_mode);
    } else {
        mpfr_set_ld(dest, static_cast<long double>(value), rounding_mode);
    }
}

template <typename T, std::size_t Components>
int check_components(const T& value)
{
    mpfrxx::mpfr_class converted(value);
    mpfrxx::mpfr_class expected =
        mpfrxx::mpfr_class::with_precision(converted.precision());
    mpfrxx::mpfr_class component =
        mpfrxx::mpfr_class::with_precision(converted.precision());
    const mpfr_rnd_t rounding_mode = mpfrxx::mpfr_class::default_rounding();

    set_expected_component(expected.mpfr_data(), value.x[0], rounding_mode);
    for (std::size_t i = 1; i < Components; ++i) {
        set_expected_component(component.mpfr_data(), value.x[i], MPFR_RNDN);
        mpfr_add(expected.mpfr_data(),
                 expected.mpfr_data(),
                 component.mpfr_data(),
                 rounding_mode);
    }

    return mpfr_cmp(converted.mpfr_data(), expected.mpfr_data()) == 0 ? 0 : 1;
}
}

int main()
{
    return check_components<ds_real, 2>(
               ds_real(1.0f, std::ldexp(1.0f, -20))) +
           check_components<ts_real, 3>(
               ts_real(1.0f,
                       std::ldexp(1.0f, -20),
                       std::ldexp(1.0f, -40))) +
           check_components<qs_real, 4>(
               qs_real(1.0f,
                       std::ldexp(1.0f, -20),
                       std::ldexp(1.0f, -40),
                       std::ldexp(1.0f, -60))) +
           check_components<td_real, 3>(
               td_real(1.0,
                       std::ldexp(1.0, -50),
                       std::ldexp(1.0, -100)))
#if defined(QD_HAVE_EDD_REAL) && QD_HAVE_EDD_REAL
           + check_components<edd_real, 2>(
               edd_real(1.0L, std::ldexp(1.0L, -70)))
#endif
           ;
}
