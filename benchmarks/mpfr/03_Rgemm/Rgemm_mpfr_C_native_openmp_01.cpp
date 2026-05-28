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

#include "Rgemm_common.hpp"


void _Rgemm(int64_t m, int64_t k, int64_t n, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, const mpfr_t beta, mpfr_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        std::exit(EXIT_FAILURE);
    }

#pragma omp parallel for collapse(2)
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
            mpfr_mul(C[i + j * ldc], C[i + j * ldc], beta, rnd);
        }
    }

#pragma omp parallel for collapse(2)
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
            mpfr_t sum, temp;
            mpfr_init(sum);
            mpfr_init(temp);
            mpfr_set_ui(sum, 0, rnd);
            for (int64_t l = 0; l < k; ++l) {
                mpfr_mul(temp, A[i + l * lda], B[l + j * ldb], rnd);
                mpfr_add(sum, sum, temp, rnd);
            }
            mpfr_mul(sum, sum, alpha, rnd);
            mpfr_add(C[i + j * ldc], C[i + j * ldc], sum, rnd);
            mpfr_clear(sum);
            mpfr_clear(temp);
        }
    }
}

int main(int argc, char **argv) {
    return run_native_rgemm_benchmark(argc, argv, _Rgemm);
}
