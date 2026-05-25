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
            mpfr_mul(C[i + j * ldc], beta, C[i + j * ldc], rnd);
        }
    }

#pragma omp parallel for
    for (int64_t j = 0; j < n; ++j) {
        const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
        mpfr_t sum, temp;
        mpfr_init(sum);
        mpfr_init(temp);
        for (int64_t l = 0; l < k; ++l) {
            mpfr_mul(temp, alpha, B[l + j * ldb], rnd);
            for (int64_t i = 0; i < m; ++i) {
                mpfr_mul(sum, temp, A[i + l * lda], rnd);
                mpfr_add(C[i + j * ldc], C[i + j * ldc], sum, rnd);
            }
        }
        mpfr_clear(sum);
        mpfr_clear(temp);
    }
}

int main(int argc, char **argv) {
    return run_native_rgemm_benchmark(argc, argv, _Rgemm);
}
