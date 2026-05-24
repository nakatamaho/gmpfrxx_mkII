// SPDX-License-Identifier: BSD-2-Clause

#include "Rdot_common.hpp"

mpfr_class _Rdot(int64_t n, mpfr_class *dx, int64_t incx, mpfr_class *dy, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const mpfr_prec_t precision = n > 0 ? dx[0].precision() : mpfrxx::default_precision_bits();
    const mpfr_rnd_t rounding = mpfrxx::default_rounding_mode();
    const mpfrxx::evaluation_context context{precision, rounding};
    mpfr_class result(0.0, precision);
    const int64_t unrolled_n = n - n % 4;

#pragma omp parallel
    {
        mpfr_class acc0(0.0, precision);
        mpfr_class acc1(0.0, precision);
        mpfr_class acc2(0.0, precision);
        mpfr_class acc3(0.0, precision);
        auto acc0_context = mpfrxx::with_context(acc0, context);
        auto acc1_context = mpfrxx::with_context(acc1, context);
        auto acc2_context = mpfrxx::with_context(acc2, context);
        auto acc3_context = mpfrxx::with_context(acc3, context);

#pragma omp for schedule(static)
        for (int64_t i = 0; i < unrolled_n; i += 4) {
            acc0_context += dx[i] * dy[i];
            acc1_context += dx[i + 1] * dy[i + 1];
            acc2_context += dx[i + 2] * dy[i + 2];
            acc3_context += dx[i + 3] * dy[i + 3];
        }

#pragma omp for schedule(static)
        for (int64_t i = unrolled_n; i < n; ++i) {
            acc0_context += dx[i] * dy[i];
        }

        acc0_context += acc1;
        acc2_context += acc3;
        acc0_context += acc2;

#pragma omp critical
        {
            auto result_context = mpfrxx::with_context(result, context);
            result_context += acc0;
        }
    }

    return result;
}

int main(int argc, char **argv) {
    return run_class_rdot_benchmark(argc, argv, _Rdot);
}
