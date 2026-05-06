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
 * Example 09: near-multiple-root perturbation with mpfrxx::mpc_class.
 *
 * The polynomial
 *
 *     p(x) = (x - 1)^20 + 1e-40
 *
 * is a compact conditioning example.  In exact arithmetic, the perturbation
 * changes the 20-fold root at x = 1 into roots satisfying
 *
 *     (x - 1)^20 = -1e-40,
 *
 * so the roots lie on a circle of radius 1e-2 around x = 1.  The coefficient
 * perturbation is 1e-40, but the root displacement is about 1e-2.  This
 * exposes the same core issue as Wilkinson's polynomial: root location can be
 * dramatically more sensitive than coefficient size suggests.
 *
 * This default-precision port runs the stored polynomial construction at the
 * active wrapper default precision.  If the 1e-40 addition to the constant
 * coefficient is rounded away, the printed root circle is not a property of
 * the stored coefficients.
 *
 * This particular polynomial is an elementary didactic variant rather than a
 * separate named historical test.  The sensitivity context follows
 * Wilkinson's root-conditioning example; the simultaneous solver follows
 * Ehrlich and Aberth.
 *
 * Wilkinson reference:
 *
 * - James H. Wilkinson, "Rounding Errors in Algebraic Processes", originally
 *   published by HMSO/Prentice-Hall, 1963; SIAM Classics reissue, 2023.
 *   DOI: 10.1137/1.9781611977523
 *
 * Aberth-Ehrlich references:
 *
 * - Louis W. Ehrlich, "A modified Newton method for polynomials",
 *   Communications of the ACM 10(2), 107-108, 1967.
 *   DOI: 10.1145/363067.363115
 * - Oliver Aberth, "Iteration methods for finding all zeros of a polynomial
 *   simultaneously", Mathematics of Computation 27(122), 339-344, 1973.
 *   DOI: 10.1090/S0025-5718-1973-0329236-7
 */

#include <mpfrxx_mkII.h>

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using mpfrxx::mpc_class;
using mpfrxx::mpfr_class;

mpc_class make_complex(const mpfr_class& real_value,
                       const mpfr_class& imag_value)
{
    return mpc_class(real_value, imag_value);
}

mpc_class make_real(const mpfr_class& real_value)
{
    return make_complex(real_value, mpfr_class(0));
}

std::vector<mpfr_class> shifted_power_coefficients(std::size_t degree,
                                                   const mpfr_class& epsilon)
{
    std::vector<mpfr_class> coefficients(1, mpfr_class(1));

    for (std::size_t factor = 0; factor < degree; ++factor) {
        (void)factor;
        std::vector<mpfr_class> next(coefficients.size() + 1, mpfr_class(0));
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            next[i] -= coefficients[i];
            next[i + 1] += coefficients[i];
        }
        coefficients = next;
    }

    coefficients[0] += epsilon;
    return coefficients;
}

mpc_class evaluate_polynomial(const std::vector<mpfr_class>& coefficients,
                              const mpc_class& z)
{
    if (coefficients.empty()) {
        return make_real(mpfr_class(0));
    }

    mpc_class value = make_real(coefficients.back());
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value *= z;
        value += coefficients[k];
    }
    return value;
}

mpc_class evaluate_derivative(const std::vector<mpfr_class>& coefficients,
                              const mpc_class& z)
{
    if (coefficients.size() < 2) {
        return make_real(mpfr_class(0));
    }

    std::size_t degree = coefficients.size() - 1;
    mpc_class value = make_real(
        coefficients[degree] * mpfr_class(static_cast<unsigned long>(degree)));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        value += coefficients[k] * mpfr_class(static_cast<unsigned long>(k));
    }
    return value;
}

std::vector<mpc_class> initial_circle(std::size_t degree,
                                      const mpfr_class& center,
                                      const mpfr_class& radius)
{
    std::vector<mpc_class> roots;
    roots.reserve(degree);

    mpfr_class pi = mpfrxx::const_pi();
    mpfr_class two_pi = mpfr_class(2) * pi;
    mpfr_class degree_value(static_cast<unsigned long>(degree));
    mpfr_class angle_offset("0.07");

    for (std::size_t i = 0; i < degree; ++i) {
        mpfr_class index(static_cast<unsigned long>(i));
        mpfr_class angle = (pi + two_pi * index) / degree_value + angle_offset;
        roots.push_back(make_complex(center + radius * mpfrxx::cos(angle),
                                     radius * mpfrxx::sin(angle)));
    }

    return roots;
}

