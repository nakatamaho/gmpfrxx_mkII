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

struct Rgemm4x4Scratch {
    mpf_t zero;
    mpf_t sink[RgemmBlockSize * RgemmBlockSize];
    mpf_t acc[RgemmBlockSize * RgemmBlockSize];
    mpf_t scaled_b[RgemmBlockSize];
    mpf_t prod;
};

void scratch_init(Rgemm4x4Scratch &scratch, mp_bitcnt_t precision) {
    mpf_init2(scratch.zero, precision);
    mpf_set_ui(scratch.zero, 0);
    mpf_init2(scratch.prod, precision);
    for (int i = 0; i < RgemmBlockSize * RgemmBlockSize; ++i) {
        mpf_init2(scratch.sink[i], precision);
        mpf_init2(scratch.acc[i], precision);
    }
    for (int i = 0; i < RgemmBlockSize; ++i) {
        mpf_init2(scratch.scaled_b[i], precision);
    }
}

void scratch_clear(Rgemm4x4Scratch &scratch) {
    mpf_clear(scratch.zero);
    mpf_clear(scratch.prod);
    for (int i = 0; i < RgemmBlockSize * RgemmBlockSize; ++i) {
        mpf_clear(scratch.sink[i]);
        mpf_clear(scratch.acc[i]);
    }
    for (int i = 0; i < RgemmBlockSize; ++i) {
        mpf_clear(scratch.scaled_b[i]);
    }
}

void rgemm_4x4_block(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t j0, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, const mpf_t beta, mpf_t *C, int64_t ldc, Rgemm4x4Scratch &scratch) {
    mpf_ptr c_ptr[RgemmBlockSize * RgemmBlockSize];
    mpf_srcptr a_ptr[RgemmBlockSize];
    mpf_srcptr b_ptr[RgemmBlockSize];

    for (int jj = 0; jj < RgemmBlockSize; ++jj) {
        const int64_t j = j0 + jj;
        const bool valid_col = j < n;
        for (int ii = 0; ii < RgemmBlockSize; ++ii) {
            const int64_t i = i0 + ii;
            const int idx = ii + jj * RgemmBlockSize;
            if (i < m && valid_col) {
                c_ptr[idx] = C[i + j * ldc];
                mpf_mul(scratch.acc[idx], beta, c_ptr[idx]);
            } else {
                mpf_set_ui(scratch.sink[idx], 0);
                c_ptr[idx] = scratch.sink[idx];
                mpf_set_ui(scratch.acc[idx], 0);
            }
        }
    }

    for (int64_t l = 0; l < k; ++l) {
        for (int jj = 0; jj < RgemmBlockSize; ++jj) {
            const int64_t j = j0 + jj;
            b_ptr[jj] = (j < n) ? B[l + j * ldb] : scratch.zero;
            mpf_mul(scratch.scaled_b[jj], alpha, b_ptr[jj]);
        }

        for (int ii = 0; ii < RgemmBlockSize; ++ii) {
            const int64_t i = i0 + ii;
            a_ptr[ii] = (i < m) ? A[i + l * lda] : scratch.zero;
        }

        for (int jj = 0; jj < RgemmBlockSize; ++jj) {
            for (int ii = 0; ii < RgemmBlockSize; ++ii) {
                const int idx = ii + jj * RgemmBlockSize;
                mpf_mul(scratch.prod, scratch.scaled_b[jj], a_ptr[ii]);
                mpf_add(scratch.acc[idx], scratch.acc[idx], scratch.prod);
            }
        }
    }

    for (int idx = 0; idx < RgemmBlockSize * RgemmBlockSize; ++idx) {
        mpf_set(c_ptr[idx], scratch.acc[idx]);
    }
}

} // namespace


// Reference implementation using mpf_t for C = alpha * A * B + beta * C
void _Rgemm(int64_t m, int64_t k, int64_t n, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, const mpf_t beta, mpf_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mp_bitcnt_t scratch_precision = mpf_get_prec(alpha);
#pragma omp parallel
    {
        Rgemm4x4Scratch scratch;
        scratch_init(scratch, scratch_precision);

#pragma omp for collapse(2) schedule(static)
        for (int64_t j = 0; j < n; j += RgemmBlockSize) {
            for (int64_t i = 0; i < m; i += RgemmBlockSize) {
                rgemm_4x4_block(m, k, n, i, j, alpha, A, lda, B, ldb, beta, C, ldc, scratch);
            }
        }

        scratch_clear(scratch);
    }
}

int main(int argc, char **argv) {
    return rgemm_gmp_bench::run_native_rgemm_benchmark(argc, argv, _Rgemm);
}
