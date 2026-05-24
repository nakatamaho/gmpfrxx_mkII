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

void _Rdot(int64_t n, mpf_t *dx, int64_t incx, mpf_t *dy, int64_t incy, mpf_t *ans) {
    if (incx != 1 || incy != 1) {
        std::cerr << "Increments other than 1 are not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    const mp_bitcnt_t precision = mpf_get_prec(*ans);
    mpf_t temp, templ;

    mpf_set_d(*ans, 0.0);

// no reduction for multiple precision
#pragma omp parallel private(temp, templ)
    {
        mpf_init2(temp, precision);
        mpf_init2(templ, precision);
        mpf_set_d(temp, 0.0);
        mpf_set_d(templ, 0.0);

#pragma omp for
        for (int64_t i = 0; i < n; i++) {
            mpf_set(templ, dx[i]);
            mpf_mul(templ, templ, dy[i]);
            mpf_add(temp, temp, templ);
        }
#pragma omp critical
        { mpf_add(*ans, *ans, temp); }
        mpf_clear(temp);
        mpf_clear(templ);
    }
}

int main(int argc, char **argv) {
    return rdot_gmp_bench::run_native_rdot_benchmark(argc, argv, _Rdot);
}
