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

namespace {

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(128, 3.0, -1.0);

    z = z * w + z;
    require_close(z.real_to_double(), 6.0);
    require_close(z.imag_to_double(), 7.0);

    auto add_alias = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    add_alias = add_alias + w;
    require_close(add_alias.real_to_double(), 4.0);
    require_close(add_alias.imag_to_double(), 1.0);

    auto rhs_alias = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    rhs_alias = w + rhs_alias;
    require_close(rhs_alias.real_to_double(), 4.0);
    require_close(rhs_alias.imag_to_double(), 1.0);

    auto mul_alias = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    mul_alias = mul_alias * w;
    require_close(mul_alias.real_to_double(), 5.0);
    require_close(mul_alias.imag_to_double(), 5.0);

    auto div_alias = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    div_alias = div_alias / w;
    require_close(div_alias.real_to_double(), 0.1);
    require_close(div_alias.imag_to_double(), 0.7);

    auto direct_leaf = mpfrxx::mpc_class::with_precision(128, 0.0, 0.0);
    direct_leaf = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0) + w;
    require_close(direct_leaf.real_to_double(), 4.0);
    require_close(direct_leaf.imag_to_double(), 1.0);

    return 0;
}
