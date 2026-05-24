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
 *
 */

#include "Rgemv_common.hpp"

void _Rgemv(int64_t m, int64_t n, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *x, int64_t incx, const mpf_t beta, mpf_t *y, int64_t incy) {

    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mp_bitcnt_t work_prec = mpf_get_prec(alpha);
    mpf_t temp_b, prod;
    mpf_init2(temp_b, work_prec);
    mpf_init2(prod, work_prec);

    // Scale y by beta
    for (int64_t i = 0; i < m; ++i) {
        mpf_mul(y[i], beta, y[i]); // y[i] = beta * y[i]
    }

    // Compute alpha * A * x and add to y
    for (int64_t j = 0; j < n; ++j) {
        mpf_mul(temp_b, alpha, x[j]); // temp_b = alpha * x[j]
        for (int64_t i = 0; i < m; ++i) {
            mpf_mul(prod, temp_b, A[i + j * lda]); // prod = temp_b * A[i + j*lda]
            mpf_add(y[i], y[i], prod);             // y[i] += prod
        }
    }

    mpf_clear(temp_b);
    mpf_clear(prod);
}

// Initialize a matrix with random values

int main(int argc, char **argv) {
    return rgemv_gmp_bench::run_native_rgemv_benchmark(argc, argv, _Rgemv);
}
