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

#ifndef GMPFRXX_MKII_BENCHMARK_GMP_RAXPY_COMMON_HPP
#define GMPFRXX_MKII_BENCHMARK_GMP_RAXPY_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <gmp.h>

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Raxpy.hpp"

#define MFLOPS 1e+6

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

inline void print_l1_result(const mpf_class &l1_norm) {
    std::cout << "L1 Norm of difference: ";
    gmp_printf("%.4Fg\n", l1_norm.get_mpf_t());

    if (l1_norm < 1e-5) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }
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

using NativeRaxpyKernel = void (*)(int64_t, const mpf_t, mpf_t *, int64_t, mpf_t *, int64_t);
using ClassRaxpyKernel = void (*)(int64_t, const mpf_class &, mpf_class *, int64_t, mpf_class *, int64_t);

inline int run_native_raxpy_benchmark(int argc, char **argv, NativeRaxpyKernel kernel) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t n = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(prec);
    configure_mpf_precision(prec);

    mpf_t alpha;
    mpf_init2(alpha, prec);
    mpf_urandomb(alpha, state, prec);

    mpf_t *x = new mpf_t[n];
    mpf_t *y = new mpf_t[n];
    init_mpf_vec(x, n, prec, state);
    init_mpf_vec(y, n, prec, state);

    mpf_class alpha_class = mpf_class(alpha);
    mpf_class *x_class = new mpf_class[n];
    mpf_class *y_class = new mpf_class[n];
    for (int64_t i = 0; i < n; ++i) {
        x_class[i] = mpf_class(x[i]);
        y_class[i] = mpf_class(y[i]);
    }

    require_mpf_precision_at_least("raw_alpha", mpf_get_prec(alpha), requested_prec);
    require_mpf_precision_at_least("class_alpha", class_precision_bits(alpha_class), requested_prec);
    if (n > 0) {
        require_mpf_precision_at_least("raw_x[0]", mpf_get_prec(x[0]), requested_prec);
        require_mpf_precision_at_least("raw_y[0]", mpf_get_prec(y[0]), requested_prec);
        require_mpf_precision_at_least("class_x[0]", class_precision_bits(x_class[0]), requested_prec);
        require_mpf_precision_at_least("class_y[0]", class_precision_bits(y_class[0]), requested_prec);
    }

    auto start = std::chrono::high_resolution_clock::now();
    kernel(n, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    Raxpy(n, alpha_class, x_class, 1, y_class, 1);
    if (n > 0) {
        require_mpf_precision_at_least("raw_y_after[0]", mpf_get_prec(y[0]), requested_prec);
        require_mpf_precision_at_least("class_y_reference[0]", class_precision_bits(y_class[0]), requested_prec);
    }

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, y_class, n));

    clear_mpf_vec(x, n);
    clear_mpf_vec(y, n);
    mpf_clear(alpha);
    delete[] x;
    delete[] y;
    delete[] x_class;
    delete[] y_class;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

inline int run_class_raxpy_benchmark(int argc, char **argv, ClassRaxpyKernel kernel) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t n = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mp_bitcnt_t requested_prec = static_cast<mp_bitcnt_t>(prec);
    configure_mpf_precision(prec);

    mpf_t alpha_raw;
    mpf_init2(alpha_raw, prec);
    mpf_urandomb(alpha_raw, state, prec);

    mpf_t *x_raw = new mpf_t[n];
    mpf_t *y_raw = new mpf_t[n];
    init_mpf_vec(x_raw, n, prec, state);
    init_mpf_vec(y_raw, n, prec, state);

    mpf_class alpha = mpf_class(alpha_raw);
    mpf_class *x = new mpf_class[n];
    mpf_class *y = new mpf_class[n];
    mpf_class *yy = new mpf_class[n];
    for (int64_t i = 0; i < n; ++i) {
        x[i] = mpf_class(x_raw[i]);
        y[i] = mpf_class(y_raw[i]);
        yy[i] = y[i];
    }

    require_mpf_precision_at_least("raw_alpha", mpf_get_prec(alpha_raw), requested_prec);
    require_mpf_precision_at_least("class_alpha", class_precision_bits(alpha), requested_prec);
    if (n > 0) {
        require_mpf_precision_at_least("raw_x[0]", mpf_get_prec(x_raw[0]), requested_prec);
        require_mpf_precision_at_least("raw_y[0]", mpf_get_prec(y_raw[0]), requested_prec);
        require_mpf_precision_at_least("class_x[0]", class_precision_bits(x[0]), requested_prec);
        require_mpf_precision_at_least("class_y[0]", class_precision_bits(y[0]), requested_prec);
        require_mpf_precision_at_least("class_yy[0]", class_precision_bits(yy[0]), requested_prec);
    }

    auto start = std::chrono::high_resolution_clock::now();
    kernel(n, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    Raxpy(n, alpha, x, 1, yy, 1);
    if (n > 0) {
        require_mpf_precision_at_least("class_y_after[0]", class_precision_bits(y[0]), requested_prec);
        require_mpf_precision_at_least("class_yy_reference[0]", class_precision_bits(yy[0]), requested_prec);
    }

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, yy, n));

    clear_mpf_vec(x_raw, n);
    clear_mpf_vec(y_raw, n);
    mpf_clear(alpha_raw);
    delete[] x_raw;
    delete[] y_raw;
    delete[] x;
    delete[] y;
    delete[] yy;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

#endif // GMPFRXX_MKII_BENCHMARK_GMP_RAXPY_COMMON_HPP
