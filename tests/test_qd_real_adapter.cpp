#include <gmpfrxx_mkII/adapters/qd_real.hpp>

#include <cmath>

int main()
{
    qd_real x;
    x.x[0] = 1.0;
    x.x[1] = std::ldexp(1.0, -100);
    x.x[2] = std::ldexp(1.0, -200);
    x.x[3] = std::ldexp(1.0, -300);

    mpfrxx::mpfr_class converted = x;
    mpfrxx::mpfr_class expected =
        mpfrxx::mpfr_class::with_precision(converted.precision());
    mpfrxx::mpfr_class component =
        mpfrxx::mpfr_class::with_precision(converted.precision());

    mpfr_set_d(expected.mpfr_data(), x.x[0], mpfrxx::mpfr_class::default_rounding());
    for (int i = 1; i < 4; ++i) {
        mpfr_set_d(component.mpfr_data(), x.x[i], MPFR_RNDN);
        mpfr_add(expected.mpfr_data(),
                 expected.mpfr_data(),
                 component.mpfr_data(),
                 mpfrxx::mpfr_class::default_rounding());
    }

    return mpfr_cmp(converted.mpfr_data(), expected.mpfr_data()) == 0 ? 0 : 1;
}
