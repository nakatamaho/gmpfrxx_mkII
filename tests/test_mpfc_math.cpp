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

#include <gmpxx_mkII.h>

#include <cmath>
#include <cstdlib>

namespace {

void require_close(double actual, double expected, double tolerance = 1e-10)
{
    if (std::abs(actual - expected) > tolerance) {
        std::abort();
    }
}

void require_close(const gmpxx::mpfc_class& actual, double real, double imag, double tolerance = 1e-10)
{
    require_close(actual.real().to_double(), real, tolerance);
    require_close(actual.imag().to_double(), imag, tolerance);
}

} // namespace

int main()
{
    auto i = gmpxx::mpfc_class::with_precision(192, 0.0, 1.0);
    auto one = gmpxx::mpfc_class::with_precision(192, 1.0, 0.0);
    auto z = gmpxx::mpfc_class::with_precision(192, 1.0, 1.0);

    require_close(gmpxx::sqrt(gmpxx::mpfc_class::with_precision(192, -1.0, 0.0)), 0.0, 1.0);
    require_close(gmpxx::exp(i), std::cos(1.0), std::sin(1.0));
    require_close(gmpxx::log(one), 0.0, 0.0);
    require_close(gmpxx::sin(i), 0.0, std::sinh(1.0));
    require_close(gmpxx::cos(i), std::cosh(1.0), 0.0);
    require_close(gmpxx::tan(gmpxx::mpfc_class::with_precision(192, 0.25, 0.0)), std::tan(0.25), 0.0);
    require_close(gmpxx::sinh(i), 0.0, std::sin(1.0));
    require_close(gmpxx::cosh(i), std::cos(1.0), 0.0);
    require_close(gmpxx::tanh(gmpxx::mpfc_class::with_precision(192, 0.25, 0.0)), std::tanh(0.25), 0.0);
    require_close(gmpxx::pow(z, 2), 0.0, 2.0);
    require_close(gmpxx::pow(one, z), 1.0, 0.0);
    require_close(gmpxx::gamma(one), 1.0, 0.0);

    auto expr_value = gmpxx::exp(z + one);
    require_close(expr_value, std::exp(2.0) * std::cos(1.0), std::exp(2.0) * std::sin(1.0), 1e-9);

    return 0;
}
