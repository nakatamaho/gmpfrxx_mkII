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
        printf("Not supported, exitting\n");
        exit(-1);
    }

    const mpfr_prec_t precision = n > 0 ? dx[0].precision() : mpfrxx::default_precision_bits();
    const mpfr_rnd_t rounding = mpfrxx::default_rounding_mode();
    mpfr_class temp(0.0, precision);

// no reduction for multiple precision
#ifdef _OPENMP
#pragma omp parallel shared(temp, dx, dy, n, precision, rounding)
#endif
    {
        mpfr_class templ(0.0, precision);
        auto templ_context = mpfrxx::with_context(templ, precision, rounding);
#ifdef _OPENMP
#pragma omp for
#endif
        for (int64_t i = 0; i < n; i++) {
            templ_context += dx[i] * dy[i];
        }
#ifdef _OPENMP
#pragma omp critical
#endif
        {
            auto temp_context = mpfrxx::with_context(temp, precision, rounding);
            temp_context += templ;
        }
    }
    return temp;
}

int main(int argc, char **argv) {
    return run_class_rdot_benchmark(argc, argv, _Rdot);
}
