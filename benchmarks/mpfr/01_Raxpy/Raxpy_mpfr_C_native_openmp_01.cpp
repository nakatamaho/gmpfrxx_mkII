/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <omp.h>

#include "Raxpy_common.hpp"

gmp_randstate_t state;

void _Raxpy(int64_t n, const mpfr_t alpha, mpfr_t *x, int64_t incx, mpfr_t *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

#pragma omp parallel
    {
#ifndef MPFR_C_NATIVE_USE_FMA
        mpfr_t temp;
        mpfr_init(temp);
#endif

#pragma omp for
        for (int64_t i = 0; i < n; ++i) {
#ifdef MPFR_C_NATIVE_USE_FMA
            mpfr_fma(y[i], alpha, x[i], y[i], mpfrxx::default_rounding_mode());
#else
            mpfr_mul(temp, alpha, x[i], mpfrxx::default_rounding_mode());
            mpfr_add(y[i], y[i], temp, mpfrxx::default_rounding_mode());
#endif
        }

#ifndef MPFR_C_NATIVE_USE_FMA
        mpfr_clear(temp);
#endif
    }
}

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    const int64_t N = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    mpfr_t *x = new mpfr_t[N];
    mpfr_t *y = new mpfr_t[N];
    mpfr_t alpha;
    mpfr_init2(alpha, prec);
    mpfr_urandomb(alpha, state);

    init_mpfr_vec(x, N, prec);
    init_mpfr_vec(y, N, prec);

    mpfr_class *x_mpfr_class = new mpfr_class[N];
    mpfr_class *y_mpfr_class = new mpfr_class[N];
    mpfr_class alpha_class = mpfr_class(alpha);

    for (int64_t i = 0; i < N; ++i) {
        x_mpfr_class[i] = mpfr_class(x[i]);
        y_mpfr_class[i] = mpfr_class(y[i]);
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    _Raxpy(N, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Raxpy(N, alpha_class, x_mpfr_class, 1, y_mpfr_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(N)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, y_mpfr_class, N));

    clear_mpfr_vec(x, N);
    clear_mpfr_vec(y, N);
    mpfr_clear(alpha);
    delete[] x;
    delete[] y;
    delete[] x_mpfr_class;
    delete[] y_mpfr_class;

    return EXIT_SUCCESS;
}
