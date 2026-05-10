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

#include <iostream>
#include <chrono>
#include <gmp.h>

#include "mpfr_operation_counter.hpp"

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

    int64_t i;

    mpfr_class temp, templ;
    temp = 0.0;
    for (i = 0; i < n; i++) {
        templ = dx[i];
        templ *= dy[i];
        temp += templ;
    }
    return temp;
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
    mpfr_class _ans;

    for (int i = 0; i < N; i++) {
        vec1_mpfr_class[i] = mpfr_class(vec1[i]);
        vec2_mpfr_class[i] = mpfr_class(vec2[i]);
    }
    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    _ans = _Rdot(N, vec1_mpfr_class, 1, vec2_mpfr_class, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    mpfr_class ans = Rdot(N, vec1_mpfr_class, 1, vec2_mpfr_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(N) - 1.0) / elapsed_seconds.count() / MFLOPS << std::endl;

    mpfr_class _tmp;
    _tmp = abs(_ans - ans);
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
