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
 * OR SERVICES; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <gmp.h>

#include "mpf_operation_counter.hpp"

#ifndef GMP_RDOT_MICROBENCH_MODE
#define GMP_RDOT_MICROBENCH_MODE 10
#endif

#define MFLOPS 1e+6

gmp_randstate_t state;
volatile unsigned long long rdot_read_sink = 0;

void init_mpf_vec(mpf_t *vec, int n, int prec) {
    for (int i = 0; i < n; i++) {
        mpf_init2(vec[i], prec);
        mpf_urandomb(vec[i], state, prec);
    }
}

void clear_mpf_vec(mpf_t *vec, int n) {
    for (int i = 0; i < n; i++) {
        mpf_clear(vec[i]);
    }
}

void bench_readonly(int64_t n, mpf_t *dx, mpf_t *dy) {
    unsigned long long sink = 0;
    for (int64_t i = 0; i < n; ++i) {
        const __mpf_struct &x = dx[i][0];
        const __mpf_struct &y = dy[i][0];
        sink += static_cast<unsigned long long>(x._mp_size);
        sink += static_cast<unsigned long long>(y._mp_size);
        sink += static_cast<unsigned long long>(x._mp_exp);
        sink += static_cast<unsigned long long>(y._mp_exp);
        sink ^= static_cast<unsigned long long>(x._mp_d[0]);
        sink += static_cast<unsigned long long>(y._mp_d[0]);
    }
    rdot_read_sink = sink;
}

void bench_addonly(int64_t n, mpf_t *dx, mpf_t *ans) {
    mpf_t acc;
    mpf_init(acc);
    mpf_set_ui(acc, 0);
    for (int64_t i = 0; i < n; ++i) {
        mpf_add(acc, acc, dx[i]);
    }
    mpf_swap(*ans, acc);
    mpf_clear(acc);
}

void bench_mulonly(int64_t n, mpf_t *dx, mpf_t *dy, mpf_t *ans) {
    mpf_t templ;
    mpf_init(templ);
    mpf_set_ui(templ, 0);
    for (int64_t i = 0; i < n; ++i) {
        mpf_mul(templ, dx[i], dy[i]);
    }
    mpf_swap(*ans, templ);
    mpf_clear(templ);
}

void bench_muladd(int64_t n, mpf_t *dx, mpf_t *dy, mpf_t *ans) {
    mpf_t acc, templ;
    mpf_init(acc);
    mpf_init(templ);
    mpf_set_ui(acc, 0);
    mpf_set_ui(templ, 0);
    for (int64_t i = 0; i < n; ++i) {
        mpf_mul(templ, dx[i], dy[i]);
        mpf_add(acc, acc, templ);
    }
    mpf_swap(*ans, acc);
    mpf_clear(acc);
    mpf_clear(templ);
}

const char *mode_name() {
#if GMP_RDOT_MICROBENCH_MODE == 7
    return "07_readonly";
#elif GMP_RDOT_MICROBENCH_MODE == 8
    return "08_addonly";
#elif GMP_RDOT_MICROBENCH_MODE == 9
    return "09_mulonly";
#else
    return "10_muladd";
#endif
}

int main(int argc, char **argv) {
    benchmark_allocator_counter::install();
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return 1;
    }

    const int N = std::atoi(argv[1]);
    const int prec = std::atoi(argv[2]);
    mpf_set_default_prec(prec);

    mpf_t *vec1 = new mpf_t[N];
    mpf_t *vec2 = new mpf_t[N];
    mpf_t ans;

    mpf_init2(ans, prec);
    init_mpf_vec(vec1, N, prec);
    init_mpf_vec(vec2, N, prec);

    benchmark_mpf_operation_counter::begin_kernel();
    const auto start = std::chrono::high_resolution_clock::now();
#if GMP_RDOT_MICROBENCH_MODE == 7
    bench_readonly(N, vec1, vec2);
#elif GMP_RDOT_MICROBENCH_MODE == 8
    bench_addonly(N, vec1, &ans);
#elif GMP_RDOT_MICROBENCH_MODE == 9
    bench_mulonly(N, vec1, vec2, &ans);
#else
    bench_muladd(N, vec1, vec2, &ans);
#endif
    const auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpf_operation_counter::print_kernel("timed_kernel");

    const std::chrono::duration<double> elapsed_seconds = end - start;
    const double seconds = elapsed_seconds.count();
    std::cout << "MODE: " << mode_name() << std::endl;
    std::cout << "Elapsed time: " << seconds << " s" << std::endl;
    std::cout << "MELEMENTS_PER_SEC: " << (double(N) / seconds / MFLOPS) << std::endl;
#if GMP_RDOT_MICROBENCH_MODE == 10
    std::cout << "MFLOPS: " << (2.0 * double(N) - 1.0) / seconds / MFLOPS << std::endl;
#endif
#if GMP_RDOT_MICROBENCH_MODE == 7
    std::cout << "READ_SINK: " << rdot_read_sink << std::endl;
#else
    std::cout << "ANS: ";
    gmp_printf("%.4Fg\n", ans);
#endif
    std::cout << "OK" << std::endl;

    clear_mpf_vec(vec1, N);
    clear_mpf_vec(vec2, N);
    mpf_clear(ans);
    delete[] vec1;
    delete[] vec2;

    return 0;
}
