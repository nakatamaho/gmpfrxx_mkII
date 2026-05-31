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
#include <cstdio>
#include <cstdlib>
#include <cstring>
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

#include "Rdot.hpp"

namespace rdot_gmp_bench {

constexpr double MflopsScale = 1e+6;

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

inline void print_diff_result(const mpf_class &diff) {
    std::cout << "DIFF: ";
    gmp_printf("%.4Fg ", diff.get_mpf_t());
    if (diff < 1e-5) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "NG" << std::endl;
    }
}

using NativeRdotKernel = void (*)(int64_t, mpf_t *, int64_t, mpf_t *, int64_t, mpf_t *);
using ClassRdotKernel = mpf_class (*)(int64_t, mpf_class *, int64_t, mpf_class *, int64_t);

inline int run_native_rdot_benchmark(int argc, char **argv, NativeRdotKernel kernel) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, 3, skip_check)) {
        print_nocheck_usage(argv[0], "<vector size> <precision>");
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

    mpf_t *vec1 = new mpf_t[n];
    mpf_t *vec2 = new mpf_t[n];
    mpf_t raw_answer;

    mpf_init2(raw_answer, prec);
    init_mpf_vec(vec1, n, prec, state);
    init_mpf_vec(vec2, n, prec, state);

    mpf_class *vec1_class = new mpf_class[n];
    mpf_class *vec2_class = new mpf_class[n];
    for (int64_t i = 0; i < n; ++i) {
        vec1_class[i] = mpf_class(vec1[i]);
        vec2_class[i] = mpf_class(vec2[i]);
    }

    require_mpf_precision_at_least("raw_answer", mpf_get_prec(raw_answer), requested_prec);
    if (n > 0) {
        require_mpf_precision_at_least("raw_vec1[0]", mpf_get_prec(vec1[0]), requested_prec);
        require_mpf_precision_at_least("raw_vec2[0]", mpf_get_prec(vec2[0]), requested_prec);
        require_mpf_precision_at_least("class_vec1[0]", class_precision_bits(vec1_class[0]), requested_prec);
        require_mpf_precision_at_least("class_vec2[0]", class_precision_bits(vec2_class[0]), requested_prec);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    kernel(n, vec1, 1, vec2, 1, &raw_answer);
    const auto end = std::chrono::high_resolution_clock::now();

    mpf_class reference;
    if (!skip_check) {
        reference = Rdot(n, vec1_class, 1, vec2_class, 1);
        require_mpf_precision_at_least("class_reference", class_precision_bits(reference), requested_prec);
    }
    require_mpf_precision_at_least("raw_answer_after", mpf_get_prec(raw_answer), requested_prec);

    const std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(n) - 1.0) / elapsed_seconds.count() / MflopsScale << std::endl;
    if (!skip_check) {
        print_diff_result(abs(mpf_class(raw_answer) - reference));
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    clear_mpf_vec(vec1, n);
    clear_mpf_vec(vec2, n);
    mpf_clear(raw_answer);
    delete[] vec1;
    delete[] vec2;
    delete[] vec1_class;
    delete[] vec2_class;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

inline int run_class_rdot_benchmark(int argc, char **argv, ClassRdotKernel kernel) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    bool skip_check = false;
    if (!parse_nocheck_option(argc, argv, 3, skip_check)) {
        print_nocheck_usage(argv[0], "<vector size> <precision>");
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

    mpf_t *vec1_raw = new mpf_t[n];
    mpf_t *vec2_raw = new mpf_t[n];
    init_mpf_vec(vec1_raw, n, prec, state);
    init_mpf_vec(vec2_raw, n, prec, state);

    mpf_class *vec1 = new mpf_class[n];
    mpf_class *vec2 = new mpf_class[n];
    for (int64_t i = 0; i < n; ++i) {
        vec1[i] = mpf_class(vec1_raw[i]);
        vec2[i] = mpf_class(vec2_raw[i]);
    }

    if (n > 0) {
        require_mpf_precision_at_least("raw_vec1[0]", mpf_get_prec(vec1_raw[0]), requested_prec);
        require_mpf_precision_at_least("raw_vec2[0]", mpf_get_prec(vec2_raw[0]), requested_prec);
        require_mpf_precision_at_least("class_vec1[0]", class_precision_bits(vec1[0]), requested_prec);
        require_mpf_precision_at_least("class_vec2[0]", class_precision_bits(vec2[0]), requested_prec);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    const mpf_class answer = kernel(n, vec1, 1, vec2, 1);
    const auto end = std::chrono::high_resolution_clock::now();

    mpf_class reference;
    if (!skip_check) {
        reference = Rdot(n, vec1, 1, vec2, 1);
        require_mpf_precision_at_least("class_reference", class_precision_bits(reference), requested_prec);
    }
    require_mpf_precision_at_least("class_answer", class_precision_bits(answer), requested_prec);

    const std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(n) - 1.0) / elapsed_seconds.count() / MflopsScale << std::endl;
    if (!skip_check) {
        print_diff_result(abs(answer - reference));
    } else {
        std::cout << "Check skipped." << std::endl;
    }

    clear_mpf_vec(vec1_raw, n);
    clear_mpf_vec(vec2_raw, n);
    delete[] vec1_raw;
    delete[] vec2_raw;
    delete[] vec1;
    delete[] vec2;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

} // namespace rdot_gmp_bench
