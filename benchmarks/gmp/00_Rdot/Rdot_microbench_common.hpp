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

#include "mpf_operation_counter.hpp"

namespace rdot_gmp_microbench {

constexpr double MflopsScale = 1e+6;
inline volatile unsigned long long read_sink = 0;

enum class OutputMode {
    ReadSink,
    Answer,
    AnswerAndMflops,
};

using Kernel = void (*)(int64_t, mpf_t *, mpf_t *, mpf_t *);

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

inline int run(int argc, char **argv, const char *mode_name, OutputMode output_mode, Kernel kernel) {
    benchmark_allocator_counter::install();

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
    mpf_set_default_prec(prec);

    mpf_t *vec1 = new mpf_t[n];
    mpf_t *vec2 = new mpf_t[n];
    mpf_t answer;

    mpf_init2(answer, prec);
    init_mpf_vec(vec1, n, prec, state);
    init_mpf_vec(vec2, n, prec, state);

    benchmark_mpf_operation_counter::begin_kernel();
    const auto start = std::chrono::high_resolution_clock::now();
    kernel(n, vec1, vec2, &answer);
    const auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpf_operation_counter::print_kernel("timed_kernel");

    const std::chrono::duration<double> elapsed_seconds = end - start;
    const double seconds = elapsed_seconds.count();
    std::cout << "MODE: " << mode_name << std::endl;
    std::cout << "Elapsed time: " << seconds << " s" << std::endl;
    std::cout << "MELEMENTS_PER_SEC: " << (double(n) / seconds / MflopsScale) << std::endl;
    if (output_mode == OutputMode::AnswerAndMflops) {
        std::cout << "MFLOPS: " << (2.0 * double(n) - 1.0) / seconds / MflopsScale << std::endl;
    }
    if (output_mode == OutputMode::ReadSink) {
        std::cout << "READ_SINK: " << read_sink << std::endl;
    } else {
        std::cout << "ANS: ";
        gmp_printf("%.4Fg\n", answer);
    }
    std::cout << "OK" << std::endl;

    clear_mpf_vec(vec1, n);
    clear_mpf_vec(vec2, n);
    mpf_clear(answer);
    delete[] vec1;
    delete[] vec2;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

} // namespace rdot_gmp_microbench
