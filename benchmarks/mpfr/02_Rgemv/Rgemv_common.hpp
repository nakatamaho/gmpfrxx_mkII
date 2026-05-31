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

#ifndef GMPFRXX_MKII_BENCHMARK_MPFR_RGEMV_COMMON_HPP
#define GMPFRXX_MKII_BENCHMARK_MPFR_RGEMV_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <gmp.h>

#include "mpfr_operation_counter.hpp"

#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Rgemv.hpp"

#define MFLOPS 1e+6

inline void init_mpfr_mat(mpfr_t *mat, int64_t m, int64_t n, int64_t lda, int prec, gmp_randstate_t state) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpfr_init2(mat[i + j * lda], prec);
            mpfr_urandomb(mat[i + j * lda], state);
        }
    }
}

inline void clear_mpfr_mat(mpfr_t *mat, int64_t m, int64_t n, int64_t lda) {
    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            mpfr_clear(mat[i + j * lda]);
        }
    }
}

inline void init_mpfr_vec(mpfr_t *vec, int64_t n, int prec, gmp_randstate_t state) {
    for (int64_t i = 0; i < n; ++i) {
        mpfr_init2(vec[i], prec);
        mpfr_urandomb(vec[i], state);
    }
}

inline void clear_mpfr_vec(mpfr_t *vec, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        mpfr_clear(vec[i]);
    }
}

inline mpfr_class l1_norm_difference(const mpfr_t *raw, const mpfr_class *wrapped, int64_t n) {
    mpfr_class l1_norm = 0;
    for (int64_t i = 0; i < n; ++i) {
        l1_norm += abs(mpfr_class(raw[i]) - wrapped[i]);
    }
    return l1_norm;
}

inline mpfr_class l1_norm_difference(const mpfr_class *lhs, const mpfr_class *rhs, int64_t n) {
    mpfr_class l1_norm = 0;
    for (int64_t i = 0; i < n; ++i) {
        l1_norm += abs(lhs[i] - rhs[i]);
    }
    return l1_norm;
}

inline bool is_nocheck_option(const char *arg) noexcept {
    return std::strcmp(arg, "-nocheck") == 0 || std::strcmp(arg, "--nocheck") == 0 || std::strcmp(arg, "--no-check") == 0;
}

inline bool parse_nocheck_option(int argc, char **argv, int expected_argc, bool &skip_check) {
    skip_check = false;
    if (argc == expected_argc) {
        return true;
    }
    if (argc == expected_argc + 1 && is_nocheck_option(argv[expected_argc])) {
        skip_check = true;
        return true;
    }
    return false;
}

