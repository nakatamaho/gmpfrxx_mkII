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

#ifndef GMPFRXX_MKII_BENCHMARK_MPFR_RDOT_COMMON_HPP
#define GMPFRXX_MKII_BENCHMARK_MPFR_RDOT_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <gmp.h>
#include <mpfr.h>

#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Rdot.hpp"

#define MFLOPS 1e+6

inline void init_mpfr_vec(mpfr_t *vec, int64_t n, int prec, gmp_randstate_t state) {
    for (int64_t i = 0; i < n; i++) {
        mpfr_init2(vec[i], prec);
        mpfr_urandomb(vec[i], state);
    }
}

inline void clear_mpfr_vec(mpfr_t *vec, int64_t n) {
    for (int64_t i = 0; i < n; i++) {
        mpfr_clear(vec[i]);
    }
}

inline void print_diff_result(const mpfr_class &diff) {
    std::cout << "DIFF: ";
    mpfr_printf("%.4Rg ", diff.get_mpfr_t());
    if (diff < 1e-5) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "NG" << std::endl;
    }
}

inline int run_native_rdot_benchmark(int argc, char **argv,
                                     void (*kernel)(int64_t, mpfr_t *, int64_t, mpfr_t *, int64_t, mpfr_t *)) {
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
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    mpfr_t *vec1 = new mpfr_t[n];
    mpfr_t *vec2 = new mpfr_t[n];
    mpfr_t raw_answer;

    mpfr_init2(raw_answer, prec);
    init_mpfr_vec(vec1, n, prec, state);
    init_mpfr_vec(vec2, n, prec, state);

    mpfr_class *vec1_class = new mpfr_class[n];
    mpfr_class *vec2_class = new mpfr_class[n];
    for (int64_t i = 0; i < n; i++) {
        vec1_class[i] = mpfr_class(vec1[i]);
        vec2_class[i] = mpfr_class(vec2[i]);
    }

    auto start = std::chrono::high_resolution_clock::now();
    kernel(n, vec1, 1, vec2, 1, &raw_answer);
    auto end = std::chrono::high_resolution_clock::now();

    mpfr_class reference = Rdot(n, vec1_class, 1, vec2_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(n) - 1.0) / elapsed_seconds.count() / MFLOPS << std::endl;
    print_diff_result(abs(mpfr_class(raw_answer) - reference));

    clear_mpfr_vec(vec1, n);
    clear_mpfr_vec(vec2, n);
    delete[] vec1_class;
    delete[] vec2_class;
    mpfr_clear(raw_answer);
    delete[] vec1;
    delete[] vec2;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

inline int run_class_rdot_benchmark(int argc, char **argv,
                                    mpfr_class (*kernel)(int64_t, mpfr_class *, int64_t, mpfr_class *, int64_t)) {
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
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    mpfr_t *vec1_raw = new mpfr_t[n];
    mpfr_t *vec2_raw = new mpfr_t[n];

    init_mpfr_vec(vec1_raw, n, prec, state);
    init_mpfr_vec(vec2_raw, n, prec, state);

    mpfr_class *vec1 = new mpfr_class[n];
    mpfr_class *vec2 = new mpfr_class[n];
    for (int64_t i = 0; i < n; i++) {
        vec1[i] = mpfr_class(vec1_raw[i]);
        vec2[i] = mpfr_class(vec2_raw[i]);
    }

    auto start = std::chrono::high_resolution_clock::now();
    mpfr_class answer = kernel(n, vec1, 1, vec2, 1);
    auto end = std::chrono::high_resolution_clock::now();

    mpfr_class reference = Rdot(n, vec1, 1, vec2, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(n) - 1.0) / elapsed_seconds.count() / MFLOPS << std::endl;
    print_diff_result(abs(answer - reference));

    clear_mpfr_vec(vec1_raw, n);
    clear_mpfr_vec(vec2_raw, n);
    delete[] vec1;
    delete[] vec2;
    delete[] vec1_raw;
    delete[] vec2_raw;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

#endif // GMPFRXX_MKII_BENCHMARK_MPFR_RDOT_COMMON_HPP
