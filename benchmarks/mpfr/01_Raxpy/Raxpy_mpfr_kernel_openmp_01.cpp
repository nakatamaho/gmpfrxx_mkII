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

void _Raxpy(int64_t n, const mpfr_class &alpha, mpfr_class *x, int64_t incx, mpfr_class *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

#pragma omp parallel for schedule(static)
    for (int64_t i = 0; i < n; ++i) {
        y[i] += alpha * x[i];
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

    mpfr_t alpha_raw;
    mpfr_init2(alpha_raw, prec);
    mpfr_urandomb(alpha_raw, state);

    mpfr_t *x_raw = new mpfr_t[N];
    mpfr_t *y_raw = new mpfr_t[N];
    init_mpfr_vec(x_raw, N, prec);
    init_mpfr_vec(y_raw, N, prec);

    mpfr_class *x = new mpfr_class[N];
    mpfr_class *y = new mpfr_class[N];
    mpfr_class *yy = new mpfr_class[N];
    mpfr_class alpha = mpfr_class(alpha_raw);

    for (int64_t i = 0; i < N; ++i) {
        x[i] = mpfr_class(x_raw[i]);
        y[i] = mpfr_class(y_raw[i]);
        yy[i] = y[i];
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    _Raxpy(N, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Raxpy(N, alpha, x, 1, yy, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(N)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, yy, N));

    clear_mpfr_vec(x_raw, N);
    clear_mpfr_vec(y_raw, N);
    mpfr_clear(alpha_raw);
    delete[] x_raw;
    delete[] y_raw;
    delete[] x;
    delete[] y;
    delete[] yy;

    return EXIT_SUCCESS;
}
