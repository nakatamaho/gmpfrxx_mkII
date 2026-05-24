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

constexpr int RgemmBlockSize = 4;

void set_scratch_precision_zero(mpf_class &value, mp_bitcnt_t precision) {
    mpf_set_prec(value.get_mpf_t(), precision);
    value = 0;
}

struct Rgemm4x4Scratch {
    explicit Rgemm4x4Scratch(mp_bitcnt_t precision) {
        set_scratch_precision_zero(zero, precision);
        set_scratch_precision_zero(prod, precision);
        for (int i = 0; i < RgemmBlockSize * RgemmBlockSize; ++i) {
            set_scratch_precision_zero(sink[i], precision);
            set_scratch_precision_zero(acc[i], precision);
        }
        for (int i = 0; i < RgemmBlockSize; ++i) {
            set_scratch_precision_zero(scaled_b[i], precision);
        }
    }

    mpf_class zero;
    mpf_class sink[RgemmBlockSize * RgemmBlockSize];
    mpf_class acc[RgemmBlockSize * RgemmBlockSize];
    mpf_class scaled_b[RgemmBlockSize];
    mpf_class prod;
};

void rgemm_4x4_block(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t j0, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, mpf_class *C, int64_t ldc, Rgemm4x4Scratch &scratch) {
    mpf_class *c_ptr[RgemmBlockSize * RgemmBlockSize];
    const mpf_class *a_ptr[RgemmBlockSize];
    const mpf_class *b_ptr[RgemmBlockSize];

    for (int jj = 0; jj < RgemmBlockSize; ++jj) {
        const int64_t j = j0 + jj;
        const bool valid_col = j < n;
        for (int ii = 0; ii < RgemmBlockSize; ++ii) {
            const int64_t i = i0 + ii;
            const int idx = ii + jj * RgemmBlockSize;
            if (i < m && valid_col) {
                c_ptr[idx] = &C[i + j * ldc];
            } else {
                scratch.sink[idx] = 0;
                c_ptr[idx] = &scratch.sink[idx];
            }
            scratch.acc[idx] = 0;
        }
    }

    for (int64_t l = 0; l < k; ++l) {
        for (int jj = 0; jj < RgemmBlockSize; ++jj) {
            const int64_t j = j0 + jj;
            b_ptr[jj] = (j < n) ? &B[l + j * ldb] : &scratch.zero;
            scratch.scaled_b[jj] = alpha;
            scratch.scaled_b[jj] *= *b_ptr[jj];
        }

        for (int ii = 0; ii < RgemmBlockSize; ++ii) {
            const int64_t i = i0 + ii;
            a_ptr[ii] = (i < m) ? &A[i + l * lda] : &scratch.zero;
        }

        for (int jj = 0; jj < RgemmBlockSize; ++jj) {
            for (int ii = 0; ii < RgemmBlockSize; ++ii) {
                const int idx = ii + jj * RgemmBlockSize;
                scratch.prod = scratch.scaled_b[jj];
                scratch.prod *= *a_ptr[ii];
                scratch.acc[idx] += scratch.prod;
            }
        }
    }

    for (int idx = 0; idx < RgemmBlockSize * RgemmBlockSize; ++idx) {
        *c_ptr[idx] += scratch.acc[idx];
    }
}

} // namespace

// Reference implementation using mpf_class for C = alpha * A * B + beta * C
void _Rgemm(int64_t m, int64_t k, int64_t n, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, const mpf_class &beta, mpf_class *C, int64_t ldc) {
#pragma omp parallel
    {
        Rgemm4x4Scratch scratch(alpha.get_prec());

#pragma omp for collapse(2) schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                C[i + j * ldc] *= beta;
            }
        }

#pragma omp for collapse(2) schedule(static)
        for (int64_t j = 0; j < n; j += RgemmBlockSize) {
            for (int64_t i = 0; i < m; i += RgemmBlockSize) {
                rgemm_4x4_block(m, k, n, i, j, alpha, A, lda, B, ldb, C, ldc, scratch);
            }
        }
    }
}

int main(int argc, char **argv) {
    return rgemm_gmp_bench::run_class_rgemm_benchmark(argc, argv, _Rgemm);
}
