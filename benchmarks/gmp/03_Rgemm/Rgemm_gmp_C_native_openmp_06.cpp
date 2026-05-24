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

namespace {

constexpr int RgemmPanelSize = 4;

struct RgemmPanelScratch {
    mpf_t scaled_b[RgemmPanelSize];
    mpf_t prod;
};

void scratch_init(RgemmPanelScratch &scratch, mp_bitcnt_t precision) {
    mpf_init2(scratch.prod, precision);
    for (int i = 0; i < RgemmPanelSize; ++i) {
        mpf_init2(scratch.scaled_b[i], precision);
    }
}

void scratch_clear(RgemmPanelScratch &scratch) {
    mpf_clear(scratch.prod);
    for (int i = 0; i < RgemmPanelSize; ++i) {
        mpf_clear(scratch.scaled_b[i]);
    }
}

int64_t panel_width(int64_t n, int64_t j0) {
    const int64_t remaining = n - j0;
    return remaining < RgemmPanelSize ? remaining : RgemmPanelSize;
}

void rgemm_panel_rows(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t row_block, int64_t j0, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, mpf_t *C, int64_t ldc, RgemmPanelScratch &scratch) {
    const int64_t jb = panel_width(n, j0);
    const int64_t i_end = std::min(i0 + row_block, m);

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            mpf_mul(scratch.scaled_b[jj], alpha, B[l + (j0 + jj) * ldb]);
        }

        for (int64_t i = i0; i < i_end; ++i) {
            const mpf_t &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                mpf_mul(scratch.prod, scratch.scaled_b[jj], a);
                mpf_add(C[i + (j0 + jj) * ldc], C[i + (j0 + jj) * ldc], scratch.prod);
            }
        }
    }
}

void rgemm_compute(int64_t m, int64_t k, int64_t n, int64_t row_block, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, const mpf_t beta, mpf_t *C, int64_t ldc) {
    const mp_bitcnt_t scratch_precision = mpf_get_prec(alpha);

#pragma omp parallel
    {
        RgemmPanelScratch scratch;
        scratch_init(scratch, scratch_precision);

#pragma omp for collapse(2) schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                mpf_mul(C[i + j * ldc], beta, C[i + j * ldc]);
            }
        }

        const int64_t panel_count = (n + RgemmPanelSize - 1) / RgemmPanelSize;
        const int64_t row_block_count = (m + row_block - 1) / row_block;
#pragma omp for collapse(2) schedule(static)
        for (int64_t panel_index = 0; panel_index < panel_count; ++panel_index) {
            for (int64_t row_block_index = 0; row_block_index < row_block_count; ++row_block_index) {
                const int64_t j0 = panel_index * RgemmPanelSize;
                const int64_t i0 = row_block_index * row_block;
                rgemm_panel_rows(m, k, n, i0, row_block, j0, alpha, A, lda, B, ldb, C, ldc, scratch);
            }
        }

        scratch_clear(scratch);
    }
}


} // namespace


void _Rgemm(int64_t m, int64_t k, int64_t n, int64_t row_block, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, const mpf_t beta, mpf_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }

    rgemm_compute(m, k, n, row_block, alpha, A, lda, B, ldb, beta, C, ldc);
}

int main(int argc, char **argv) {
    return rgemm_gmp_bench::run_native_rgemm_row_block_benchmark(argc, argv, _Rgemm);
}
