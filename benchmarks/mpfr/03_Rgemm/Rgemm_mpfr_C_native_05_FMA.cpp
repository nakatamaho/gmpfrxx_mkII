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

void scale_c(int64_t m, int64_t n, const mpfr_t beta, mpfr_t *C, int64_t ldc, mpfr_rnd_t rnd) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpfr_mul(C[i + j * ldc], beta, C[i + j * ldc], rnd);
        }
    }
}

void rgemm_panel(int64_t m, int64_t k, int64_t n, int64_t j0, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, const mpfr_t beta, mpfr_t *C, int64_t ldc, RgemmPanelScratch &scratch, mpfr_rnd_t rnd) {
    const int64_t jb = panel_width(n, j0);

    for (int64_t i = 0; i < m; ++i) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            mpfr_mul(C[i + (j0 + jj) * ldc], beta, C[i + (j0 + jj) * ldc], rnd);
        }
    }

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            mpfr_mul(scratch.scaled_b[jj], alpha, B[l + (j0 + jj) * ldb], rnd);
        }

        for (int64_t i = 0; i < m; ++i) {
            const mpfr_t &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                mpfr_fma(C[i + (j0 + jj) * ldc], scratch.scaled_b[jj], a, C[i + (j0 + jj) * ldc], rnd);
            }
        }
    }
}

} // namespace


void _Rgemm(int64_t m, int64_t k, int64_t n, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, const mpfr_t beta, mpfr_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    RgemmPanelScratch scratch;
    scratch_init(scratch, mpfr_get_prec(alpha), rnd);
    for (int64_t j = 0; j < n; j += RgemmPanelSize) {
        rgemm_panel(m, k, n, j, alpha, A, lda, B, ldb, beta, C, ldc, scratch, rnd);
    }
    scratch_clear(scratch);
}

int main(int argc, char **argv) {
    return run_native_rgemm_benchmark(argc, argv, _Rgemm);
}
