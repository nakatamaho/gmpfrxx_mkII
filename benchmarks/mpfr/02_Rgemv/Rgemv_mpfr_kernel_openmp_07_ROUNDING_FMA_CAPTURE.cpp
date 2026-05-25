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

#include <omp.h>

#include <vector>

#include "Rgemv_common.hpp"

void _Rgemv(int64_t m, int64_t n, const mpfr_class &alpha, const mpfr_class *A, int64_t lda, const mpfr_class *x, int64_t incx, const mpfr_class &beta, mpfr_class *y, int64_t incy) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const mpfr_prec_t precision = m > 0 ? y[0].precision() : mpfrxx::default_precision_bits();
    const mpfr_rnd_t rounding = mpfrxx::default_rounding_mode();
    const mpfrxx::evaluation_context context{precision, rounding};
    const int64_t num_threads = static_cast<int64_t>(omp_get_max_threads());
    const int64_t partial_count = num_threads * m;
    std::vector<mpfr_class> partials;
    partials.reserve(static_cast<std::size_t>(partial_count));
    for (int64_t i = 0; i < partial_count; ++i) {
        partials.emplace_back(0.0, precision);
    }

#pragma omp parallel
    {
        const int64_t tid = static_cast<int64_t>(omp_get_thread_num());
        mpfr_class *local_y = partials.data() + tid * m;

#pragma omp for schedule(static)
        for (int64_t i = 0; i < m; ++i) {
            auto y_context = mpfrxx::with_context(y[i], context);
            y_context *= beta;
        }

        for (int64_t i = 0; i < m; ++i) {
            auto local_context = mpfrxx::with_context(local_y[i], context);
            local_context = 0.0;
        }

        mpfr_class temp(0.0, precision);
        auto temp_context = mpfrxx::with_context(temp, context);

#pragma omp for schedule(static)
        for (int64_t j = 0; j < n; ++j) {
            temp_context = alpha * x[j];
            for (int64_t i = 0; i < m; ++i) {
                auto local_context = mpfrxx::with_context(local_y[i], context);
                local_context += temp * A[i + j * lda];
            }
        }

#pragma omp for schedule(static)
        for (int64_t i = 0; i < m; ++i) {
            auto y_context = mpfrxx::with_context(y[i], context);
            for (int64_t t = 0; t < num_threads; ++t) {
                y_context += partials[t * m + i];
            }
        }
    }
}

int main(int argc, char **argv) {
    return run_class_rgemv_benchmark(argc, argv, _Rgemv);
}
