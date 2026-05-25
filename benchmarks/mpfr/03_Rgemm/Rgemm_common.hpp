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
 */

#ifndef GMPFRXX_MKII_BENCHMARK_MPFR_RGEMM_COMMON_HPP
#define GMPFRXX_MKII_BENCHMARK_MPFR_RGEMM_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <gmp.h>

#include "mpfr_operation_counter.hpp"

#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Rgemm.hpp"

#define MFLOPS 1e+6


inline double flops_gemm(int64_t m_i, int64_t n_i, int64_t k_i) {
    const double m = static_cast<double>(m_i);
    const double n = static_cast<double>(n_i);
    const double k = static_cast<double>(k_i);
    return m * (k + 2.0) * n + m * k * n;
}

inline void configure_mpfr_precision(int prec) {
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);
}

inline void require_mpfr_precision_at_least(const char *label, mpfr_prec_t actual, mpfr_prec_t requested) {
    if (actual >= requested) {
        return;
    }
    std::cerr << "Precision check failed for " << label << ": requested at least " << requested
              << " bits, actual " << actual << " bits" << std::endl;
    std::exit(EXIT_FAILURE);
}

inline mpfr_prec_t class_precision_bits(const mpfr_class &value) {
    return mpfr_get_prec(value.get_mpfr_t());
}

inline void init_mpfr_mat(mpfr_t *mat, int64_t rows, int64_t cols, int64_t ld, int prec, gmp_randstate_t state) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpfr_init2(mat[i + j * ld], prec);
            mpfr_urandomb(mat[i + j * ld], state);
        }
    }
}

inline void clear_mpfr_mat(mpfr_t *mat, int64_t rows, int64_t cols, int64_t ld) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpfr_clear(mat[i + j * ld]);
        }
    }
}

inline mpfr_class l1_norm_difference(const mpfr_t *raw, const mpfr_class *wrapped, int64_t rows, int64_t cols, int64_t ld) {
    mpfr_class l1_norm = 0;
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            l1_norm += abs(mpfr_class(raw[i + j * ld]) - wrapped[i + j * ld]);
        }
    }
    return l1_norm;
}

inline mpfr_class l1_norm_difference(const mpfr_class *lhs, const mpfr_class *rhs, int64_t rows, int64_t cols, int64_t ld) {
    mpfr_class l1_norm = 0;
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            l1_norm += abs(lhs[i + j * ld] - rhs[i + j * ld]);
        }
    }
    return l1_norm;
}

inline void print_l1_result(const mpfr_class &l1_norm) {
    std::cout << "L1 Norm of difference: ";
    mpfr_printf("%.4Rg\n", l1_norm.get_mpfr_t());

    if (l1_norm < 1e-5) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }
}

