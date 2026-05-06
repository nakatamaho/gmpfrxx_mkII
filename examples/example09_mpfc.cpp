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
 * Example 09: near-multiple-root perturbation with gmpxx::mpfc_class.
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

#include <gmpxx_mkII.h>

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using gmpxx::mpf_class;
using gmpxx::mpfc_class;

mpfc_class make_complex(const mpf_class& real_value,
                        const mpf_class& imag_value)
{
    return mpfc_class(real_value, imag_value);
}

mpfc_class make_real(const mpf_class& real_value)
{
    return make_complex(real_value, mpf_class(0));
}

std::vector<mpf_class> shifted_power_coefficients(std::size_t degree,
                                                  const mpf_class& epsilon)
{
    std::vector<mpf_class> coefficients(1, mpf_class(1));

    for (std::size_t factor = 0; factor < degree; ++factor) {
        (void)factor;
        std::vector<mpf_class> next(coefficients.size() + 1, mpf_class(0));
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            next[i] -= coefficients[i];
            next[i + 1] += coefficients[i];
        }
        coefficients = next;
    }

    coefficients[0] += epsilon;
    return coefficients;
}

mpfc_class evaluate_polynomial(const std::vector<mpf_class>& coefficients,
                               const mpfc_class& z)
{
    if (coefficients.empty()) {
        return make_real(mpf_class(0));
    }

    mpfc_class value = make_real(coefficients.back());
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value *= z;
        value += coefficients[k];
    }
    return value;
}

mpfc_class evaluate_derivative(const std::vector<mpf_class>& coefficients,
                               const mpfc_class& z)
{
    if (coefficients.size() < 2) {
        return make_real(mpf_class(0));
    }

    std::size_t degree = coefficients.size() - 1;
    mpfc_class value = make_real(
        coefficients[degree] * mpf_class(static_cast<unsigned long>(degree)));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        value += coefficients[k] * mpf_class(static_cast<unsigned long>(k));
    }
    return value;
}

std::vector<mpfc_class> initial_circle(std::size_t degree,
                                       const mpf_class& center,
                                       const mpf_class& radius)
{
    std::vector<mpfc_class> roots;
    roots.reserve(degree);

    mpf_class two_pi = gmpxx::two_pi();
    mpf_class pi = gmpxx::const_pi();
    mpf_class degree_value(static_cast<unsigned long>(degree));
    mpf_class angle_offset("0.07");

    for (std::size_t i = 0; i < degree; ++i) {
        mpf_class index(static_cast<unsigned long>(i));
        mpf_class angle = (pi + two_pi * index) / degree_value + angle_offset;
        roots.push_back(make_complex(center + radius * gmpxx::cos(angle),
                                     radius * gmpxx::sin(angle)));
    }

    return roots;
}

std::vector<mpfc_class> solve_with_aberth(
    const std::vector<mpf_class>& coefficients,
    std::vector<mpfc_class> roots, const mpf_class& tolerance,
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
        mpf_class max_update(0);
        std::vector<mpfc_class> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            mpfc_class f = evaluate_polynomial(coefficients, roots[i]);
            mpfc_class df = evaluate_derivative(coefficients, roots[i]);
            mpfc_class ratio = f / df;

            mpfc_class one = make_real(mpf_class(1));
            mpfc_class repulsion = make_real(mpf_class(0));
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    repulsion += one / (roots[i] - roots[j]);
                }
            }

            mpfc_class correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpf_class update_size = gmpxx::abs(correction);
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

void sort_by_angle_around_one(std::vector<mpfc_class>& roots)
{
    mpfc_class one(mpf_class(1), mpf_class(0));
    std::sort(roots.begin(), roots.end(),
              [&](const mpfc_class& a, const mpfc_class& b) {
                  return gmpxx::arg(a - one) < gmpxx::arg(b - one);
              });
}

void print_radius_summary(std::vector<mpfc_class> roots,
                          const std::vector<mpf_class>& coefficients)
{
    mpfc_class one(mpf_class(1), mpf_class(0));
    sort_by_angle_around_one(roots);

    mpf_class min_radius("1e100");
    mpf_class max_radius(0);
    mpf_class sum_radius(0);
    mpf_class max_residual(0);

    for (const mpfc_class& root : roots) {
        mpf_class radius = gmpxx::abs(root - one);
        min_radius = radius < min_radius ? radius : min_radius;
        max_radius = radius > max_radius ? radius : max_radius;
        sum_radius += radius;

        mpf_class residual = gmpxx::abs(evaluate_polynomial(coefficients, root));
        max_residual = residual > max_residual ? residual : max_residual;
    }

    mpf_class mean_radius =
        sum_radius / mpf_class(static_cast<unsigned long>(roots.size()));

    std::cout << "  radius min/mean/max = " << min_radius << "  "
              << mean_radius << "  " << max_radius << '\n';
    std::cout << "  max |p(root)|       = " << max_residual << '\n';
    std::cout << "  first roots by angle around x=1:\n";
    std::cout << "    angle                  |root-1|"
                 "                 root\n";

    std::size_t shown = std::min<std::size_t>(roots.size(), 8);
    for (std::size_t i = 0; i < shown; ++i) {
        mpfc_class shifted = roots[i] - one;
        std::cout << "    " << std::setw(22) << gmpxx::arg(shifted)
                  << "  " << std::setw(22) << gmpxx::abs(shifted)
                  << "  " << roots[i] << '\n';
    }
}

void run_default_precision()
{
    constexpr std::size_t degree = 20;

    mpf_class epsilon("1e-40");
    mpf_class expected_radius("1e-2");
    mpf_class center(1);
    std::vector<mpf_class> coefficients =
        shifted_power_coefficients(degree, epsilon);
    mpf_class retained_epsilon = coefficients[0] - mpf_class(1);

    std::cout << "\ndefault precision = "
              << gmpxx::default_mpf_precision_bits() << " bits\n";
    std::cout << "  retained constant perturbation = "
              << retained_epsilon << '\n';

    if (retained_epsilon == 0) {
        std::cout << "  perturbation is rounded away at this precision.\n";
        return;
    }

    std::cout << "  expected |root-1| = " << expected_radius << '\n';
    mpf_class initial_radius = expected_radius * mpf_class("1.2");
    mpf_class tolerance =
        gmpxx::exp2(-mpf_class(gmpxx::default_mpf_precision_bits() / 2));

    std::vector<mpfc_class> roots = solve_with_aberth(
        coefficients, initial_circle(degree, center, initial_radius),
        tolerance, 40);

    print_radius_summary(roots, coefficients);
}

}  // namespace

int main()
{
    std::cout << std::scientific << std::setprecision(18);
    std::cout << "Near-multiple-root perturbation example using mpfc_class\n";
    std::cout << "p(x) = (x - 1)^20 + 1e-40\n";
    std::cout << "The exact roots satisfy |x - 1| = 1e-2.\n";

    run_default_precision();

    return 0;
}
