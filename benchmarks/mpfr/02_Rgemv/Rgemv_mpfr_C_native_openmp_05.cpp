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

#include "Rgemv_common.hpp"


void _Rgemv(int64_t m, int64_t n, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *x, int64_t incx, const mpfr_t beta, mpfr_t *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const mpfr_prec_t work_prec = m > 0 ? mpfr_get_prec(y[0]) : mpfr_get_prec(alpha);
    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    mpfr_t *scaled_x = new mpfr_t[n];

    for (int64_t j = 0; j < n; ++j) {
        mpfr_init2(scaled_x[j], work_prec);
    }

#pragma omp parallel
    {
#pragma omp for schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            mpfr_mul(scaled_x[j], alpha, x[j], rnd);
        }

        mpfr_t templ;
        mpfr_init2(templ, work_prec);

#pragma omp for schedule(static)
        for (int64_t i = 0; i < m; ++i) {
            mpfr_mul(y[i], y[i], beta, rnd);
            for (int64_t j = 0; j < n; ++j) {
                mpfr_mul(templ, scaled_x[j], A[i + j * lda], rnd);
                mpfr_add(y[i], y[i], templ, rnd);
            }
        }

        mpfr_clear(templ);
    }

    for (int64_t j = 0; j < n; ++j) {
        mpfr_clear(scaled_x[j]);
    }
    delete[] scaled_x;
}

int main(int argc, char **argv) {
    return run_native_rgemv_benchmark(argc, argv, _Rgemv);
}