std::vector<mpc_class> solve_with_aberth(
    const std::vector<mpfr_class>& coefficients,
    std::vector<mpc_class> roots, const mpfr_class& tolerance,
    int max_iterations)
{
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    std::size_t degree = coefficients.size() - 1;
    if (roots.size() != degree) {
        throw std::invalid_argument("initial root count must match degree");
    }

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpfr_class max_update(0);
        std::vector<mpc_class> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            mpc_class f = evaluate_polynomial(coefficients, roots[i]);
            mpc_class df = evaluate_derivative(coefficients, roots[i]);
            mpc_class ratio = f / df;

            mpc_class one = make_real(mpfr_class(1));
            mpc_class repulsion = make_real(mpfr_class(0));
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    repulsion += one / (roots[i] - roots[j]);
                }
            }

            mpc_class correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpfr_class update_size = mpfrxx::abs(correction);
            if (update_size > max_update) {
                max_update = update_size;
            }
        }

        roots = next_roots;
        if (iteration <= 4 || iteration % 5 == 0) {
            std::cout << "  iteration " << std::setw(2) << iteration
                      << ", max correction = " << max_update << '\n';
        }
        if (max_update < tolerance) {
            break;
        }
    }

    return roots;
}

void sort_by_angle_around_one(std::vector<mpc_class>& roots)
{
    mpc_class one(mpfr_class(1), mpfr_class(0));
    std::sort(roots.begin(), roots.end(),
              [&](const mpc_class& a, const mpc_class& b) {
                  return mpfrxx::arg(a - one) < mpfrxx::arg(b - one);
              });
}

void print_radius_summary(std::vector<mpc_class> roots,
                          const std::vector<mpfr_class>& coefficients)
{
    mpc_class one(mpfr_class(1), mpfr_class(0));
    sort_by_angle_around_one(roots);

    mpfr_class min_radius("1e100");
    mpfr_class max_radius(0);
    mpfr_class sum_radius(0);
    mpfr_class max_residual(0);

    for (const mpc_class& root : roots) {
        mpfr_class radius = mpfrxx::abs(root - one);
        min_radius = radius < min_radius ? radius : min_radius;
        max_radius = radius > max_radius ? radius : max_radius;
        sum_radius += radius;

        mpfr_class residual = mpfrxx::abs(evaluate_polynomial(coefficients, root));
        max_residual = residual > max_residual ? residual : max_residual;
    }

    mpfr_class mean_radius =
        sum_radius / mpfr_class(static_cast<unsigned long>(roots.size()));

    std::cout << "  radius min/mean/max = " << min_radius << "  "
              << mean_radius << "  " << max_radius << '\n';
    std::cout << "  max |p(root)|       = " << max_residual << '\n';
    std::cout << "  first roots by angle around x=1:\n";
    std::cout << "    angle                  |root-1|"
                 "                 root\n";

    std::size_t shown = std::min<std::size_t>(roots.size(), 8);
    for (std::size_t i = 0; i < shown; ++i) {
        mpc_class shifted = roots[i] - one;
        std::cout << "    " << std::setw(22) << mpfrxx::arg(shifted)
                  << "  " << std::setw(22) << mpfrxx::abs(shifted)
                  << "  " << roots[i] << '\n';
    }
}

void run_default_precision()
{
    constexpr std::size_t degree = 20;

    mpfr_class epsilon("1e-40");
    mpfr_class expected_radius("1e-2");
    mpfr_class center(1);
    std::vector<mpfr_class> coefficients =
        shifted_power_coefficients(degree, epsilon);
    mpfr_class retained_epsilon = coefficients[0] - mpfr_class(1);

    std::cout << "\ndefault precision = "
              << mpfrxx::default_precision_bits() << " bits\n";
    std::cout << "  retained constant perturbation = "
              << retained_epsilon << '\n';

    if (retained_epsilon == 0) {
        std::cout << "  perturbation is rounded away at this precision.\n";
        return;
    }

    std::cout << "  expected |root-1| = " << expected_radius << '\n';
    mpfr_class initial_radius = expected_radius * mpfr_class("1.2");
    mpfr_class tolerance =
        mpfrxx::exp2(-mpfr_class(mpfrxx::default_precision_bits() / 2));

    std::vector<mpc_class> roots = solve_with_aberth(
        coefficients, initial_circle(degree, center, initial_radius),
        tolerance, 40);

    print_radius_summary(roots, coefficients);
}

}  // namespace

int main()
{
    std::cout << std::scientific << std::setprecision(18);
    std::cout << "Near-multiple-root perturbation example using mpc_class\n";
    std::cout << "p(x) = (x - 1)^20 + 1e-40\n";
    std::cout << "The exact roots satisfy |x - 1| = 1e-2.\n";

    run_default_precision();

    return 0;
}
