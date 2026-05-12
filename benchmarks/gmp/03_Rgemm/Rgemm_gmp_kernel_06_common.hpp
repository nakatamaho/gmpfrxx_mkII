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

#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#if defined(RGEMM_GMP_KERNEL_06_USE_OPENMP)
#include <omp.h>
#endif

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Rgemm.hpp"

#define MFLOPS 1e+6

namespace {

constexpr int RgemmPanelSize = 4;
constexpr int64_t FixedRowBlockSize = 256;

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

void rgemm_panel_rows(int64_t m, int64_t k, int64_t n, int64_t i0, int64_t row_block, int64_t j0, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, mpf_class *C, int64_t ldc, RgemmPanelScratch &scratch) {
    const int64_t jb = panel_width(n, j0);
    const int64_t i_end = std::min(i0 + row_block, m);

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            scratch.scaled_b[jj] = alpha;
            scratch.scaled_b[jj] *= B[l + (j0 + jj) * ldb];
        }

        for (int64_t i = i0; i < i_end; ++i) {
            const mpf_class &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                scratch.prod = scratch.scaled_b[jj];
                scratch.prod *= a;
                C[i + (j0 + jj) * ldc] += scratch.prod;
            }
        }
    }
}

void rgemm_compute(int64_t m, int64_t k, int64_t n, int64_t row_block, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, const mpf_class &beta, mpf_class *C, int64_t ldc) {
    const mp_bitcnt_t scratch_precision = alpha.get_prec();

#if defined(RGEMM_GMP_KERNEL_06_USE_OPENMP)
#pragma omp parallel
    {
        RgemmPanelScratch scratch(scratch_precision);

#pragma omp for collapse(2) schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                C[i + j * ldc] *= beta;
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
    }
#else
    scale_c(m, n, beta, C, ldc);

    RgemmPanelScratch scratch(scratch_precision);
    for (int64_t j = 0; j < n; j += RgemmPanelSize) {
        for (int64_t i = 0; i < m; i += row_block) {
            rgemm_panel_rows(m, k, n, i, row_block, j, alpha, A, lda, B, ldb, C, ldc, scratch);
        }
    }
#endif
}

int64_t fixed_row_block(int64_t m) {
    return std::min<int64_t>(FixedRowBlockSize, m);
}

} // namespace

void _Rgemm(int64_t m, int64_t k, int64_t n, int64_t row_block, const mpf_class &alpha, const mpf_class *A, int64_t lda, const mpf_class *B, int64_t ldb, const mpf_class &beta, mpf_class *C, int64_t ldc) {
    rgemm_compute(m, k, n, row_block, alpha, A, lda, B, ldb, beta, C, ldc);
}

double flops_gemm(int k_i, int m_i, int n_i) {
    double adds, muls, flops;
    double k, m, n;
    m = (double)m_i;
    n = (double)n_i;
    k = (double)k_i;
    muls = m * (k + 2) * n;
    adds = m * k * n;
    flops = muls + adds;
    return flops;
}

int main(int argc, char **argv) {
    gmp_randclass r(gmp_randinit_default);
    r.seed(42);

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t M = std::atoll(argv[1]);
    int64_t K = std::atoll(argv[2]);
    int64_t N = std::atoll(argv[3]);
    int prec = std::atoi(argv[4]);
    mpf_set_default_prec(prec);
#if !defined(USE_ORIGINAL_GMPXX)
    gmpxx::set_default_mpf_precision_bits(prec);
#endif

    mpf_class *A = new mpf_class[M * K];
    mpf_class *B = new mpf_class[K * N];
    mpf_class *C = new mpf_class[M * N];
    mpf_class *C_ref = new mpf_class[M * N];

    mpf_class alpha = r.get_f(prec);
    mpf_class beta = r.get_f(prec);

    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < K; ++j) {
            A[i + j * M] = r.get_f(prec);
        }
    }

    for (int64_t i = 0; i < K; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            B[i + j * K] = r.get_f(prec);
        }
    }

    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            C[i + j * M] = r.get_f(prec);
            C_ref[i + j * M] = C[i + j * M];
        }
    }

    const int64_t row_block = fixed_row_block(M);
    std::cout << "Fixed row block size: " << row_block << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    _Rgemm(M, K, N, row_block, alpha, A, M, B, K, beta, C, M);
    auto end = std::chrono::high_resolution_clock::now();

    Rgemm("n", "n", M, N, K, alpha, A, M, B, K, beta, C_ref, M);

    std::chrono::duration<double> elapsed = end - start;
    double mflops = flops_gemm(M, N, K) / (elapsed.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            mpf_class diff = abs(C[i + j * M] - C_ref[i + j * M]);
            l1_norm += diff;
        }
    }

    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fg\n", l1_norm.get_mpf_t());

    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_ref;

    return EXIT_SUCCESS;
}
