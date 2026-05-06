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
 * Example 10: Mignotte polynomial root separation with mpfrxx::mpfr_class.
 */

#include <mpfrxx_mkII.h>

#include <cstddef>
#include <iomanip>
#include <iostream>

namespace {

using mpfrxx::mpfr_class;

mpfr_class pow_ui(mpfr_class base, std::size_t exponent)
{
    mpfr_class result(1);
    while (exponent != 0) {
        if ((exponent & 1U) != 0U) {
            result *= base;
        }
        exponent >>= 1U;
        if (exponent != 0) {
            base *= base;
        }
    }
    return result;
}

mpfr_class inverse_power(unsigned long base, std::size_t exponent)
{
    return mpfr_class(1) / pow_ui(mpfr_class(base), exponent);
}

mpfr_class evaluate_mignotte(const mpfr_class& x, std::size_t degree,
                             unsigned long a)
{
    mpfr_class ax_minus_one = mpfr_class(a) * x - mpfr_class(1);
    return pow_ui(x, degree) - mpfr_class(2) * ax_minus_one * ax_minus_one;
}

struct close_pair {
    bool resolved = false;
    mpfr_class left;
    mpfr_class right;
    mpfr_class separation;
    mpfr_class left_residual;
    mpfr_class right_residual;
};

bool refine_bisection(mpfr_class negative_endpoint,
                      mpfr_class positive_endpoint, std::size_t degree,
                      unsigned long a, const mpfr_class& tolerance,
                      mpfr_class& root)
{
    mpfr_class f_negative =
        evaluate_mignotte(negative_endpoint, degree, a);
    mpfr_class f_positive =
        evaluate_mignotte(positive_endpoint, degree, a);
    if (!(f_negative < 0 && f_positive > 0)) {
        return false;
    }

    mpfr_prec_t iteration_limit = mpfrxx::default_precision_bits() + 32;
    for (mpfr_prec_t i = 0; i < iteration_limit; ++i) {
        mpfr_class midpoint = (negative_endpoint + positive_endpoint) /
                              mpfr_class(2);
        if (midpoint == negative_endpoint || midpoint == positive_endpoint) {
            root = midpoint;
            return true;
        }

        mpfr_class f_midpoint = evaluate_mignotte(midpoint, degree, a);
        if (f_midpoint < 0) {
            negative_endpoint = midpoint;
        } else {
            positive_endpoint = midpoint;
        }
        if (mpfrxx::abs(positive_endpoint - negative_endpoint) < tolerance) {
            root = (negative_endpoint + positive_endpoint) / mpfr_class(2);
            return true;
        }
    }

    root = (negative_endpoint + positive_endpoint) / mpfr_class(2);
    return true;
}

close_pair solve_close_pair(std::size_t degree, unsigned long a)
{
    close_pair result;

    mpfr_class center = mpfr_class(1) / mpfr_class(a);
    mpfr_class expected_separation =
        mpfrxx::sqrt(mpfr_class(2)) * inverse_power(a, degree / 2 + 1);
    mpfr_class half_width = expected_separation * mpfr_class(4);
    mpfr_class tolerance =
        mpfrxx::exp2(-mpfr_class(mpfrxx::default_precision_bits() / 2));

    mpfr_class left_outer = center - half_width;
    mpfr_class right_outer = center + half_width;
    if (left_outer == center || right_outer == center) {
        return result;
    }

    mpfr_class f_center = evaluate_mignotte(center, degree, a);
    if (!(f_center > 0)) {
        return result;
    }

    for (int expansion = 0; expansion < 8; ++expansion) {
        if (evaluate_mignotte(left_outer, degree, a) < 0 &&
            evaluate_mignotte(right_outer, degree, a) < 0) {
            break;
        }
        half_width *= mpfr_class(2);
        left_outer = center - half_width;
        right_outer = center + half_width;
        if (left_outer == center || right_outer == center) {
            return result;
        }
    }

    mpfr_class left_root(0);
    mpfr_class right_root(0);
    if (!refine_bisection(left_outer, center, degree, a, tolerance,
                         left_root)) {
        return result;
    }
    if (!refine_bisection(right_outer, center, degree, a, tolerance,
                         right_root)) {
        return result;
    }

    result.resolved = true;
    result.left = left_root;
    result.right = right_root;
    result.separation = right_root - left_root;
    result.left_residual =
        mpfrxx::abs(evaluate_mignotte(left_root, degree, a));
    result.right_residual =
        mpfrxx::abs(evaluate_mignotte(right_root, degree, a));
    return result;
}

void run_default_precision()
{
    constexpr std::size_t degree = 20;
    constexpr unsigned long a = 1000000;

    mpfr_class center = mpfr_class(1) / mpfr_class(a);
    mpfr_class expected_separation =
        mpfrxx::sqrt(mpfr_class(2)) * inverse_power(a, degree / 2 + 1);

    std::cout << "\ndefault precision = "
              << mpfrxx::default_precision_bits() << " bits\n";
    std::cout << "  center 1/a                  = " << center << '\n';
    std::cout << "  asymptotic close separation = "
              << expected_separation << '\n';

    close_pair roots = solve_close_pair(degree, a);
    if (!roots.resolved) {
        std::cout << "  close pair not resolved: endpoints around 1/a"
                     " collapse or fail to bracket distinct roots at this"
                     " precision.\n";
        return;
    }

    std::cout << "  left root                   = " << roots.left << '\n';
    std::cout << "  right root                  = " << roots.right << '\n';
    std::cout << "  left/right offset from 1/a  = "
              << (roots.left - center) << "  "
              << (roots.right - center) << '\n';
    std::cout << "  computed separation         = "
              << roots.separation << '\n';
    std::cout << "  |p(left)|, |p(right)|       = "
              << roots.left_residual << "  "
              << roots.right_residual << '\n';
}

}  // namespace

int main()
{
    std::cout << std::scientific << std::setprecision(24);
    std::cout << "Mignotte polynomial root-separation example using mpfr_class\n";
    std::cout << "p_{20,1000000}(x) = x^20 - 2(1000000x - 1)^2\n";
    std::cout << "                   = x^20 - 2000000000000x^2"
                 " + 4000000x - 2\n";

    run_default_precision();

    return 0;
}
