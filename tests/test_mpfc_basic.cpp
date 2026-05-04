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
#include <sstream>

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
    auto a = gmpxx::mpfc_class::with_precision(128, 1.0, 2.0);
    auto b = gmpxx::mpfc_class::with_precision(128, 3.0, 4.0);
    auto r = gmpxx::mpf_class::with_precision(128, 5.0);

    gmpxx::mpfc_class result = a + b;
    require_close(result.real().to_double(), 4.0);
    require_close(result.imag().to_double(), 6.0);

    result = a * b;
    require_close(result.real().to_double(), -5.0);
    require_close(result.imag().to_double(), 10.0);

    result = b / a;
    require_close(result.real().to_double(), 2.2);
    require_close(result.imag().to_double(), -0.4);

    result = r + a * 2;
    require_close(result.real().to_double(), 7.0);
    require_close(result.imag().to_double(), 4.0);

    result = -a;
    require_close(result.real().to_double(), -1.0);
    require_close(result.imag().to_double(), -2.0);

    std::ostringstream out;
    out << a;
    if (out.str().find(',') == std::string::npos) {
        std::abort();
    }

    return 0;
}
