/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <omp.h>

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
