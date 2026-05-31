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
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <gmp.h>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Rgemm.hpp"

namespace rgemm_gmp_bench {

constexpr double MflopsScale = 1e+6;
constexpr int64_t FixedRowBlockSize = 256;

inline double flops_gemm(int64_t m_i, int64_t n_i, int64_t k_i) {
    const double m = static_cast<double>(m_i);
    const double n = static_cast<double>(n_i);
    const double k = static_cast<double>(k_i);
    return m * (k + 2.0) * n + m * k * n;
}

inline int64_t fixed_row_block(int64_t m) {
    return std::min<int64_t>(FixedRowBlockSize, m);
}

inline void configure_mpf_precision(int prec) {
    mpf_set_default_prec(prec);
#if !defined(USE_ORIGINAL_GMPXX)
    gmpxx::set_default_mpf_precision_bits(prec);
#endif
}

inline void require_mpf_precision_at_least(const char *label, mp_bitcnt_t actual, mp_bitcnt_t requested) {
    if (actual >= requested) {
        return;
    }
    std::cerr << "Precision check failed for " << label << ": requested at least " << requested
              << " bits, actual " << actual << " bits" << std::endl;
    std::exit(EXIT_FAILURE);
}

inline mp_bitcnt_t class_precision_bits(const mpf_class &value) {
    return mpf_get_prec(value.get_mpf_t());
}

inline bool is_nocheck_option(const char *arg) noexcept {
    return std::strcmp(arg, "-nocheck") == 0 || std::strcmp(arg, "--nocheck") == 0 || std::strcmp(arg, "--no-check") == 0;
}

inline bool parse_nocheck_option(int argc, char **argv, bool &skip_check) {
    skip_check = false;
    if (argc == 5) {
        return true;
    }
    if (argc == 6 && is_nocheck_option(argv[5])) {
        skip_check = true;
        return true;
    }
    return false;
}

inline void print_rgemm_usage(const char *program) {
    std::cerr << "Usage: " << program << " <rows m> <cols k> <cols n> <precision> [-nocheck]" << std::endl;
}

inline void init_mpf_mat(mpf_t *mat, int64_t rows, int64_t cols, int64_t ld, int prec, gmp_randstate_t state) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpf_init2(mat[i + j * ld], prec);
            mpf_urandomb(mat[i + j * ld], state, prec);
        }
    }
}

inline void clear_mpf_mat(mpf_t *mat, int64_t rows, int64_t cols, int64_t ld) {
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            mpf_clear(mat[i + j * ld]);
        }
    }
}

inline mpf_class l1_norm_difference(const mpf_class *lhs, const mpf_class *rhs, int64_t rows, int64_t cols, int64_t ld) {
    mpf_class l1_norm = 0;
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            l1_norm += abs(lhs[i + j * ld] - rhs[i + j * ld]);
        }
    }
    return l1_norm;
}

inline mpf_class l1_norm_difference(const mpf_t *lhs, const mpf_class *rhs, int64_t rows, int64_t cols, int64_t ld) {
    mpf_class l1_norm = 0;
    for (int64_t j = 0; j < cols; ++j) {
        for (int64_t i = 0; i < rows; ++i) {
            l1_norm += abs(mpf_class(lhs[i + j * ld]) - rhs[i + j * ld]);
        }
    }
    return l1_norm;
}

