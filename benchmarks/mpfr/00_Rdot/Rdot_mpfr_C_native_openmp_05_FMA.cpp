// SPDX-License-Identifier: BSD-2-Clause

#include "Rdot_common.hpp"

void _Rdot(int64_t n, mpfr_t *dx, int64_t incx, mpfr_t *dy, int64_t incy, mpfr_t *ans) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const mpfr_prec_t precision = mpfr_get_prec(*ans);
    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    const int64_t unrolled_n = n - (n % 4);
    mpfr_set_d(*ans, 0.0, rnd);

#pragma omp parallel
    {
        mpfr_t acc0, acc1, acc2, acc3;
        mpfr_init2(acc0, precision);
        mpfr_init2(acc1, precision);
        mpfr_init2(acc2, precision);
        mpfr_init2(acc3, precision);
        mpfr_set_d(acc0, 0.0, rnd);
        mpfr_set_d(acc1, 0.0, rnd);
        mpfr_set_d(acc2, 0.0, rnd);
        mpfr_set_d(acc3, 0.0, rnd);

#pragma omp for schedule(static)
        for (int64_t i = 0; i < unrolled_n; i += 4) {
            mpfr_fma(acc0, dx[i], dy[i], acc0, rnd);
            mpfr_fma(acc1, dx[i + 1], dy[i + 1], acc1, rnd);
            mpfr_fma(acc2, dx[i + 2], dy[i + 2], acc2, rnd);
            mpfr_fma(acc3, dx[i + 3], dy[i + 3], acc3, rnd);
        }

#pragma omp for schedule(static)
        for (int64_t i = unrolled_n; i < n; ++i) {
            mpfr_fma(acc0, dx[i], dy[i], acc0, rnd);
        }

        mpfr_add(acc0, acc0, acc1, rnd);
        mpfr_add(acc2, acc2, acc3, rnd);
        mpfr_add(acc0, acc0, acc2, rnd);

#pragma omp critical
        { mpfr_add(*ans, *ans, acc0, rnd); }

        mpfr_clear(acc3);
        mpfr_clear(acc2);
        mpfr_clear(acc1);
        mpfr_clear(acc0);
    }
}

int main(int argc, char **argv) {
    return run_native_rdot_benchmark(argc, argv, _Rdot);
}
