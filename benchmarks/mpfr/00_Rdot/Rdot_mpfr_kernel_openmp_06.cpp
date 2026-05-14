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

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <gmp.h>

#include "benchmark_allocator_counter.hpp"
#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Rdot.hpp"

#define MFLOPS 1e+6

gmp_randstate_t state;

mpfr_class _Rdot(int64_t n, mpfr_class *dx, int64_t incx, mpfr_class *dy, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mpfr_prec_t precision = n > 0 ? dx[0].precision() : mpfrxx::default_precision_bits();
    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    const int64_t unrolled_n = n - (n % 4);
    mpfr_class result = mpfr_class::with_precision(precision);
    mpfr_set_zero(result.mpfr_data(), 0);

#pragma omp parallel
    {
        mpfr_class acc0 = mpfr_class::with_precision(precision);
        mpfr_class acc1 = mpfr_class::with_precision(precision);
        mpfr_class acc2 = mpfr_class::with_precision(precision);
        mpfr_class acc3 = mpfr_class::with_precision(precision);
        mpfr_set_zero(acc0.mpfr_data(), 0);
        mpfr_set_zero(acc1.mpfr_data(), 0);
        mpfr_set_zero(acc2.mpfr_data(), 0);
        mpfr_set_zero(acc3.mpfr_data(), 0);
#ifndef MPFRXX_ENABLE_FMA
        mpfr_class templ0 = mpfr_class::with_precision(precision);
        mpfr_class templ1 = mpfr_class::with_precision(precision);
        mpfr_class templ2 = mpfr_class::with_precision(precision);
        mpfr_class templ3 = mpfr_class::with_precision(precision);
#endif

#pragma omp for schedule(static) nowait
        for (int64_t i = 0; i < unrolled_n; i += 4) {
#ifdef MPFRXX_ENABLE_FMA
            mpfr_fma(acc0.mpfr_data(), dx[i].mpfr_data(), dy[i].mpfr_data(), acc0.mpfr_data(), rnd);
            mpfr_fma(acc1.mpfr_data(), dx[i + 1].mpfr_data(), dy[i + 1].mpfr_data(), acc1.mpfr_data(), rnd);
            mpfr_fma(acc2.mpfr_data(), dx[i + 2].mpfr_data(), dy[i + 2].mpfr_data(), acc2.mpfr_data(), rnd);
            mpfr_fma(acc3.mpfr_data(), dx[i + 3].mpfr_data(), dy[i + 3].mpfr_data(), acc3.mpfr_data(), rnd);
#else
            mpfr_mul(templ0.mpfr_data(), dx[i].mpfr_data(), dy[i].mpfr_data(), rnd);
            mpfr_mul(templ1.mpfr_data(), dx[i + 1].mpfr_data(), dy[i + 1].mpfr_data(), rnd);
            mpfr_mul(templ2.mpfr_data(), dx[i + 2].mpfr_data(), dy[i + 2].mpfr_data(), rnd);
            mpfr_mul(templ3.mpfr_data(), dx[i + 3].mpfr_data(), dy[i + 3].mpfr_data(), rnd);
            mpfr_add(acc0.mpfr_data(), acc0.mpfr_data(), templ0.mpfr_data(), rnd);
            mpfr_add(acc1.mpfr_data(), acc1.mpfr_data(), templ1.mpfr_data(), rnd);
            mpfr_add(acc2.mpfr_data(), acc2.mpfr_data(), templ2.mpfr_data(), rnd);
            mpfr_add(acc3.mpfr_data(), acc3.mpfr_data(), templ3.mpfr_data(), rnd);
#endif
        }

        acc0 += acc1;
        acc2 += acc3;
        acc0 += acc2;

#pragma omp critical
        result += acc0;
    }

#ifndef MPFRXX_ENABLE_FMA
    mpfr_class templ0 = mpfr_class::with_precision(precision);
#endif
    for (int64_t i = unrolled_n; i < n; ++i) {
#ifdef MPFRXX_ENABLE_FMA
        mpfr_fma(result.mpfr_data(), dx[i].mpfr_data(), dy[i].mpfr_data(), result.mpfr_data(), rnd);
#else
        mpfr_mul(templ0.mpfr_data(), dx[i].mpfr_data(), dy[i].mpfr_data(), rnd);
        mpfr_add(result.mpfr_data(), result.mpfr_data(), templ0.mpfr_data(), rnd);
#endif
    }

    return result;
}

void init_mpfr_vec(mpfr_t *vec, int n, int prec) {
    for (int i = 0; i < n; i++) {
        mpfr_init2(vec[i], prec);
        mpfr_urandomb(vec[i], state);
    }
}

void clear_mpfr_vec(mpfr_t *vec, int n) {
    for (int i = 0; i < n; i++) {
        mpfr_clear(vec[i]);
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
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    mpfr_t *vec1 = new mpfr_t[N];
    mpfr_t *vec2 = new mpfr_t[N];
    mpfr_t tmp, dot_product;

    mpfr_init2(dot_product, prec);
    mpfr_init2(tmp, prec);
    init_mpfr_vec(vec1, N, prec);
    init_mpfr_vec(vec2, N, prec);

    mpfr_class *vec1_mpfr_class = new mpfr_class[N];
    mpfr_class *vec2_mpfr_class = new mpfr_class[N];
    mpfr_class _ans = mpfr_class::with_precision(prec);
    mpfr_set_zero(_ans.mpfr_data(), 0);

    for (int i = 0; i < N; i++) {
        vec1_mpfr_class[i] = mpfr_class(vec1[i]);
        vec2_mpfr_class[i] = mpfr_class(vec2[i]);
    }

    benchmark_allocator_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    _ans = _Rdot(N, vec1_mpfr_class, 1, vec2_mpfr_class, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_allocator_counter::print_kernel("timed_kernel");

    mpfr_class ans = Rdot(N, vec1_mpfr_class, 1, vec2_mpfr_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(N) - 1.0) / elapsed_seconds.count() / MFLOPS << std::endl;

    mpfr_class _tmp = abs(_ans - ans);
    std::cout << "DIFF: ";
    mpfr_printf("%.4Rg ", _tmp.get_mpfr_t());
    if (_tmp < 1e-5)
        std::cout << "OK" << std::endl;
    else
        std::cout << "NG" << std::endl;

    clear_mpfr_vec(vec1, N);
    clear_mpfr_vec(vec2, N);
    delete[] vec1_mpfr_class;
    delete[] vec2_mpfr_class;
    mpfr_clear(tmp);
    mpfr_clear(dot_product);
    delete[] vec1;
    delete[] vec2;

    return 0;
}
