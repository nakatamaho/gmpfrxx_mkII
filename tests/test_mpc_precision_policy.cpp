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

#include <mpfrxx_mkII.h>
#include <mpcxx_mkII.h>

#include <cmath>
#include <cstdlib>
#include <utility>

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(96, 128, 1.0, 2.0);
    auto r = mpfrxx::mpfr_class::with_precision(160, 3.0);

    if (z.precision() != 128) {
        std::abort();
    }

    mpfrxx::mpc_class materialized = z + r;
    if (materialized.real_precision() != 160 || materialized.imag_precision() != 128) {
        std::abort();
    }

    auto dst = mpfrxx::mpc_class::with_precision(80, 112);
    dst = z + r;
    if (dst.real_precision() != 80 || dst.imag_precision() != 112) {
        std::abort();
    }

    auto a = mpfrxx::mpc_class::with_precision(96, 128, 1.0, 2.0);
    auto b = mpfrxx::mpc_class::with_precision(96, 128, 3.0, -1.0);
    auto c = mpfrxx::mpc_class::with_precision(96, 128, -0.5, 4.0);
    auto chain_dst = mpfrxx::mpc_class::with_precision(80, 112);
    chain_dst = a + b + c;
    if (chain_dst.real_precision() != 80 || chain_dst.imag_precision() != 112) {
        std::abort();
    }
    if (std::abs(chain_dst.real_to_double() - 3.5) > 1e-12 ||
        std::abs(chain_dst.imag_to_double() - 5.0) > 1e-12) {
        std::abort();
    }

    auto add_assign = a;
    add_assign += b;
    if (add_assign.real_precision() != a.real_precision() ||
        add_assign.imag_precision() != a.imag_precision() ||
        std::abs(add_assign.real_to_double() - 4.0) > 1e-12 ||
        std::abs(add_assign.imag_to_double() - 1.0) > 1e-12) {
        std::abort();
    }

    auto sub_assign = a;
    sub_assign -= b;
    if (sub_assign.real_precision() != a.real_precision() ||
        sub_assign.imag_precision() != a.imag_precision() ||
        std::abs(sub_assign.real_to_double() + 2.0) > 1e-12 ||
        std::abs(sub_assign.imag_to_double() - 3.0) > 1e-12) {
        std::abort();
    }

    auto mul_assign = a;
    mul_assign *= b;
    if (mul_assign.real_precision() != a.real_precision() ||
        mul_assign.imag_precision() != a.imag_precision() ||
        std::abs(mul_assign.real_to_double() - 5.0) > 1e-12 ||
        std::abs(mul_assign.imag_to_double() - 5.0) > 1e-12) {
        std::abort();
    }

    auto div_assign = a;
    div_assign /= b;
    if (div_assign.real_precision() != a.real_precision() ||
        div_assign.imag_precision() != a.imag_precision() ||
        std::abs(div_assign.real_to_double() - 0.1) > 1e-12 ||
        std::abs(div_assign.imag_to_double() - 0.7) > 1e-12) {
        std::abort();
    }

    auto expr_assign = a;
    expr_assign += b + c;
    if (expr_assign.real_precision() != a.real_precision() ||
        expr_assign.imag_precision() != a.imag_precision() ||
        std::abs(expr_assign.real_to_double() - 3.5) > 1e-12 ||
        std::abs(expr_assign.imag_to_double() - 5.0) > 1e-12) {
        std::abort();
    }

    auto move_same_prec = mpfrxx::mpc_class::with_precision(96, 128);
    auto move_same_source = mpfrxx::mpc_class::with_precision(96, 128, 7.0, -8.0);
    move_same_prec = std::move(move_same_source);
    if (move_same_prec.real_precision() != 96 ||
        move_same_prec.imag_precision() != 128 ||
        std::abs(move_same_prec.real_to_double() - 7.0) > 1e-12 ||
        std::abs(move_same_prec.imag_to_double() + 8.0) > 1e-12) {
        std::abort();
    }

    auto move_dst = mpfrxx::mpc_class::with_precision(192, 224);
    auto move_lower_prec = mpfrxx::mpc_class::with_precision(80, 112, 1.25, -2.5);
    move_dst = std::move(move_lower_prec);
    if (move_dst.real_precision() != 192 ||
        move_dst.imag_precision() != 224 ||
        std::abs(move_dst.real_to_double() - 1.25) > 1e-12 ||
        std::abs(move_dst.imag_to_double() + 2.5) > 1e-12) {
        std::abort();
    }

    return 0;
}
