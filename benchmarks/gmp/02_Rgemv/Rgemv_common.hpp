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


#pragma once

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <gmp.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Rgemv.hpp"

namespace rgemv_gmp_bench {

constexpr double MflopsScale = 1e+6;

inline void configure_mpf_precision(int prec) {
    mpf_set_default_prec(prec);
#if !defined(USE_ORIGINAL_GMPXX)
    gmpxx::set_default_mpf_precision_bits(prec);
#endif
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

inline void init_mpf_vec(mpf_t *vec, int64_t n, int prec, gmp_randstate_t state) {
    for (int64_t i = 0; i < n; ++i) {
        mpf_init2(vec[i], prec);
        mpf_urandomb(vec[i], state, prec);
    }
}

inline void clear_mpf_vec(mpf_t *vec, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        mpf_clear(vec[i]);
    }
}

inline mpf_class l1_norm_difference(const mpf_t *raw, const mpf_class *wrapped, int64_t n) {
    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < n; ++i) {
        l1_norm += abs(mpf_class(raw[i]) - wrapped[i]);
    }
    return l1_norm;
}

inline mpf_class l1_norm_difference(const mpf_class *lhs, const mpf_class *rhs, int64_t n) {
    mpf_class l1_norm = 0;
    for (int64_t i = 0; i < n; ++i) {
        l1_norm += abs(lhs[i] - rhs[i]);
    }
    return l1_norm;
}

inline int print_l1_result(const mpf_class &l1_norm) {
    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fg\n", l1_norm.get_mpf_t());

    const mpf_class threshold = 1e-5;
    if (l1_norm < threshold) {
        std::cout << "Result OK" << std::endl;
        return EXIT_SUCCESS;
    }
    std::cout << "Result NG" << std::endl;
    return EXIT_FAILURE;
}

using NativeRgemvKernel = void (*)(int64_t, int64_t, const mpf_t, const mpf_t *, int64_t, const mpf_t *, int64_t, const mpf_t, mpf_t *, int64_t);
using ClassRgemvKernel = void (*)(int64_t, int64_t, const mpf_class &, const mpf_class *, int64_t, const mpf_class *, int64_t, const mpf_class &, mpf_class *, int64_t);

inline int run_native_rgemv_benchmark(int argc, char **argv, NativeRgemvKernel kernel) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols n> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t n = std::atoll(argv[2]);
    const int prec = std::atoi(argv[3]);
    configure_mpf_precision(prec);

    const int64_t lda = m;
    mpf_t alpha, beta;
    mpf_init2(alpha, prec);
    mpf_init2(beta, prec);
    mpf_urandomb(alpha, state, prec);
    mpf_urandomb(beta, state, prec);

    mpf_t *A = new mpf_t[m * n];
    mpf_t *x = new mpf_t[n];
    mpf_t *y = new mpf_t[m];
    init_mpf_mat(A, m, n, lda, prec, state);
    init_mpf_vec(x, n, prec, state);
    init_mpf_vec(y, m, prec, state);

    mpf_class alpha_ref = mpf_class(alpha);
    mpf_class beta_ref = mpf_class(beta);
    mpf_class *A_ref = new mpf_class[m * n];
    mpf_class *x_ref = new mpf_class[n];
    mpf_class *y_ref = new mpf_class[m];
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A_ref[i + j * lda] = mpf_class(A[i + j * lda]);
        }
    }
    for (int64_t i = 0; i < n; ++i) {
        x_ref[i] = mpf_class(x[i]);
    }
    for (int64_t i = 0; i < m; ++i) {
        y_ref[i] = mpf_class(y[i]);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(m, n, alpha, A, lda, x, 1, beta, y, 1);
    const auto end = std::chrono::high_resolution_clock::now();

    Rgemv("n", m, n, alpha_ref, A_ref, lda, x_ref, 1, beta_ref, y_ref, 1);

    const std::chrono::duration<double> elapsed = end - start;
    const double mflops = (2.0 * double(m) * double(n)) / (elapsed.count() * MflopsScale);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    const int result = print_l1_result(l1_norm_difference(y, y_ref, m));

    clear_mpf_mat(A, m, n, lda);
    clear_mpf_vec(x, n);
    clear_mpf_vec(y, m);
    mpf_clear(alpha);
    mpf_clear(beta);
    delete[] A;
    delete[] x;
    delete[] y;
    delete[] A_ref;
    delete[] x_ref;
    delete[] y_ref;
    gmp_randclear(state);

    return result;
}

inline int run_class_rgemv_benchmark(int argc, char **argv, ClassRgemvKernel kernel) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <rows m> <cols n> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t n = std::atoll(argv[2]);
    const int prec = std::atoi(argv[3]);
    configure_mpf_precision(prec);

    const int64_t lda = m;
    mpf_t alpha_raw, beta_raw;
    mpf_init2(alpha_raw, prec);
    mpf_init2(beta_raw, prec);
    mpf_urandomb(alpha_raw, state, prec);
    mpf_urandomb(beta_raw, state, prec);

    mpf_t *A_raw = new mpf_t[m * n];
    mpf_t *x_raw = new mpf_t[n];
    mpf_t *y_raw = new mpf_t[m];
    init_mpf_mat(A_raw, m, n, lda, prec, state);
    init_mpf_vec(x_raw, n, prec, state);
    init_mpf_vec(y_raw, m, prec, state);

    mpf_class alpha = mpf_class(alpha_raw);
    mpf_class beta = mpf_class(beta_raw);
    mpf_class *A = new mpf_class[m * n];
    mpf_class *x = new mpf_class[n];
    mpf_class *y = new mpf_class[m];
    mpf_class *reference_y = new mpf_class[m];
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A[i + j * lda] = mpf_class(A_raw[i + j * lda]);
        }
    }
    for (int64_t i = 0; i < n; ++i) {
        x[i] = mpf_class(x_raw[i]);
    }
    for (int64_t i = 0; i < m; ++i) {
        y[i] = mpf_class(y_raw[i]);
        reference_y[i] = y[i];
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(m, n, alpha, A, lda, x, 1, beta, y, 1);
    const auto end = std::chrono::high_resolution_clock::now();

    Rgemv("n", m, n, alpha, A, lda, x, 1, beta, reference_y, 1);

    const std::chrono::duration<double> elapsed = end - start;
    const double mflops = (2.0 * double(m) * double(n)) / (elapsed.count() * MflopsScale);

    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    const int result = print_l1_result(l1_norm_difference(y, reference_y, m));

    clear_mpf_mat(A_raw, m, n, lda);
    clear_mpf_vec(x_raw, n);
    clear_mpf_vec(y_raw, m);
    mpf_clear(alpha_raw);
    mpf_clear(beta_raw);
    delete[] A_raw;
    delete[] x_raw;
    delete[] y_raw;
    delete[] A;
    delete[] x;
    delete[] y;
    delete[] reference_y;
    gmp_randclear(state);

    return result;
}

} // namespace rgemv_gmp_bench
