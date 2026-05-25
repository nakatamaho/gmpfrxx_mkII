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
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <new>

#include <gmp.h>

#include "mpfr_operation_counter.hpp"

#include "mpfrxx_mkII.h"
using namespace mpfrxx;

#include "Raxpy.hpp"

#define MFLOPS 1e+6

inline void init_mpfr_vec(mpfr_t *vec, int64_t n, int prec, gmp_randstate_t state) {
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

inline void require_mpfr_precision_at_least(const char *label, mpfr_prec_t actual, mpfr_prec_t requested) {
    if (actual >= requested) {
        return;
    }
    std::cerr << "Precision check failed for " << label << ": requested at least " << requested
              << " bits, actual " << actual << " bits" << std::endl;
    std::exit(EXIT_FAILURE);
}

inline mpfr_prec_t class_precision_bits(const mpfr_class &value) {
    return mpfr_get_prec(value.get_mpfr_t());
}

class packed_mpfr_vec {
public:
    packed_mpfr_vec(std::size_t n, mpfr_prec_t prec)
        : n_(n),
          prec_(prec),
          significand_bytes_(static_cast<std::size_t>(mpfr_custom_get_size(prec))),
          significand_offset_(round_up(sizeof(__mpfr_struct), alignof(mp_limb_t))),
          stride_(round_up(significand_offset_ + significand_bytes_, alignof(__mpfr_struct))),
          bytes_(n_ * stride_),
          buf_(::operator new(bytes_, std::align_val_t(cache_line_bytes))) {
        for (std::size_t i = 0; i < n_; ++i) {
            mpfr_ptr value = at(i);
            void *limbs = static_cast<void *>(limbs_at(i));
            mpfr_custom_init(limbs, prec_);
            mpfr_custom_init_set(value, MPFR_ZERO_KIND, 0, prec_, limbs);
        }
    }

    packed_mpfr_vec(const packed_mpfr_vec &) = delete;
    packed_mpfr_vec &operator=(const packed_mpfr_vec &) = delete;

    ~packed_mpfr_vec() {
        // Do not call mpfr_clear(): the significands live inside buf_.
        ::operator delete(buf_, std::align_val_t(cache_line_bytes));
    }

    mpfr_ptr at(std::size_t i) const {
        return reinterpret_cast<mpfr_ptr>(static_cast<unsigned char *>(buf_) + i * stride_);
    }

    mp_limb_t *limbs_at(std::size_t i) const {
        return reinterpret_cast<mp_limb_t *>(static_cast<unsigned char *>(buf_) + i * stride_ + significand_offset_);
    }

    std::size_t size() const {
        return n_;
    }

    mpfr_prec_t precision() const {
        return prec_;
    }

    std::size_t significand_bytes() const {
        return significand_bytes_;
    }

    std::size_t stride() const {
        return stride_;
    }

private:
    static constexpr std::size_t cache_line_bytes = 64;

    static std::size_t round_up(std::size_t value, std::size_t alignment) {
        return ((value + alignment - 1) / alignment) * alignment;
    }

    std::size_t n_;
    mpfr_prec_t prec_;
    std::size_t significand_bytes_;
    std::size_t significand_offset_;
    std::size_t stride_;
    std::size_t bytes_;
    void *buf_;
};

inline void init_packed_mpfr_vec(packed_mpfr_vec &vec, gmp_randstate_t state) {
    for (std::size_t i = 0; i < vec.size(); ++i) {
        mpfr_urandomb(vec.at(i), state);
    }
}

inline mpfr_class l1_norm_difference(const packed_mpfr_vec &raw, const mpfr_class *wrapped) {
    mpfr_class l1_norm = 0;
    for (std::size_t i = 0; i < raw.size(); ++i) {
        l1_norm += abs(mpfr_class(raw.at(i)) - wrapped[i]);
    }
    return l1_norm;
}

inline int run_native_raxpy_benchmark(int argc, char **argv,
                                      void (*kernel)(int64_t, const mpfr_t, mpfr_t *, int64_t, mpfr_t *, int64_t)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t n = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mpfr_prec_t requested_prec = static_cast<mpfr_prec_t>(prec);
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    mpfr_t *x = new mpfr_t[n];
    mpfr_t *y = new mpfr_t[n];
    mpfr_t alpha;
    mpfr_init2(alpha, prec);
    mpfr_urandomb(alpha, state);

    init_mpfr_vec(x, n, prec, state);
    init_mpfr_vec(y, n, prec, state);

    mpfr_class *x_class = new mpfr_class[n];
    mpfr_class *y_class = new mpfr_class[n];
    mpfr_class alpha_class = mpfr_class(alpha);

    for (int64_t i = 0; i < n; ++i) {
        x_class[i] = mpfr_class(x[i]);
        y_class[i] = mpfr_class(y[i]);
    }

    require_mpfr_precision_at_least("raw_alpha", mpfr_get_prec(alpha), requested_prec);
    require_mpfr_precision_at_least("class_alpha", class_precision_bits(alpha_class), requested_prec);
    if (n > 0) {
        require_mpfr_precision_at_least("raw_x[0]", mpfr_get_prec(x[0]), requested_prec);
        require_mpfr_precision_at_least("raw_y[0]", mpfr_get_prec(y[0]), requested_prec);
        require_mpfr_precision_at_least("class_x[0]", class_precision_bits(x_class[0]), requested_prec);
        require_mpfr_precision_at_least("class_y[0]", class_precision_bits(y_class[0]), requested_prec);
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(n, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Raxpy(n, alpha_class, x_class, 1, y_class, 1);
    if (n > 0) {
        require_mpfr_precision_at_least("raw_y_after[0]", mpfr_get_prec(y[0]), requested_prec);
        require_mpfr_precision_at_least("class_y_reference[0]", class_precision_bits(y_class[0]), requested_prec);
    }

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, y_class, n));

    clear_mpfr_vec(x, n);
    clear_mpfr_vec(y, n);
    mpfr_clear(alpha);
    delete[] x;
    delete[] y;
    delete[] x_class;
    delete[] y_class;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

inline int run_class_raxpy_benchmark(int argc, char **argv,
                                     void (*kernel)(int64_t, const mpfr_class &, mpfr_class *, int64_t, mpfr_class *, int64_t)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t n = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    if (prec <= 0) {
        std::cerr << "Precision must be positive: " << prec << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mpfr_prec_t requested_prec = static_cast<mpfr_prec_t>(prec);
    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    mpfr_t alpha_raw;
    mpfr_init2(alpha_raw, prec);
    mpfr_urandomb(alpha_raw, state);

    mpfr_t *x_raw = new mpfr_t[n];
    mpfr_t *y_raw = new mpfr_t[n];
    init_mpfr_vec(x_raw, n, prec, state);
    init_mpfr_vec(y_raw, n, prec, state);

    mpfr_class *x = new mpfr_class[n];
    mpfr_class *y = new mpfr_class[n];
    mpfr_class *yy = new mpfr_class[n];
    mpfr_class alpha = mpfr_class(alpha_raw);

    for (int64_t i = 0; i < n; ++i) {
        x[i] = mpfr_class(x_raw[i]);
        y[i] = mpfr_class(y_raw[i]);
        yy[i] = y[i];
    }

    require_mpfr_precision_at_least("raw_alpha", mpfr_get_prec(alpha_raw), requested_prec);
    require_mpfr_precision_at_least("class_alpha", class_precision_bits(alpha), requested_prec);
    if (n > 0) {
        require_mpfr_precision_at_least("raw_x[0]", mpfr_get_prec(x_raw[0]), requested_prec);
        require_mpfr_precision_at_least("raw_y[0]", mpfr_get_prec(y_raw[0]), requested_prec);
        require_mpfr_precision_at_least("class_x[0]", class_precision_bits(x[0]), requested_prec);
        require_mpfr_precision_at_least("class_y[0]", class_precision_bits(y[0]), requested_prec);
        require_mpfr_precision_at_least("class_yy[0]", class_precision_bits(yy[0]), requested_prec);
    }

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(n, alpha, x, 1, y, 1);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Raxpy(n, alpha, x, 1, yy, 1);
    if (n > 0) {
        require_mpfr_precision_at_least("class_y_after[0]", class_precision_bits(y[0]), requested_prec);
        require_mpfr_precision_at_least("class_yy_reference[0]", class_precision_bits(yy[0]), requested_prec);
    }

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, yy, n));

    clear_mpfr_vec(x_raw, n);
    clear_mpfr_vec(y_raw, n);
    mpfr_clear(alpha_raw);
    delete[] x_raw;
    delete[] y_raw;
    delete[] x;
    delete[] y;
    delete[] yy;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

inline int run_packed_native_raxpy_benchmark(int argc, char **argv,
                                             void (*kernel)(int64_t, const mpfr_t, const packed_mpfr_vec &, packed_mpfr_vec &)) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    const int64_t n = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    if (n <= 0 || prec <= 0) {
        std::cerr << "Vector size and precision must be positive." << std::endl;
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    const mpfr_prec_t requested_prec = static_cast<mpfr_prec_t>(prec);

    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    packed_mpfr_vec x(static_cast<std::size_t>(n), prec);
    packed_mpfr_vec y(static_cast<std::size_t>(n), prec);
    mpfr_t alpha;
    mpfr_init2(alpha, prec);
    mpfr_urandomb(alpha, state);

    init_packed_mpfr_vec(x, state);
    init_packed_mpfr_vec(y, state);

    mpfr_class *x_class = new mpfr_class[n];
    mpfr_class *y_class = new mpfr_class[n];
    mpfr_class alpha_class = mpfr_class(alpha);

    for (int64_t i = 0; i < n; ++i) {
        x_class[i] = mpfr_class(x.at(static_cast<std::size_t>(i)));
        y_class[i] = mpfr_class(y.at(static_cast<std::size_t>(i)));
    }

    require_mpfr_precision_at_least("raw_alpha", mpfr_get_prec(alpha), requested_prec);
    require_mpfr_precision_at_least("packed_x[0]", mpfr_get_prec(x.at(0)), requested_prec);
    require_mpfr_precision_at_least("packed_y[0]", mpfr_get_prec(y.at(0)), requested_prec);
    require_mpfr_precision_at_least("class_alpha", class_precision_bits(alpha_class), requested_prec);
    require_mpfr_precision_at_least("class_x[0]", class_precision_bits(x_class[0]), requested_prec);
    require_mpfr_precision_at_least("class_y[0]", class_precision_bits(y_class[0]), requested_prec);

    std::cout << "Packed significand bytes: " << x.significand_bytes() << std::endl;
    std::cout << "Packed element stride: " << x.stride() << " bytes" << std::endl;

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    kernel(n, alpha, x, y);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Raxpy(n, alpha_class, x_class, 1, y_class, 1);
    require_mpfr_precision_at_least("packed_y_after[0]", mpfr_get_prec(y.at(0)), requested_prec);
    require_mpfr_precision_at_least("class_y_reference[0]", class_precision_bits(y_class[0]), requested_prec);

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(n)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, y_class));

    mpfr_clear(alpha);
    delete[] x_class;
    delete[] y_class;
    gmp_randclear(state);

    return EXIT_SUCCESS;
}

#endif // GMPFRXX_MKII_BENCHMARK_MPFR_RAXPY_COMMON_HPP
