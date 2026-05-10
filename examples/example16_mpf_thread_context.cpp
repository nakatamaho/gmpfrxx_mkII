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
 * Example 16: thread-local MPF default precision with AGM pi.
 *
 * This is the Gauss-Legendre / Brent-Salamin pi iteration from example04_mpf,
 * run in two worker threads with different wrapper default precisions.  The
 * executable is linked with the optional default-context provider, so
 * gmpxx::default_mpf_precision_guard changes only the calling thread's wrapper
 * default precision and does not call GMP's process-global mpf_set_default_prec.
 */

#include <gmpxx_mkII.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

namespace {

int decimal_digits_for_bits(mp_bitcnt_t bits)
{
    return static_cast<int>(
        std::floor(static_cast<double>(bits) * std::log10(2.0)));
}

struct pi_result {
    std::string name;
    mp_bitcnt_t requested_precision;
    mp_bitcnt_t default_precision;
    mp_bitcnt_t result_precision;
    int iterations;
    gmpxx::mpf_class pi;
};

pi_result compute_pi_in_thread(const char* name, mp_bitcnt_t precision)
{
    gmpxx::default_mpf_precision_guard guard(precision);

    gmpxx::mpf_class one("1.0");
    gmpxx::mpf_class two("2.0");
    gmpxx::mpf_class four("4.0");
    gmpxx::mpf_class a = one;
    gmpxx::mpf_class b = one / gmpxx::sqrt(two);
    gmpxx::mpf_class t("0.25");
    gmpxx::mpf_class p = one;
    gmpxx::mpf_class pi("0.0");
    gmpxx::mpf_class previous_pi("0.0");
    gmpxx::mpf_class tolerance =
        gmpxx::exp2(-gmpxx::mpf_class(gmpxx::default_mpf_precision_bits() / 2));

    int iteration = 0;
    do {
        previous_pi = pi;

        gmpxx::mpf_class next_a = (a + b) / two;
        gmpxx::mpf_class next_b = gmpxx::sqrt(a * b);
        gmpxx::mpf_class delta = a - next_a;
        t = t - p * delta * delta;
        p = two * p;
        a = next_a;
        b = next_b;

        gmpxx::mpf_class sum = a + b;
        pi = sum * sum / (four * t);

        ++iteration;
    } while (iteration == 1 || gmpxx::abs(pi - previous_pi) > tolerance);

    pi_result result{};
    result.name = name;
    result.requested_precision = precision;
    result.default_precision = gmpxx::default_mpf_precision_bits();
    result.result_precision = pi.precision();
    result.iterations = iteration;
    result.pi = pi;
    return result;
}

void print_result(const pi_result& result)
{
    const int digits = std::min(decimal_digits_for_bits(result.result_precision), 80);
    std::cout << result.name
              << " requested=" << result.requested_precision
              << " default=" << result.default_precision
              << " result_prec=" << result.result_precision
              << " iterations=" << result.iterations
              << '\n'
              << "  pi = " << std::fixed << std::setprecision(digits)
              << result.pi << '\n';
}

} // namespace

int main()
{
    pi_result low;
    pi_result high;

    std::thread thread_low([&] {
        low = compute_pi_in_thread("thread-a", 160);
    });
    std::thread thread_high([&] {
        high = compute_pi_in_thread("thread-b", 768);
    });

    thread_low.join();
    thread_high.join();

    print_result(low);
    print_result(high);

    if (low.default_precision != 160 ||
        high.default_precision != 768 ||
        low.result_precision < 160 ||
        high.result_precision < 768) {
        std::abort();
    }

    gmpxx::mpf_class main_thread_value;
    std::cout << "main thread default=" << gmpxx::default_mpf_precision_bits()
              << " value_prec=" << main_thread_value.precision()
              << '\n';

    return 0;
}