inline int print_check_result(const mpf_class &l1_norm) {
    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fg\n", l1_norm.get_mpf_t());

    const mpf_class threshold = 1e-5;
    if (l1_norm > threshold) {
        std::cerr << "Check failed: L1 norm exceeds threshold." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Check passed." << std::endl;
    return EXIT_SUCCESS;
}

using NativeKernel = void (*)(int64_t, int64_t, int64_t, const mpf_t, const mpf_t *, int64_t, const mpf_t *, int64_t, const mpf_t, mpf_t *, int64_t);
using NativeRowBlockKernel = void (*)(int64_t, int64_t, int64_t, int64_t, const mpf_t, const mpf_t *, int64_t, const mpf_t *, int64_t, const mpf_t, mpf_t *, int64_t);
using ClassKernel = void (*)(int64_t, int64_t, int64_t, const mpf_class &, const mpf_class *, int64_t, const mpf_class *, int64_t, const mpf_class &, mpf_class *, int64_t);
using ClassRowBlockKernel = void (*)(int64_t, int64_t, int64_t, int64_t, const mpf_class &, const mpf_class *, int64_t, const mpf_class *, int64_t, const mpf_class &, mpf_class *, int64_t);

inline int run_native_rgemm_benchmark(int argc, char **argv, NativeKernel kernel) {
    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, skip_check)) {
        print_rgemm_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t k = std::atoll(argv[2]);
    const int64_t n = std::atoll(argv[3]);
    const int prec = std::atoi(argv[4]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        return EXIT_FAILURE;
    }
    const mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(prec);
    const int64_t lda = m;
    const int64_t ldb = k;
    const int64_t ldc = m;

    configure_mpf_precision(prec);
    require_mpf_precision_at_least("default_mpf_precision", mpf_get_default_prec(), requested_prec);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    mpf_t alpha;
    mpf_t beta;
    mpf_init2(alpha, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_urandomb(beta, state, prec);

    mpf_t *A = new mpf_t[m * k];
    mpf_t *B = new mpf_t[k * n];
    mpf_t *C = new mpf_t[m * n];
    init_mpf_mat(A, m, k, lda, prec, state);
    init_mpf_mat(B, k, n, ldb, prec, state);
    init_mpf_mat(C, m, n, ldc, prec, state);

    mpf_class alpha_ref;
    mpf_class beta_ref;
    mpf_class *A_ref = nullptr;
    mpf_class *B_ref = nullptr;
    mpf_class *C_ref = nullptr;
    if (!skip_check) {
        alpha_ref = mpf_class(alpha);
        beta_ref = mpf_class(beta);
        A_ref = new mpf_class[m * k];
        B_ref = new mpf_class[k * n];
        C_ref = new mpf_class[m * n];
        for (int64_t j = 0; j < k; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                A_ref[i + j * lda] = mpf_class(A[i + j * lda]);
            }
        }
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < k; ++i) {
                B_ref[i + j * ldb] = mpf_class(B[i + j * ldb]);
            }
        }
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                C_ref[i + j * ldc] = mpf_class(C[i + j * ldc]);
            }
        }
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(m, k, n, alpha, A, lda, B, ldb, beta, C, ldc);
    const auto end = std::chrono::high_resolution_clock::now();

    if (!skip_check) {
        Rgemm("n", "n", m, n, k, alpha_ref, A_ref, lda, B_ref, ldb, beta_ref, C_ref, ldc);
    }

    const std::chrono::duration<double> elapsed = end - start;
    const double mflops = flops_gemm(m, n, k) / (elapsed.count() * MflopsScale);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    int result = EXIT_SUCCESS;
    if (!skip_check) {
        const mpf_class l1_norm = l1_norm_difference(C, C_ref, m, n, ldc);
        result = print_check_result(l1_norm);
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    clear_mpf_mat(A, m, k, lda);
    clear_mpf_mat(B, k, n, ldb);
    clear_mpf_mat(C, m, n, ldc);
    mpf_clear(alpha);
    mpf_clear(beta);
    gmp_randclear(state);
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] A_ref;
    delete[] B_ref;
    delete[] C_ref;
    return result;
}

inline int run_native_rgemm_row_block_benchmark(int argc, char **argv, NativeRowBlockKernel kernel) {
    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, skip_check)) {
        print_rgemm_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t k = std::atoll(argv[2]);
    const int64_t n = std::atoll(argv[3]);
    const int prec = std::atoi(argv[4]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        return EXIT_FAILURE;
    }
    const mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(prec);
    const int64_t row_block = fixed_row_block(m);
    std::cout << "Fixed row block size: " << row_block << std::endl;

    const int64_t lda = m;
    const int64_t ldb = k;
    const int64_t ldc = m;

    configure_mpf_precision(prec);
    require_mpf_precision_at_least("default_mpf_precision", mpf_get_default_prec(), requested_prec);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    mpf_t alpha;
    mpf_t beta;
    mpf_init2(alpha, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_urandomb(beta, state, prec);

    mpf_t *A = new mpf_t[m * k];
    mpf_t *B = new mpf_t[k * n];
    mpf_t *C = new mpf_t[m * n];
    init_mpf_mat(A, m, k, lda, prec, state);
    init_mpf_mat(B, k, n, ldb, prec, state);
    init_mpf_mat(C, m, n, ldc, prec, state);

    mpf_class alpha_ref;
    mpf_class beta_ref;
    mpf_class *A_ref = nullptr;
    mpf_class *B_ref = nullptr;
    mpf_class *C_ref = nullptr;
    if (!skip_check) {
        alpha_ref = mpf_class(alpha);
        beta_ref = mpf_class(beta);
        A_ref = new mpf_class[m * k];
        B_ref = new mpf_class[k * n];
        C_ref = new mpf_class[m * n];
        for (int64_t j = 0; j < k; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                A_ref[i + j * lda] = mpf_class(A[i + j * lda]);
            }
        }
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < k; ++i) {
                B_ref[i + j * ldb] = mpf_class(B[i + j * ldb]);
            }
        }
        for (int64_t j = 0; j < n; ++j) {
            for (int64_t i = 0; i < m; ++i) {
                C_ref[i + j * ldc] = mpf_class(C[i + j * ldc]);
            }
        }
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(m, k, n, row_block, alpha, A, lda, B, ldb, beta, C, ldc);
    const auto end = std::chrono::high_resolution_clock::now();

    if (!skip_check) {
        Rgemm("n", "n", m, n, k, alpha_ref, A_ref, lda, B_ref, ldb, beta_ref, C_ref, ldc);
    }

    const std::chrono::duration<double> elapsed = end - start;
    const double mflops = flops_gemm(m, n, k) / (elapsed.count() * MflopsScale);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    int result = EXIT_SUCCESS;
    if (!skip_check) {
        const mpf_class l1_norm = l1_norm_difference(C, C_ref, m, n, ldc);
        result = print_check_result(l1_norm);
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    clear_mpf_mat(A, m, k, lda);
    clear_mpf_mat(B, k, n, ldb);
    clear_mpf_mat(C, m, n, ldc);
    mpf_clear(alpha);
    mpf_clear(beta);
    gmp_randclear(state);
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] A_ref;
    delete[] B_ref;
    delete[] C_ref;
    return result;
}

