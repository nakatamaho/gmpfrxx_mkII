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

#include "Rdot_common.hpp"

mpfr_class _Rdot(int64_t n, mpfr_class *dx, int64_t incx, mpfr_class *dy, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const mpfr_prec_t precision = n > 0 ? dx[0].precision() : mpfrxx::default_precision_bits();
    const mpfr_rnd_t rounding = mpfrxx::default_rounding_mode();
    mpfr_class result(0.0, precision);
    const int64_t unrolled_n = n - n % 4;

#pragma omp parallel
    {
        mpfr_class acc0(0.0, precision);
        mpfr_class acc1(0.0, precision);
        mpfr_class acc2(0.0, precision);
        mpfr_class acc3(0.0, precision);
        auto acc0_rounding = mpfrxx::with_rounding(acc0, rounding);
        auto acc1_rounding = mpfrxx::with_rounding(acc1, rounding);
        auto acc2_rounding = mpfrxx::with_rounding(acc2, rounding);
        auto acc3_rounding = mpfrxx::with_rounding(acc3, rounding);

#pragma omp for schedule(static)
        for (int64_t i = 0; i < unrolled_n; i += 4) {
            acc0_rounding += dx[i] * dy[i];
            acc1_rounding += dx[i + 1] * dy[i + 1];
            acc2_rounding += dx[i + 2] * dy[i + 2];
            acc3_rounding += dx[i + 3] * dy[i + 3];
        }

#pragma omp for schedule(static)
        for (int64_t i = unrolled_n; i < n; ++i) {
            acc0_rounding += dx[i] * dy[i];
        }

        acc0_rounding += acc1;
        acc2_rounding += acc3;
        acc0_rounding += acc2;

#pragma omp critical
        {
            auto result_rounding = mpfrxx::with_rounding(result, rounding);
            result_rounding += acc0;
        }
    }

    return result;
}

int main(int argc, char **argv) {
    return run_class_rdot_benchmark(argc, argv, _Rdot);
}
