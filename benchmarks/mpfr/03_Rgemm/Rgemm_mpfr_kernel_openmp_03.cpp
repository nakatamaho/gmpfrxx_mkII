/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <omp.h>

#include "Rgemm_common.hpp"


void _Rgemm(int64_t m, int64_t k, int64_t n, const mpfr_class &alpha, const mpfr_class *A, int64_t lda, const mpfr_class *B, int64_t ldb, const mpfr_class &beta, mpfr_class *C, int64_t ldc) {
#pragma omp parallel for collapse(2)
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] *= beta;
        }
    }

#pragma omp parallel for
    for (int64_t j = 0; j < n; ++j) {
        mpfr_class temp, templ;
        for (int64_t l = 0; l < k; ++l) {
            temp = alpha;
            temp *= B[l + j * ldb];
            for (int64_t i = 0; i < m; ++i) {
                templ = temp;
                templ *= A[i + l * lda];
                C[i + j * ldc] += templ;
            }
        }
    }
}

int main(int argc, char **argv) {
    return run_class_rgemm_benchmark(argc, argv, _Rgemm);
}