inline int run_class_rgemm_benchmark(int argc, char **argv, ClassKernel kernel) {
    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, skip_check)) {
        print_rgemm_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t k = std::atoll(argv[2]);
    const int64_t n = std::atoll(argv[3]);
    const int prec = std::atoi(argv[4]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        return EXIT_FAILURE;
    }
    const mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(prec);
    const int64_t lda = m;
    const int64_t ldb = k;
    const int64_t ldc = m;

    configure_mpf_precision(prec);
    require_mpf_precision_at_least("default_mpf_precision", mpf_get_default_prec(), requested_prec);

    gmp_randclass random(gmp_randinit_default);
    random.seed(42);

    mpf_class alpha = random.get_f(prec);
    mpf_class beta = random.get_f(prec);
    mpf_class *A = new mpf_class[m * k];
    mpf_class *B = new mpf_class[k * n];
    mpf_class *C = new mpf_class[m * n];
    mpf_class *C_ref = skip_check ? nullptr : new mpf_class[m * n];

    for (int64_t j = 0; j < k; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A[i + j * lda] = random.get_f(prec);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < k; ++i) {
            B[i + j * ldb] = random.get_f(prec);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] = random.get_f(prec);
            if (!skip_check) {
                C_ref[i + j * ldc] = mpf_class(C[i + j * ldc]);
            }
        }
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(m, k, n, alpha, A, lda, B, ldb, beta, C, ldc);
    const auto end = std::chrono::high_resolution_clock::now();

    if (!skip_check) {
        Rgemm("n", "n", m, n, k, alpha, A, lda, B, ldb, beta, C_ref, ldc);
    }

    const std::chrono::duration<double> elapsed = end - start;
    const double mflops = flops_gemm(m, n, k) / (elapsed.count() * MflopsScale);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    int result = EXIT_SUCCESS;
    if (!skip_check) {
        const mpf_class l1_norm = l1_norm_difference(C, C_ref, m, n, ldc);
        result = print_check_result(l1_norm);
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_ref;
    return result;
}

inline int run_class_rgemm_row_block_benchmark(int argc, char **argv, ClassRowBlockKernel kernel) {
    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, skip_check)) {
        print_rgemm_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t k = std::atoll(argv[2]);
    const int64_t n = std::atoll(argv[3]);
    const int prec = std::atoi(argv[4]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        return EXIT_FAILURE;
    }
    const mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(prec);
    const int64_t row_block = fixed_row_block(m);
    std::cout << "Fixed row block size: " << row_block << std::endl;

    const int64_t lda = m;
    const int64_t ldb = k;
    const int64_t ldc = m;

    configure_mpf_precision(prec);
    require_mpf_precision_at_least("default_mpf_precision", mpf_get_default_prec(), requested_prec);

    gmp_randclass random(gmp_randinit_default);
    random.seed(42);

    mpf_class alpha = random.get_f(prec);
    mpf_class beta = random.get_f(prec);
    mpf_class *A = new mpf_class[m * k];
    mpf_class *B = new mpf_class[k * n];
    mpf_class *C = new mpf_class[m * n];
    mpf_class *C_ref = skip_check ? nullptr : new mpf_class[m * n];

    for (int64_t j = 0; j < k; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A[i + j * lda] = random.get_f(prec);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < k; ++i) {
            B[i + j * ldb] = random.get_f(prec);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            C[i + j * ldc] = random.get_f(prec);
            if (!skip_check) {
                C_ref[i + j * ldc] = mpf_class(C[i + j * ldc]);
            }
        }
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(m, k, n, row_block, alpha, A, lda, B, ldb, beta, C, ldc);
    const auto end = std::chrono::high_resolution_clock::now();

    if (!skip_check) {
        Rgemm("n", "n", m, n, k, alpha, A, lda, B, ldb, beta, C_ref, ldc);
    }

    const std::chrono::duration<double> elapsed = end - start;
    const double mflops = flops_gemm(m, n, k) / (elapsed.count() * MflopsScale);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;

    int result = EXIT_SUCCESS;
    if (!skip_check) {
        const mpf_class l1_norm = l1_norm_difference(C, C_ref, m, n, ldc);
        result = print_check_result(l1_norm);
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] C_ref;
    return result;
}

} // namespace rgemm_gmp_bench
