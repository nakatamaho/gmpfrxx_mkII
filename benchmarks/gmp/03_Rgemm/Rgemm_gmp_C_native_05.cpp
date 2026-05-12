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
#include <gmp.h>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Rgemm.hpp"

#define MFLOPS 1e+6

gmp_randstate_t state;

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

void scale_c(int64_t m, int64_t n, const mpf_t beta, mpf_t *C, int64_t ldc) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpf_mul(C[i + j * ldc], beta, C[i + j * ldc]);
        }
    }
}

void rgemm_panel(int64_t m, int64_t k, int64_t n, int64_t j0, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, mpf_t *C, int64_t ldc, RgemmPanelScratch &scratch) {
    const int64_t jb = panel_width(n, j0);

    for (int64_t l = 0; l < k; ++l) {
        for (int64_t jj = 0; jj < jb; ++jj) {
            mpf_mul(scratch.scaled_b[jj], alpha, B[l + (j0 + jj) * ldb]);
        }

        for (int64_t i = 0; i < m; ++i) {
            const mpf_t &a = A[i + l * lda];
            for (int64_t jj = 0; jj < jb; ++jj) {
                mpf_mul(scratch.prod, scratch.scaled_b[jj], a);
                mpf_add(C[i + (j0 + jj) * ldc], C[i + (j0 + jj) * ldc], scratch.prod);
            }
        }
    }
}

} // namespace

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

void _Rgemm(int64_t m, int64_t k, int64_t n, const mpf_t alpha, const mpf_t *A, int64_t lda, const mpf_t *B, int64_t ldb, const mpf_t beta, mpf_t *C, int64_t ldc) {
    if (lda < m || ldb < k || ldc < m) {
        std::cerr << "Leading dimensions are too small." << std::endl;
        exit(EXIT_FAILURE);
    }

    scale_c(m, n, beta, C, ldc);

    RgemmPanelScratch scratch;
    scratch_init(scratch, mpf_get_prec(alpha));
    for (int64_t j = 0; j < n; j += RgemmPanelSize) {
        rgemm_panel(m, k, n, j, alpha, A, lda, B, ldb, C, ldc, scratch);
    }
    scratch_clear(scratch);
}

void init_mpf_mat(mpf_t *mat, int64_t rows, int64_t cols, int64_t ld, int prec) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpf_init2(mat[i + j * ld], prec);
            mpf_urandomb(mat[i + j * ld], state, prec);
        }
    }
}

void clear_mpf_mat(mpf_t *mat, int64_t rows, int64_t cols, int64_t ld) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpf_clear(mat[i + j * ld]);
        }
    }
}

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    int64_t m = std::atoll(argv[1]);
    int64_t k = std::atoll(argv[2]);
    int64_t n = std::atoll(argv[3]);
    int prec = std::atoi(argv[4]);
    mpf_set_default_prec(prec);
#if !defined(USE_ORIGINAL_GMPXX)
    gmpxx::set_default_mpf_precision_bits(prec);
#endif

    int64_t lda = m;
    int64_t ldb = k;
    int64_t ldc = m;

    mpf_t *A = new mpf_t[m * k];
    mpf_t *B = new mpf_t[k * n];
    mpf_t *C = new mpf_t[m * n];

    mpf_t alpha, beta;
    mpf_init2(alpha, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(beta, state, prec);

    init_mpf_mat(A, m, k, lda, prec);
    init_mpf_mat(B, k, n, ldb, prec);
    init_mpf_mat(C, m, n, ldc, prec);

    mpf_class *A_mpf_class = new mpf_class[m * k];
    mpf_class *B_mpf_class = new mpf_class[k * n];
    mpf_class *C_mpf_class = new mpf_class[m * n];
    mpf_class alpha_class = mpf_class(alpha);
    mpf_class beta_class = mpf_class(beta);

    for (int64_t j = 0; j < k; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A_mpf_class[i + j * lda] = mpf_class(A[i + j * lda]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < k; ++i) {
            B_mpf_class[i + j * ldb] = mpf_class(B[i + j * ldb]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C_mpf_class[i + j * ldc] = mpf_class(C[i + j * ldc]);
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    _Rgemm(m, k, n, alpha, A, lda, B, ldb, beta, C, ldc);
    auto end = std::chrono::high_resolution_clock::now();

    Rgemm("n", "n", m, n, k, alpha_class, A_mpf_class, lda, B_mpf_class, ldb, beta_class, C_mpf_class, ldc);

    std::chrono::duration<double> elapsed_seconds = end - start;
    double mflops = flops_gemm(m, n, k) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    mpf_class l1_norm = 0;
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpf_class diff = abs(mpf_class(C[i + j * ldc]) - C_mpf_class[i + j * ldc]);
            l1_norm += diff;
        }
    }

    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fe\n", l1_norm.get_mpf_t());

    mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }

    clear_mpf_mat(A, m, k, lda);
    clear_mpf_mat(B, k, n, ldb);
    clear_mpf_mat(C, m, n, ldc);
    mpf_clear(alpha);
    mpf_clear(beta);
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] A_mpf_class;
    delete[] B_mpf_class;
    delete[] C_mpf_class;

    gmp_randclear(state);

    return EXIT_SUCCESS;
}
