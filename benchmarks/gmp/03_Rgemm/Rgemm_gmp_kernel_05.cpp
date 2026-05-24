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

void set_scratch_precision_zero(mpf_class &value, mp_bitcnt_t precision) {
    mpf_set_prec(value.get_mpf_t(), precision);
    value = 0;
}

struct RgemmPanelScratch {
    explicit RgemmPanelScratch(mp_bitcnt_t precision) {
        set_scratch_precision_zero(prod, precision);
        for (int i = 0; i < RgemmPanelSize; ++i) {
            set_scratch_precision_zero(scaled_b[i], precision);
        }
    }

    mpf_class scaled_b[RgemmPanelSize];
    mpf_class prod;
};

int64_t panel_width(int64_t n, int64_t j0) {
    const int64_t remaining = n - j0;
    return remaining < RgemmPanelSize ? remaining : RgemmPanelSize;
}

void scale_c(int64_t m, int64_t n, const mpf_class &beta, mpf_class *C, int64_t ldc) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] *= beta;
        }
    }
}

void rgemm_panel(int64_t m, int64_t k, int64_t n, int64_t j0, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, mpf_class *C, int64_t ldc, RgemmPanelScratch &scratch) {
    const int64_t jb = panel_width(n, j0);

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            scratch.scaled_b[jj] = alpha;
            scratch.scaled_b[jj] *= B[l + (j0 + jj) * ldb];
        }

        for (int64_t i = 0; i < m; ++i) {
            const mpf_class &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                scratch.prod = scratch.scaled_b[jj];
                scratch.prod *= a;
                C[i + (j0 + jj) * ldc] += scratch.prod;
            }
        }
    }
}

} // namespace

void _Rgemm(int64_t m, int64_t k, int64_t n, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, const mpf_class &beta, mpf_class *C, int64_t ldc) {
    scale_c(m, n, beta, C, ldc);

    RgemmPanelScratch scratch(alpha.get_prec());
    for (int64_t j = 0; j < n; j += RgemmPanelSize) {
        rgemm_panel(m, k, n, j, alpha, A, lda, B, ldb, C, ldc, scratch);
    }
}

int main(int argc, char **argv) {
    return rgemm_gmp_bench::run_class_rgemm_benchmark(argc, argv, _Rgemm);
}
