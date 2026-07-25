#include <gmpfrxx_mkII/adapters/mpfc_complex.hpp>

int main()
{
    gmpxx::mpf_class real;
    gmpxx::mpf_class imag;
    gmpxx::mpf_class low;
    mpf_set_d(real.mpf_data(), 1.0);
    mpf_set_d(imag.mpf_data(), -2.0);
    mpf_set_ui(low.mpf_data(), 1);
    mpf_div_2exp(low.mpf_data(), low.mpf_data(), 100);
    mpf_add(real.mpf_data(), real.mpf_data(), low.mpf_data());
    mpf_div_2exp(low.mpf_data(), low.mpf_data(), 1);
    mpf_add(imag.mpf_data(), imag.mpf_data(), low.mpf_data());

    const gmpxx::mpfc_class source(real, imag);
    const mpfrxx::mpc_class target(source);
    if (mpfr_cmp_d(mpc_realref(target.mpc_data()), 1.0) <= 0) {
        return 1;
    }
    if (mpfr_cmp_d(mpc_imagref(target.mpc_data()), -2.0) <= 0) {
        return 2;
    }
    return 0;
}
