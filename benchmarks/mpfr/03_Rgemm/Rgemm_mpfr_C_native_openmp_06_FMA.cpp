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
    mpfr_t scaled_b[RgemmPanelSize];
};

void scratch_init(RgemmPanelScratch &scratch, mpfr_prec_t precision, mpfr_rnd_t rnd) {
    for (int i = 0; i < RgemmPanelSize; ++i) {
        mpfr_init2(scratch.scaled_b[i], precision);
    }
}

void scratch_clear(RgemmPanelScratch &scratch) {
    for (int i = 0; i < RgemmPanelSize; ++i) {
        mpfr_clear(scratch.scaled_b[i]);
    }
}

int64_t panel_width(int64_t n, int64_t j0) {
    const int64_t remaining = n - j0;
    return remaining < RgemmPanelSize ? remaining : RgemmPanelSize;
}

void rgemm_panel_rows(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t row_block, int64_t j0, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, mpfr_t *C, int64_t ldc, RgemmPanelScratch &scratch, mpfr_rnd_t rnd) {
    const int64_t jb = panel_width(n, j0);
    const int64_t i_end = (i0 + row_block < m) ? (i0 + row_block) : m;

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            mpfr_mul(scratch.scaled_b[jj], alpha, B[l + (j0 + jj) * ldb], rnd);
        }

        for (int64_t i = i0; i < i_end; ++i) {
            const mpfr_t &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                mpfr_fma(C[i + (j0 + jj) * ldc], scratch.scaled_b[jj], a, C[i + (j0 + jj) * ldc], rnd);
            }
        }
    }
}

void rgemm_compute(int64_t m, int64_t k, int64_t n, int64_t row_block, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, const mpfr_t beta, mpfr_t *C, int64_t ldc, mpfr_rnd_t rnd) {
    const mpfr_prec_t scratch_precision = mpfr_get_prec(alpha);

#pragma omp parallel
    {
        RgemmPanelScratch scratch;
        scratch_init(scratch, scratch_precision, rnd);

#pragma omp for collapse(2) schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                mpfr_mul(C[i + j * ldc], beta, C[i + j * ldc], rnd);
            }
        }

        const int64_t panel_count = (n + RgemmPanelSize - 1) / RgemmPanelSize;
        const int64_t row_block_count = (m + row_block - 1) / row_block;
#pragma omp for collapse(2) schedule(static)
        for (int64_t panel_index = 0; panel_index < panel_count; ++panel_index) {
            for (int64_t row_block_index = 0; row_block_index < row_block_count; ++row_block_index) {
                const int64_t j0 = panel_index * RgemmPanelSize;
                const int64_t i0 = row_block_index * row_block;
                rgemm_panel_rows(m, k, n, i0, row_block, j0, alpha, A, lda, B, ldb, C, ldc, scratch, rnd);
            }
        }

        scratch_clear(scratch);
    }
}


} // namespace


void _Rgemm(int64_t m, int64_t k, int64_t n, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, const mpfr_t beta, mpfr_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }

    const int64_t row_block = m < 256 ? m : 256;
    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    rgemm_compute(m, k, n, row_block, alpha, A, lda, B, ldb, beta, C, ldc, rnd);
}

int main(int argc, char **argv) {
    return run_native_rgemm_benchmark(argc, argv, _Rgemm);
}
