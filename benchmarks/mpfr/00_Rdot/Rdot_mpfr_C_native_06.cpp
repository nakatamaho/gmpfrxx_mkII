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

#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Rdot.hpp"

#define MFLOPS 1e+6

gmp_randstate_t state;

void _Rdot(int64_t n, mpfr_t *dx, int64_t incx, mpfr_t *dy, int64_t incy, mpfr_t *ans) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mpfr_prec_t precision = mpfr_get_prec(*ans);
    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();
    const int64_t unrolled_n = n - (n % 4);
    mpfr_t acc0, acc1, acc2, acc3, templ0, templ1, templ2, templ3;

    mpfr_set_d(*ans, 0.0, rnd);
    mpfr_init2(acc0, precision);
    mpfr_init2(acc1, precision);
    mpfr_init2(acc2, precision);
    mpfr_init2(acc3, precision);
    mpfr_init2(templ0, precision);
    mpfr_init2(templ1, precision);
    mpfr_init2(templ2, precision);
    mpfr_init2(templ3, precision);
    mpfr_set_d(acc0, 0.0, rnd);
    mpfr_set_d(acc1, 0.0, rnd);
    mpfr_set_d(acc2, 0.0, rnd);
    mpfr_set_d(acc3, 0.0, rnd);
    mpfr_set_d(templ0, 0.0, rnd);
    mpfr_set_d(templ1, 0.0, rnd);
    mpfr_set_d(templ2, 0.0, rnd);
    mpfr_set_d(templ3, 0.0, rnd);

    int64_t i = 0;
    for (; i < unrolled_n; i += 4) {
        mpfr_mul(templ0, dx[i], dy[i], rnd);
        mpfr_mul(templ1, dx[i + 1], dy[i + 1], rnd);
        mpfr_mul(templ2, dx[i + 2], dy[i + 2], rnd);
        mpfr_mul(templ3, dx[i + 3], dy[i + 3], rnd);

        mpfr_add(acc0, acc0, templ0, rnd);
        mpfr_add(acc1, acc1, templ1, rnd);
        mpfr_add(acc2, acc2, templ2, rnd);
        mpfr_add(acc3, acc3, templ3, rnd);
    }

    for (; i < n; ++i) {
        mpfr_mul(templ0, dx[i], dy[i], rnd);
        mpfr_add(acc0, acc0, templ0, rnd);
    }

    mpfr_add(acc0, acc0, acc1, rnd);
    mpfr_add(acc2, acc2, acc3, rnd);
    mpfr_add(acc0, acc0, acc2, rnd);
    mpfr_swap(*ans, acc0);

    mpfr_clear(templ3);
    mpfr_clear(templ2);
    mpfr_clear(templ1);
    mpfr_clear(templ0);
    mpfr_clear(acc3);
    mpfr_clear(acc2);
    mpfr_clear(acc1);
    mpfr_clear(acc0);
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
    mpfr_t _ans, dot_product;

    mpfr_init2(dot_product, prec);
    mpfr_init2(_ans, prec);
    init_mpfr_vec(vec1, N, prec);
    init_mpfr_vec(vec2, N, prec);

    mpfr_class *vec1_mpfr_class = new mpfr_class[N];
    mpfr_class *vec2_mpfr_class = new mpfr_class[N];
    mpfr_class ans;

    for (int i = 0; i < N; i++) {
        vec1_mpfr_class[i] = mpfr_class(vec1[i]);
        vec2_mpfr_class[i] = mpfr_class(vec2[i]);
    }

    auto start = std::chrono::high_resolution_clock::now();
    _Rdot(N, vec1, 1, vec2, 1, &_ans);
    auto end = std::chrono::high_resolution_clock::now();

    ans = Rdot(N, vec1_mpfr_class, 1, vec2_mpfr_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << (2.0 * double(N) - 1.0) / elapsed_seconds.count() / MFLOPS << std::endl;

    mpfr_class _tmp = abs(mpfr_class(_ans) - ans);
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
    mpfr_clear(dot_product);
    mpfr_clear(_ans);
    delete[] vec1;
    delete[] vec2;

    return 0;
}
