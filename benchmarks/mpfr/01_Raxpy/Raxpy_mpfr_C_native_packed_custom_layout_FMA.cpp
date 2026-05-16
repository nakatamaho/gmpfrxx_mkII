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

#include "Raxpy_common.hpp"

#include <cstddef>
#include <new>

gmp_randstate_t state;

namespace {

constexpr std::size_t cache_line_bytes = 64;

std::size_t round_up(std::size_t value, std::size_t alignment) {
    return ((value + alignment - 1) / alignment) * alignment;
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
    std::size_t n_;
    mpfr_prec_t prec_;
    std::size_t significand_bytes_;
    std::size_t significand_offset_;
    std::size_t stride_;
    std::size_t bytes_;
    void *buf_;
};

void init_packed_mpfr_vec(packed_mpfr_vec &vec) {
    for (std::size_t i = 0; i < vec.size(); ++i) {
        mpfr_urandomb(vec.at(i), state);
    }
}

mpfr_class l1_norm_difference(const packed_mpfr_vec &raw, const mpfr_class *wrapped) {
    mpfr_class l1_norm = 0;
    for (std::size_t i = 0; i < raw.size(); ++i) {
        l1_norm += abs(mpfr_class(raw.at(i)) - wrapped[i]);
    }
    return l1_norm;
}

void _Raxpy(int64_t n, const mpfr_t alpha, const packed_mpfr_vec &x, packed_mpfr_vec &y) {
    const mpfr_rnd_t rnd = mpfr_get_default_rounding_mode();

    for (int64_t i = 0; i < n; ++i) {
#if defined(__GNUC__) || defined(__clang__)
        if (i + 2 < n) {
            __builtin_prefetch(x.at(static_cast<std::size_t>(i + 2)), 0, 1);
            __builtin_prefetch(y.at(static_cast<std::size_t>(i + 2)), 1, 1);
        }
#endif
        mpfr_fma(y.at(static_cast<std::size_t>(i)), alpha, x.at(static_cast<std::size_t>(i)), y.at(static_cast<std::size_t>(i)), rnd);
    }
}

} // namespace

int main(int argc, char **argv) {
    gmp_randinit_default(state);
    gmp_randseed_ui(state, 42);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <vector size> <precision>" << std::endl;
        return EXIT_FAILURE;
    }

    const int64_t N = std::atoll(argv[1]);
    const int prec = std::atoi(argv[2]);
    if (N <= 0 || prec <= 0) {
        std::cerr << "Vector size and precision must be positive." << std::endl;
        return EXIT_FAILURE;
    }

    mpfr_set_default_prec(prec);
    mpfrxx::set_default_precision_bits(prec);

    packed_mpfr_vec x(static_cast<std::size_t>(N), prec);
    packed_mpfr_vec y(static_cast<std::size_t>(N), prec);
    mpfr_t alpha;
    mpfr_init2(alpha, prec);
    mpfr_urandomb(alpha, state);

    init_packed_mpfr_vec(x);
    init_packed_mpfr_vec(y);

    mpfr_class *x_mpfr_class = new mpfr_class[N];
    mpfr_class *y_mpfr_class = new mpfr_class[N];
    mpfr_class alpha_class = mpfr_class(alpha);

    for (int64_t i = 0; i < N; ++i) {
        x_mpfr_class[i] = mpfr_class(x.at(static_cast<std::size_t>(i)));
        y_mpfr_class[i] = mpfr_class(y.at(static_cast<std::size_t>(i)));
    }

    std::cout << "Packed significand bytes: " << x.significand_bytes() << std::endl;
    std::cout << "Packed element stride: " << x.stride() << " bytes" << std::endl;

    benchmark_mpfr_operation_counter::begin_kernel();
    auto start = std::chrono::high_resolution_clock::now();
    _Raxpy(N, alpha, x, y);
    auto end = std::chrono::high_resolution_clock::now();
    benchmark_mpfr_operation_counter::print_kernel("timed_kernel");

    Raxpy(N, alpha_class, x_mpfr_class, 1, y_mpfr_class, 1);

    std::chrono::duration<double> elapsed_seconds = end - start;
    const double mflops = (2.0 * double(N)) / (elapsed_seconds.count() * MFLOPS);

    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;
    std::cout << "MFLOPS: " << mflops << std::endl;
    print_l1_result(l1_norm_difference(y, y_mpfr_class));

    mpfr_clear(alpha);
    gmp_randclear(state);
    delete[] x_mpfr_class;
    delete[] y_mpfr_class;

    return EXIT_SUCCESS;
}
