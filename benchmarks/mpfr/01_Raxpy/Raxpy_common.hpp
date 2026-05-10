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

#ifndef GMPFRXX_MKII_BENCHMARK_MPFR_RAXPY_COMMON_HPP
#define GMPFRXX_MKII_BENCHMARK_MPFR_RAXPY_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <gmp.h>

#include "mpfr_operation_counter.hpp"

#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Raxpy.hpp"

#define MFLOPS 1e+6

extern gmp_randstate_t state;

inline void init_mpfr_vec(mpfr_t *vec, int64_t n, int prec) {
    for (int64_t i = 0; i < n; ++i) {
        mpfr_init2(vec[i], prec);
        mpfr_urandomb(vec[i], state);
    }
}

inline void clear_mpfr_vec(mpfr_t *vec, int64_t n) {
    for (int64_t i = 0; i < n; ++i) {
        mpfr_clear(vec[i]);
    }
}

inline mpfr_class l1_norm_difference(const mpfr_t *raw, const mpfr_class *wrapped, int64_t n) {
    mpfr_class l1_norm = 0;
    for (int64_t i = 0; i < n; ++i) {
        l1_norm += abs(mpfr_class(raw[i]) - wrapped[i]);
    }
    return l1_norm;
}

inline mpfr_class l1_norm_difference(const mpfr_class *lhs, const mpfr_class *rhs, int64_t n) {
    mpfr_class l1_norm = 0;
    for (int64_t i = 0; i < n; ++i) {
        l1_norm += abs(lhs[i] - rhs[i]);
    }
    return l1_norm;
}

inline void print_l1_result(const mpfr_class &l1_norm) {
    std::cout << "L1 Norm of difference: ";
    mpfr_printf("%.4Rg\n", l1_norm.get_mpfr_t());

    if (l1_norm < 1e-5) {
        std::cout << "Result OK" << std::endl;
    } else {
        std::cout << "Result NG" << std::endl;
    }
}

#endif // GMPFRXX_MKII_BENCHMARK_MPFR_RAXPY_COMMON_HPP
