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
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "Rgemv_common.hpp"

void _Rgemv(int64_t m, int64_t n, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *x, int64_t incx, const mpf_t beta, mpf_t *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mp_bitcnt_t work_prec = mpf_get_prec(alpha);
    mpf_t *scaled_x = new mpf_t[n];
    for (int64_t j = 0; j < n; ++j) {
        mpf_init2(scaled_x[j], work_prec);
    }

#pragma omp parallel
    {
#pragma omp for schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            mpf_mul(scaled_x[j], alpha, x[j]);
        }

        mpf_t prod;
        mpf_init2(prod, work_prec);

#pragma omp for schedule(static)
        for (int64_t i = 0; i < m; ++i) {
            mpf_mul(y[i], beta, y[i]);
            for (int64_t j = 0; j < n; ++j) {
                mpf_mul(prod, scaled_x[j], A[i + j * lda]);
                mpf_add(y[i], y[i], prod);
            }
        }

        mpf_clear(prod);
    }

    for (int64_t j = 0; j < n; ++j) {
        mpf_clear(scaled_x[j]);
    }
    delete[] scaled_x;
}

int main(int argc, char **argv) {
    return rgemv_gmp_bench::run_native_rgemv_benchmark(argc, argv, _Rgemv);
}
