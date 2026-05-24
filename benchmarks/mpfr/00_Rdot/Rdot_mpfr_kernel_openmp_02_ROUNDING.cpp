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

#pragma omp parallel
    {
        mpfr_class partial(0.0, precision);
        auto partial_context = mpfrxx::with_context(partial, context);

#pragma omp for schedule(static)
        for (int64_t i = 0; i < n; ++i) {
            mpfr_class templ(0.0, precision);
            auto templ_context = mpfrxx::with_context(templ, context);
            templ_context = dx[i] * dy[i];
            partial_context += templ;
        }

#pragma omp critical
        {
            auto result_context = mpfrxx::with_context(result, context);
            result_context += partial;
        }
    }

    return result;
}

int main(int argc, char **argv) {
    return run_class_rdot_benchmark(argc, argv, _Rdot);
}