inline void print_nocheck_usage(const char *program, const char *args) {
    std::cerr << "Usage: " << program << " " << args << " [-nocheck]" << std::endl;
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

inline int run_native_rgemv_benchmark(int argc, char **argv,
                                      void (*kernel)(int64_t, int64_t, const mpfr_t, const mpfr_t *, int64_t, const mpfr_t *, int64_t, const mpfr_t, mpfr_t *, int64_t)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, 4, skip_check)) {
        print_nocheck_usage(argv[0], "<rows m> <cols n> <precision>");
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t n = std::atoll(argv[2]);
    const int prec = std::atoi(argv[3]);
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    const int64_t lda = m;
    mpfr_t *A = new mpfr_t[m * n];
    mpfr_t *x = new mpfr_t[n];
    mpfr_t *y = new mpfr_t[m];

    mpfr_t alpha, beta;
    mpfr_init2(alpha, prec);
    mpfr_urandomb(alpha, state);
    mpfr_init2(beta, prec);
    mpfr_urandomb(beta, state);

    init_mpfr_mat(A, m, n, lda, prec, state);
    init_mpfr_vec(x, n, prec, state);
    init_mpfr_vec(y, m, prec, state);

    mpfr_class *A_class = new mpfr_class[m * n];
    mpfr_class *x_class = new mpfr_class[n];
    mpfr_class *y_class = new mpfr_class[m];
    mpfr_class alpha_class = mpfr_class(alpha);
    mpfr_class beta_class = mpfr_class(beta);

    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A_class[i + j * lda] = mpfr_class(A[i + j * lda]);
        }
    }
    for (int64_t i = 0; i < n; ++i) {
        x_class[i] = mpfr_class(x[i]);
    }
    for (int64_t i = 0; i < m; ++i) {
        y_class[i] = mpfr_class(y[i]);
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(m, n, alpha, A, lda, x, 1, beta, y, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    if (!skip_check) {
        Rgemv("n", m, n, alpha_class, A_class, lda, x_class, 1, beta_class, y_class, 1);
    }

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(m) * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    if (!skip_check) {
        print_l1_result(l1_norm_difference(y, y_class, m));
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    clear_mpfr_mat(A, m, n, lda);
    clear_mpfr_vec(x, n);
    clear_mpfr_vec(y, m);
    mpfr_clear(alpha);
    mpfr_clear(beta);
    delete[] A;
    delete[] x;
    delete[] y;
    delete[] A_class;
    delete[] x_class;
    delete[] y_class;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

inline int run_class_rgemv_benchmark(int argc, char **argv,
                                     void (*kernel)(int64_t, int64_t, const mpfr_class &, const mpfr_class *, int64_t, const mpfr_class *, int64_t, const mpfr_class &, mpfr_class *, int64_t)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, 4, skip_check)) {
        print_nocheck_usage(argv[0], "<rows> <cols> <precision>");
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t m = std::atoll(argv[1]);
    const int64_t n = std::atoll(argv[2]);
    const int prec = std::atoi(argv[3]);
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    const int64_t lda = m;
    mpfr_t *A_raw = new mpfr_t[m * n];
    mpfr_t *x_raw = new mpfr_t[n];
    mpfr_t *y_raw = new mpfr_t[m];
    mpfr_t alpha_raw, beta_raw;
    mpfr_init2(alpha_raw, prec);
    mpfr_urandomb(alpha_raw, state);
    mpfr_init2(beta_raw, prec);
    mpfr_urandomb(beta_raw, state);
    init_mpfr_mat(A_raw, m, n, lda, prec, state);
    init_mpfr_vec(x_raw, n, prec, state);
    init_mpfr_vec(y_raw, m, prec, state);

    mpfr_class *A = new mpfr_class[m * n];
    mpfr_class *x = new mpfr_class[n];
    mpfr_class *y = new mpfr_class[m];
    mpfr_class *yy = new mpfr_class[m];
    mpfr_class alpha = mpfr_class(alpha_raw);
    mpfr_class beta = mpfr_class(beta_raw);

    for (int64_t j = 0; j < n; ++j) {
        for (int64_t i = 0; i < m; ++i) {
            A[i + j * lda] = mpfr_class(A_raw[i + j * lda]);
        }
    }
    for (int64_t j = 0; j < n; ++j) {
        x[j] = mpfr_class(x_raw[j]);
    }
    for (int64_t i = 0; i < m; ++i) {
        y[i] = mpfr_class(y_raw[i]);
        yy[i] = y[i];
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(m, n, alpha, A, lda, x, 1, beta, y, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    if (!skip_check) {
        Rgemv("n", m, n, alpha, A, lda, x, 1, beta, yy, 1);
    }

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(m) * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    if (!skip_check) {
        print_l1_result(l1_norm_difference(y, yy, m));
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    clear_mpfr_mat(A_raw, m, n, lda);
    clear_mpfr_vec(x_raw, n);
    clear_mpfr_vec(y_raw, m);
    mpfr_clear(alpha_raw);
    mpfr_clear(beta_raw);
    delete[] A_raw;
    delete[] x_raw;
    delete[] y_raw;
    delete[] A;
    delete[] x;
    delete[] y;
    delete[] yy;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

#endif // GMPFRXX_MKII_BENCHMARK_MPFR_RGEMV_COMMON_HPP
