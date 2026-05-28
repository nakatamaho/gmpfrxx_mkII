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
    mpfr_t zero;
    mpfr_t sink[RgemmBlockSize * RgemmBlockSize];
    mpfr_t acc[RgemmBlockSize * RgemmBlockSize];
    mpfr_t scaled_b[RgemmBlockSize];
};

void scratch_init(Rgemm4x4Scratch &scratch, mpfr_prec_t precision, mpfr_rnd_t rnd) {
    mpfr_init2(scratch.zero, precision);
    mpfr_set_ui(scratch.zero, 0, rnd);
    for (int i = 0; i < RgemmBlockSize * RgemmBlockSize; ++i) {
        mpfr_init2(scratch.sink[i], precision);
        mpfr_init2(scratch.acc[i], precision);
    }
    for (int i = 0; i < RgemmBlockSize; ++i) {
        mpfr_init2(scratch.scaled_b[i], precision);
    }
}

void scratch_clear(Rgemm4x4Scratch &scratch) {
    mpfr_clear(scratch.zero);
    for (int i = 0; i < RgemmBlockSize * RgemmBlockSize; ++i) {
        mpfr_clear(scratch.sink[i]);
        mpfr_clear(scratch.acc[i]);
    }
    for (int i = 0; i < RgemmBlockSize; ++i) {
        mpfr_clear(scratch.scaled_b[i]);
    }
}

void scale_c(int64_t m, int64_t n, const mpfr_t beta, mpfr_t *C, int64_t ldc, mpfr_rnd_t rnd) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpfr_mul(C[i + j * ldc], beta, C[i + j * ldc], rnd);
        }
    }
}

void rgemm_4x4_block(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t j0, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, mpfr_t *C, int64_t ldc, Rgemm4x4Scratch &scratch, mpfr_rnd_t rnd) {
    mpfr_ptr c_ptr[RgemmBlockSize * RgemmBlockSize];
    mpfr_srcptr a_ptr[RgemmBlockSize];
    mpfr_srcptr b_ptr[RgemmBlockSize];

    for (int jj = 0; jj < RgemmBlockSize; ++jj) {
        const int64_t j = j0 + jj;
        const bool valid_col = j < n;
        for (int ii = 0; ii < RgemmBlockSize; ++ii) {
            const int64_t i = i0 + ii;
            const int idx = ii + jj * RgemmBlockSize;
            if (i < m && valid_col) {
                c_ptr[idx] = C[i + j * ldc];
            } else {
                mpfr_set_ui(scratch.sink[idx], 0, rnd);
                c_ptr[idx] = scratch.sink[idx];
            }
            mpfr_set_ui(scratch.acc[idx], 0, rnd);
        }
    }

    for (int64_t l = 0; l < k; ++l) {
        for (int jj = 0; jj < RgemmBlockSize; ++jj) {
            const int64_t j = j0 + jj;
            b_ptr[jj] = (j < n) ? B[l + j * ldb] : scratch.zero;
            mpfr_mul(scratch.scaled_b[jj], alpha, b_ptr[jj], rnd);
        }

        for (int ii = 0; ii < RgemmBlockSize; ++ii) {
            const int64_t i = i0 + ii;
            a_ptr[ii] = (i < m) ? A[i + l * lda] : scratch.zero;
        }

        for (int jj = 0; jj < RgemmBlockSize; ++jj) {
            for (int ii = 0; ii < RgemmBlockSize; ++ii) {
                const int idx = ii + jj * RgemmBlockSize;
                mpfr_fma(scratch.acc[idx], scratch.scaled_b[jj], a_ptr[ii], scratch.acc[idx], rnd);
            }
        }
    }

    for (int idx = 0; idx < RgemmBlockSize * RgemmBlockSize; ++idx) {
        mpfr_add(c_ptr[idx], c_ptr[idx], scratch.acc[idx], rnd);
    }
}

} // namespace


// Reference implementation using mpfr_t for C = alpha * A * B + beta * C
void _Rgemm(int64_t m, int64_t k, int64_t n, const mpfr_t alpha, const mpfr_t *A, int64_t lda, const mpfr_t *B, int64_t ldb, const mpfr_t beta, mpfr_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    scale_c(m, n, beta, C, ldc, rnd);

    Rgemm4x4Scratch scratch;
    scratch_init(scratch, mpfr_get_prec(alpha), rnd);
    for (int64_t j = 0; j < n; j += RgemmBlockSize) {
        for (int64_t i = 0; i < m; i += RgemmBlockSize) {
            rgemm_4x4_block(m, k, n, i, j, alpha, A, lda, B, ldb, C, ldc, scratch, rnd);
        }
    }
    scratch_clear(scratch);
}

int main(int argc, char **argv) {
    return run_native_rgemm_benchmark(argc, argv, _Rgemm);
}
