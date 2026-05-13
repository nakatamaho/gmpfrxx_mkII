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
#include <cstdlib>
#include <iostream>
#include <gmp.h>

#include "benchmark_allocator_counter.hpp"

#if defined USE_ORIGINAL_GMPXX
#include <gmpxx.h>
#else
#include "gmpxx_mkII.h"
using namespace gmpxx;
#endif

#include "Rdot.hpp"

#define MFLOPS 1e+6

gmp_randstate_t state;

mpf_class _Rdot(int64_t n, mpf_class *dx, int64_t incx, mpf_class *dy, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    const int64_t unrolled_n = n - (n % 4);
    mpf_class result = 0.0;

#pragma omp parallel
    {
        mpf_class acc0 = 0.0;
        mpf_class acc1 = 0.0;
        mpf_class acc2 = 0.0;
        mpf_class acc3 = 0.0;
        mpf_class templ0;
        mpf_class templ1;
        mpf_class templ2;
        mpf_class templ3;

#pragma omp for schedule(static)
        for (int64_t i = 0; i < unrolled_n; i += 4) {
            templ0 = dx[i] * dy[i];
            templ1 = dx[i + 1] * dy[i + 1];
            templ2 = dx[i + 2] * dy[i + 2];
            templ3 = dx[i + 3] * dy[i + 3];

            acc0 += templ0;
            acc1 += templ1;
            acc2 += templ2;
            acc3 += templ3;
        }

#pragma omp for schedule(static)
        for (int64_t i = unrolled_n; i < n; ++i) {
            templ0 = dx[i] * dy[i];
            acc0 += templ0;
        }

        acc0 += acc1;
        acc2 += acc3;
        acc0 += acc2;

#pragma omp critical
        result += acc0;
    }

    return result;
}

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

int main(int argc, char **argv) {
    benchmark_allocator_counter::install();
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return 1;
    }

    int N = std::atoi(argv[1]);
    int prec = std::atoi(argv[2]);
    mpf_set_default_prec(prec);
#if !defined(USE_ORIGINAL_GMPXX)
    gmpxx::set_default_mpf_precision_bits(prec);
#endif

    mpf_t *vec1 = new mpf_t[N];
    mpf_t *vec2 = new mpf_t[N];
    mpf_t tmp, dot_product;

    mpf_init2(dot_product, prec);
    mpf_init2(tmp, prec);
    init_mpf_vec(vec1, N, prec);
    init_mpf_vec(vec2, N, prec);

    mpf_class *vec1_mpf_class = new mpf_class[N];
    mpf_class *vec2_mpf_class = new mpf_class[N];
    mpf_class _ans;

    for (int i = 0; i < N; i++) {
        vec1_mpf_class[i] = mpf_class(vec1[i]);
        vec2_mpf_class[i] = mpf_class(vec2[i]);
    }

    benchmark_allocator_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    _ans = _Rdot(N, vec1_mpf_class, 1, vec2_mpf_class, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_allocator_counter::print_kernel("timed_kernel");

    mpf_class ans = Rdot(N, vec1_mpf_class, 1, vec2_mpf_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(N) - 1.0) / elapsed_seconds.count() / MFLOPS << std::endl;

    mpf_class _tmp;
    _tmp = abs(_ans - ans);
    std::cout << "DIFF: ";
    gmp_printf("%.4Fg ", _tmp.get_mpf_t());
    if (_tmp < 1e-5)
        std::cout << "OK" << std::endl;
    else
        std::cout << "NG" << std::endl;

    clear_mpf_vec(vec1, N);
    clear_mpf_vec(vec2, N);
    mpf_clear(tmp);
    mpf_clear(dot_product);
    delete[] vec1;
    delete[] vec2;

    return 0;
}
