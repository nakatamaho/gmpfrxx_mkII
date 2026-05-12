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

#include <iostream>
#include <chrono>
#include <cstdlib>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Rgemm.hpp" // Ensure you have this header implemented

#define MFLOPS 1e+6

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

void scale_c(int64_t m, int64_t n, const mpf_class &beta, mpf_class *C, int64_t ldc) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] *= beta;
        }
    }
}

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
    scale_c(m, n, beta, C, ldc);
    Rgemm4x4Scratch scratch(alpha.get_prec());

    for (int64_t j = 0; j < n; j += RgemmBlockSize) {
        for (int64_t i = 0; i < m; i += RgemmBlockSize) {
            rgemm_4x4_block(m, k, n, i, j, alpha, A, lda, B, ldb, C, ldc, scratch);
        }
    }
}

// cf. https://netlib.org/lapack/lawnspdf/lawn41.pdf p.120
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
    // Initialize random state
    gmp_randclass r(gmp_randinit_default);
    r.seed(42);

    // Check command-line arguments
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t M = std::atoll(argv[1]); // Number of rows in A and C
    int64_t K = std::atoll(argv[2]); // Number of columns in A and rows in B
    int64_t N = std::atoll(argv[3]); // Number of columns in B and C
    int prec = std::atoi(argv[4]);   // Precision in bits
    mpf_set_default_prec(prec);
#if !defined(USE_ORIGINAL_GMPXX)
    gmpxx::set_default_mpf_precision_bits(prec);
#endif

    // Allocate memory for A (M x K), B (K x N), C (M x N), and reference C (C_ref)
    mpf_class *A = new mpf_class[M * K];
    mpf_class *B = new mpf_class[K * N];
    mpf_class *C = new mpf_class[M * N];
    mpf_class *C_ref = new mpf_class[M * N];

    // Initialize scalars alpha and beta with random values
    mpf_class alpha = r.get_f(prec);
    mpf_class beta = r.get_f(prec);

    // Initialize matrix A with random values
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < K; ++j) {
            A[i + j * M] = r.get_f(prec); // Column-major order
        }
    }

    // Initialize matrix B with random values
    for (int64_t i = 0; i < K; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            B[i + j * K] = r.get_f(prec); // Column-major order
        }
    }

    // Initialize matrix C with random values and copy to C_ref for reference
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            C[i + j * M] = r.get_f(prec);    // Column-major order
            C_ref[i + j * M] = C[i + j * M]; // Copy for reference
        }
    }

    // Perform _Rgemm
    auto start = std::chrono::high_resolution_clock::now();
    _Rgemm(M, K, N, alpha, A, M, B, K, beta, C, M);
    auto end = std::chrono::high_resolution_clock::now();

    // Perform reference computation using Rgemm
    Rgemm("n", "n", M, N, K, alpha, A, M, B, K, beta, C_ref, M);

    // Calculate elapsed time for _Rgemm
    std::chrono::duration<double> elapsed = end - start;
    // For matrix-matrix multiply, number of floating-point operations is 2 * M * N * K
    double mflops = flops_gemm(M, N, K) / (elapsed.count() * MFLOPS);

    // Output performance metrics
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    // Compute L1 norm of the difference between C and C_ref
    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < M; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            mpf_class diff = abs(C[i + j * M] - C_ref[i + j * M]);
            l1_norm += diff;
        }
    }

    // Output L1 norm
    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fg\n", l1_norm.get_mpf_t());

    // Verify correctness
    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    // Clean up
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_ref;

    return EXIT_SUCCESS;
}