inline int run_native_rgemm_benchmark(int argc, char **argv,
                                      void (*kernel)(int64_t, int64_t, int64_t, const mpfr_t, const mpfr_t *, int64_t, const mpfr_t *, int64_t, const mpfr_t, mpfr_t *, int64_t)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t k = std::atoll(argv[2]);
    const int64_t n = std::atoll(argv[3]);
    const int prec = std::atoi(argv[4]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mpfr_prec_t requested_prec = static_cast<mpfr_prec_t>(prec);
    configure_mpfr_precision(prec);
    require_mpfr_precision_at_least("default_mpfr_precision", mpfr_get_default_prec(), requested_prec);

    const int64_t lda = m;
    const int64_t ldb = k;
    const int64_t ldc = m;
    mpfr_t *A = new mpfr_t[m * k];
    mpfr_t *B = new mpfr_t[k * n];
    mpfr_t *C = new mpfr_t[m * n];
    mpfr_t alpha, beta;
    mpfr_init2(alpha, prec);
    mpfr_urandomb(alpha, state);
    mpfr_init2(beta, prec);
    mpfr_urandomb(beta, state);
    init_mpfr_mat(A, m, k, lda, prec, state);
    init_mpfr_mat(B, k, n, ldb, prec, state);
    init_mpfr_mat(C, m, n, ldc, prec, state);

    mpfr_class *A_class = new mpfr_class[m * k];
    mpfr_class *B_class = new mpfr_class[k * n];
    mpfr_class *C_class = new mpfr_class[m * n];
    mpfr_class alpha_class = mpfr_class(alpha);
    mpfr_class beta_class = mpfr_class(beta);
    for (int64_t j = 0; j < k; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A_class[i + j * lda] = mpfr_class(A[i + j * lda]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < k; ++i) {
            B_class[i + j * ldb] = mpfr_class(B[i + j * ldb]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C_class[i + j * ldc] = mpfr_class(C[i + j * ldc]);
        }
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(m, k, n, alpha, A, lda, B, ldb, beta, C, ldc);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Rgemm("n", "n", m, n, k, alpha_class, A_class, lda, B_class, ldb, beta_class, C_class, ldc);

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = flops_gemm(m, n, k) / (elapsed_seconds.count() * MFLOPS);
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(C, C_class, m, n, ldc));

    clear_mpfr_mat(A, m, k, lda);
    clear_mpfr_mat(B, k, n, ldb);
    clear_mpfr_mat(C, m, n, ldc);
    mpfr_clear(alpha);
    mpfr_clear(beta);
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] A_class;
    delete[] B_class;
    delete[] C_class;
    gmp_randclear(state);
    return EXIT_SUCCESS;
}

inline int run_class_rgemm_benchmark(int argc, char **argv,
                                     void (*kernel)(int64_t, int64_t, int64_t, const mpfr_class &, const mpfr_class *, int64_t, const mpfr_class *, int64_t, const mpfr_class &, mpfr_class *, int64_t)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols k> <cols n> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t k = std::atoll(argv[2]);
    const int64_t n = std::atoll(argv[3]);
    const int prec = std::atoi(argv[4]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mpfr_prec_t requested_prec = static_cast<mpfr_prec_t>(prec);
    configure_mpfr_precision(prec);
    require_mpfr_precision_at_least("default_mpfr_precision", mpfr_get_default_prec(), requested_prec);

    const int64_t lda = m;
    const int64_t ldb = k;
    const int64_t ldc = m;
    mpfr_t *A_raw = new mpfr_t[m * k];
    mpfr_t *B_raw = new mpfr_t[k * n];
    mpfr_t *C_raw = new mpfr_t[m * n];
    mpfr_t alpha_raw, beta_raw;
    mpfr_init2(alpha_raw, prec);
    mpfr_urandomb(alpha_raw, state);
    mpfr_init2(beta_raw, prec);
    mpfr_urandomb(beta_raw, state);
    init_mpfr_mat(A_raw, m, k, lda, prec, state);
    init_mpfr_mat(B_raw, k, n, ldb, prec, state);
    init_mpfr_mat(C_raw, m, n, ldc, prec, state);

    mpfr_class *A = new mpfr_class[m * k];
    mpfr_class *B = new mpfr_class[k * n];
    mpfr_class *C = new mpfr_class[m * n];
    mpfr_class *C_ref = new mpfr_class[m * n];
    mpfr_class alpha = mpfr_class(alpha_raw);
    mpfr_class beta = mpfr_class(beta_raw);
    for (int64_t j = 0; j < k; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A[i + j * lda] = mpfr_class(A_raw[i + j * lda]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < k; ++i) {
            B[i + j * ldb] = mpfr_class(B_raw[i + j * ldb]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] = mpfr_class(C_raw[i + j * ldc]);
            C_ref[i + j * ldc] = C[i + j * ldc];
        }
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(m, k, n, alpha, A, lda, B, ldb, beta, C, ldc);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Rgemm("n", "n", m, n, k, alpha, A, lda, B, ldb, beta, C_ref, ldc);

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = flops_gemm(m, n, k) / (elapsed_seconds.count() * MFLOPS);
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(C, C_ref, m, n, ldc));

    clear_mpfr_mat(A_raw, m, k, lda);
    clear_mpfr_mat(B_raw, k, n, ldb);
    clear_mpfr_mat(C_raw, m, n, ldc);
    mpfr_clear(alpha_raw);
    mpfr_clear(beta_raw);
    delete[] A_raw;
    delete[] B_raw;
    delete[] C_raw;
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_ref;
    gmp_randclear(state);
    return EXIT_SUCCESS;
}

#endif // GMPFRXX_MKII_BENCHMARK_MPFR_RGEMM_COMMON_HPP
