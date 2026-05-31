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

void set_scratch_precision_zero(mpfr_class &value, mpfr_prec_t precision) {
    value.set_prec(precision);
    value = 0;
}

struct RgemmPanelScratch {
    explicit RgemmPanelScratch(mpfr_prec_t precision) {
        for (int i = 0; i < RgemmPanelSize; ++i) {
            set_scratch_precision_zero(scaled_b[i], precision);
        }
    }

    mpfr_class scaled_b[RgemmPanelSize];
};

int64_t panel_width(int64_t n, int64_t j0) {
    const int64_t remaining = n - j0;
    return remaining < RgemmPanelSize ? remaining : RgemmPanelSize;
}

void scale_c(int64_t m, int64_t n, const mpfr_class &beta, mpfr_class *C, int64_t ldc, mpfr_rnd_t rounding) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            {
            auto c_rounding = mpfrxx::with_rounding(C[i + j * ldc], rounding);
            c_rounding *= beta;
        }
        }
    }
}

void rgemm_panel_rows(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t row_block, int64_t j0, const mpfr_class &alpha, const mpfr_class *A, int64_t lda, const mpfr_class *B, int64_t ldb, const mpfr_class &beta, mpfr_class *C, int64_t ldc, RgemmPanelScratch &scratch, mpfr_rnd_t rounding) {
    const int64_t jb = panel_width(n, j0);
    const int64_t i_end = (i0 + row_block < m) ? (i0 + row_block) : m;

    for (int64_t i = i0; i < i_end; ++i) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            auto c_rounding = mpfrxx::with_rounding(C[i + (j0 + jj) * ldc], rounding);
            c_rounding *= beta;
        }
    }

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            {
            auto scaled_b_rounding = mpfrxx::with_rounding(scratch.scaled_b[jj], rounding);
            scaled_b_rounding = alpha * B[l + (j0 + jj) * ldb];
        }
        }

        for (int64_t i = i0; i < i_end; ++i) {
            const mpfr_class &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                {
                    auto c_rounding = mpfrxx::with_rounding(C[i + (j0 + jj) * ldc], rounding);
                    c_rounding += scratch.scaled_b[jj] * a;
                }
            }
        }
    }
}

void rgemm_compute(int64_t m, int64_t k, int64_t n, int64_t row_block, const mpfr_class &alpha, const mpfr_class *A, int64_t lda, const mpfr_class *B, int64_t ldb, const mpfr_class &beta, mpfr_class *C, int64_t ldc, mpfr_rnd_t rounding) {
    const mpfr_prec_t scratch_precision = alpha.get_prec();

    RgemmPanelScratch scratch(scratch_precision);
    for (int64_t j = 0; j < n; j += RgemmPanelSize) {
        for (int64_t i = 0; i < m; i += row_block) {
            rgemm_panel_rows(m, k, n, i, row_block, j, alpha, A, lda, B, ldb, beta, C, ldc, scratch, rounding);
        }
    }
}


} // namespace

void _Rgemm(int64_t m, int64_t k, int64_t n, const mpfr_class &alpha, const mpfr_class *A, int64_t lda, const mpfr_class *B, int64_t ldb, const mpfr_class &beta, mpfr_class *C, int64_t ldc) {
    const mpfr_rnd_t rounding = mpfrxx::default_rounding_mode();
    const int64_t row_block = m < 256 ? m : 256;
    rgemm_compute(m, k, n, row_block, alpha, A, lda, B, ldb, beta, C, ldc, rounding);
}

int main(int argc, char **argv) {
    return run_class_rgemm_benchmark(argc, argv, _Rgemm);
}
