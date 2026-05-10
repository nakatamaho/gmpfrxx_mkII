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
 * Example 16 MPFR variant: thread-local default precision with AGM pi.
 *
 * This is the Gauss-Legendre / Brent-Salamin pi iteration from example04_mpfr,
 * run in two worker threads with different MPFR default precisions.  Unlike the
 * GMP MPF provider example, this uses libmpfr's default precision state
 * directly through the wrapper API.
 */

#include <mpfrxx_mkII.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {

class default_precision_guard {
public:
    explicit default_precision_guard(mpfr_prec_t precision) noexcept
        : old_precision_(mpfrxx::default_precision_bits())
    {
        mpfrxx::set_default_precision_bits(precision);
    }

    ~default_precision_guard()
    {
        mpfrxx::set_default_precision_bits(old_precision_);
    }

    default_precision_guard(const default_precision_guard&) = delete;
    default_precision_guard& operator=(const default_precision_guard&) = delete;

private:
    mpfr_prec_t old_precision_;
};

int decimal_digits_for_bits(mpfr_prec_t bits)
{
    return static_cast<int>(
        std::floor(static_cast<double>(bits) * std::log10(2.0)));
}

struct pi_result {
    std::string name;
    mpfr_prec_t requested_precision;
    mpfr_prec_t default_precision;
    mpfr_prec_t result_precision;
    int iterations;
    mpfrxx::mpfr_class pi;
};

pi_result compute_pi_in_thread(const char* name, mpfr_prec_t precision)
{
    default_precision_guard guard(precision);

    mpfrxx::mpfr_class one("1.0");
    mpfrxx::mpfr_class two("2.0");
    mpfrxx::mpfr_class four("4.0");
    mpfrxx::mpfr_class a = one;
    mpfrxx::mpfr_class b = one / mpfrxx::sqrt(two);
    mpfrxx::mpfr_class t("0.25");
    mpfrxx::mpfr_class p = one;
    mpfrxx::mpfr_class pi("0.0");
    mpfrxx::mpfr_class previous_pi("0.0");
    mpfrxx::mpfr_class tolerance =
        mpfrxx::exp2(-mpfrxx::mpfr_class(mpfrxx::default_precision_bits() / 2));

    int iteration = 0;
    do {
        previous_pi = pi;

        mpfrxx::mpfr_class next_a = (a + b) / two;
        mpfrxx::mpfr_class next_b = mpfrxx::sqrt(a * b);
        mpfrxx::mpfr_class delta = a - next_a;
        t = t - p * delta * delta;
        p = two * p;
        a = next_a;
        b = next_b;

        mpfrxx::mpfr_class sum = a + b;
        pi = sum * sum / (four * t);

        ++iteration;
    } while (iteration == 1 || mpfrxx::abs(pi - previous_pi) > tolerance);

    pi_result result{};
    result.name = name;
    result.requested_precision = precision;
    result.default_precision = mpfrxx::default_precision_bits();
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
    const mpfr_prec_t main_default = mpfrxx::default_precision_bits();
    std::unique_ptr<pi_result> low;
    std::unique_ptr<pi_result> high;

    std::thread thread_low([&] {
        low = std::make_unique<pi_result>(compute_pi_in_thread("thread-a", 160));
    });
    std::thread thread_high([&] {
        high = std::make_unique<pi_result>(compute_pi_in_thread("thread-b", 768));
    });

    thread_low.join();
    thread_high.join();

    print_result(*low);
    print_result(*high);

    if (low->default_precision != 160 ||
        high->default_precision != 768 ||
        low->result_precision < 160 ||
        high->result_precision < 768 ||
        mpfrxx::default_precision_bits() != main_default) {
        std::abort();
    }

    mpfrxx::mpfr_class main_thread_value;
    std::cout << "main thread default=" << mpfrxx::default_precision_bits()
              << " value_prec=" << main_thread_value.precision()
              << '\n';

    return 0;
}
