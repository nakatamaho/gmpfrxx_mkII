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

/*
 * Example 04: Gauss-Legendre / Brent-Salamin iteration for pi.
 *
 * The variables a, b, t, and p implement the quadratically convergent
 * arithmetic-geometric-mean formula for pi.  Each iteration roughly doubles
 * the number of correct digits when the working precision is high enough.
 * The example is useful for gmpxx_mkII because it stresses sqrt(), division,
 * expression-template temporaries, and repeated assignment at a fixed
 * precision.
 *
 * First modern publications:
 *
 * - Eugene Salamin, "Computation of pi using arithmetic-geometric mean",
 *   Mathematics of Computation 30(135), 565-570, 1976.
 *   DOI: 10.1090/S0025-5718-1976-0404124-9
 * - Richard P. Brent, "Fast multiple-precision evaluation of elementary
 *   functions", Journal of the ACM 23(2), 242-251, 1976.
 *   DOI: 10.1145/321941.321944
 */

#include <gmpxx_mkII.h>

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {

int decimal_digits_for_bits(mp_bitcnt_t bits)
{
    return static_cast<int>(
        std::floor(static_cast<double>(bits) * std::log10(2.0)));
}

} // namespace

int main()
{
    using namespace gmpxx;
    const int decimal_digits =
        decimal_digits_for_bits(default_mpf_precision_bits());

    mpf_class one("1.0");
    mpf_class two("2.0");
    mpf_class four("4.0");
    mpf_class a = one;
    mpf_class b = one / sqrt(two);
    mpf_class t("0.25");
    mpf_class p = one;
    mpf_class pi("0.0");
    mpf_class previous_pi("0.0");
    mpf_class tolerance =
        exp2(-mpf_class(default_mpf_precision_bits() / 2));

    std::cout << std::fixed << std::setprecision(decimal_digits);
    std::cout << "Gauss-Legendre iteration for pi\n";

    int iteration = 0;
    do {
        previous_pi = pi;

        mpf_class next_a = (a + b) / two;
        mpf_class next_b = sqrt(a * b);
        mpf_class delta = a - next_a;
        t = t - p * delta * delta;
        p = two * p;
        a = next_a;
        b = next_b;

        mpf_class sum = a + b;
        pi = sum * sum / (four * t);

        ++iteration;
        std::cout << "iteration " << std::setw(2) << iteration
                  << ": " << pi << '\n';
    } while (iteration == 1 || abs(pi - previous_pi) > tolerance);

    std::cout << "const_pi() result: " << const_pi() << '\n';
    return 0;
}
