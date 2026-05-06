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
 * Example 05: Aberth-Ehrlich root finding with a local real-only complex
 * type.
 *
 * This example solves a degree-10 polynomial with real coefficients and
 * complex roots.  It deliberately implements the complex arithmetic from
 * pairs of mpf_class values so that the real operations are visible.  The
 * update is the Aberth-Ehrlich simultaneous root iteration:
 *
 *     z_i <- z_i - (p(z_i) / p'(z_i)) /
 *                  (1 - (p(z_i) / p'(z_i))
 *                       * sum_{j != i} 1 / (z_i - z_j)).
 *
 * The summation term acts as an implicit deflation/repulsion term, reducing
 * the tendency for two approximations to converge to the same simple root.
 * The starting points are placed on a Cauchy root-bound circle.  Horner
 * evaluation is used for both p and p'.
 *
 * First publications for the iteration:
 *
 * - Louis W. Ehrlich, "A modified Newton method for polynomials",
 *   Communications of the ACM 10(2), 107-108, 1967.
 *   DOI: 10.1145/363067.363115
 * - Oliver Aberth, "Iteration methods for finding all zeros of a polynomial
 *   simultaneously", Mathematics of Computation 27(122), 339-344, 1973.
 *   DOI: 10.1090/S0025-5718-1973-0329236-7
 */

#include <gmpxx_mkII.h>

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using gmpxx::mpf_class;

struct complex_mpf {
    mpf_class re;
    mpf_class im;
};

complex_mpf make_complex(char const* re, char const* im)
{
    return {mpf_class(re), mpf_class(im)};
}

complex_mpf make_complex(mpf_class const& re, mpf_class const& im)
{
    return {re, im};
}

complex_mpf operator+(complex_mpf const& lhs, complex_mpf const& rhs)
{
    return {lhs.re + rhs.re, lhs.im + rhs.im};
}

complex_mpf operator-(complex_mpf const& lhs, complex_mpf const& rhs)
{
    return {lhs.re - rhs.re, lhs.im - rhs.im};
}

complex_mpf operator*(complex_mpf const& lhs, complex_mpf const& rhs)
{
    return {lhs.re * rhs.re - lhs.im * rhs.im,
            lhs.re * rhs.im + lhs.im * rhs.re};
}

complex_mpf operator*(complex_mpf const& lhs, mpf_class const& rhs)
{
    return {lhs.re * rhs, lhs.im * rhs};
}

complex_mpf operator/(complex_mpf const& lhs, complex_mpf const& rhs)
{
    mpf_class denom = rhs.re * rhs.re + rhs.im * rhs.im;
    return {(lhs.re * rhs.re + lhs.im * rhs.im) / denom,
            (lhs.im * rhs.re - lhs.re * rhs.im) / denom};
}

mpf_class norm2(complex_mpf const& z)
{
    return z.re * z.re + z.im * z.im;
}

mpf_class abs_complex(complex_mpf const& z)
{
    return gmpxx::sqrt(norm2(z));
}

complex_mpf reciprocal(complex_mpf const& z)
{
    mpf_class denom = norm2(z);
    return {z.re / denom, -z.im / denom};
}

mpf_class cauchy_radius(std::vector<mpf_class> const& coefficients)
{
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    mpf_class leading = gmpxx::abs(coefficients.back());
    if (leading == mpf_class(0)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    mpf_class max_ratio(0);
    for (std::size_t i = 0; i + 1 < coefficients.size(); ++i) {
        mpf_class ratio = gmpxx::abs(coefficients[i]) / leading;
        if (ratio > max_ratio) {
            max_ratio = ratio;
        }
    }

    return mpf_class(1) + max_ratio;
}

complex_mpf evaluate_polynomial(std::vector<mpf_class> const& coefficients,
                                complex_mpf const& z)
{
    if (coefficients.empty()) {
        return make_complex("0.0", "0.0");
    }

    complex_mpf value = make_complex(coefficients.back(), mpf_class(0));
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value = value * z + make_complex(coefficients[k], mpf_class(0));
    }
    return value;
}

complex_mpf evaluate_derivative(std::vector<mpf_class> const& coefficients,
                                complex_mpf const& z)
{
    if (coefficients.size() < 2) {
        return make_complex("0.0", "0.0");
    }

    std::size_t degree = coefficients.size() - 1;
    complex_mpf value = make_complex(coefficients[degree] *
                                         mpf_class(static_cast<unsigned long>(
                                             degree)),
                                     mpf_class(0));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value = value * z +
                make_complex(coefficients[k] *
                                 mpf_class(static_cast<unsigned long>(k)),
                             mpf_class(0));
    }
    return value;
}

std::vector<complex_mpf>
initial_circle(std::size_t degree, mpf_class const& radius)
{
    std::vector<complex_mpf> roots;
    roots.reserve(degree);

    mpf_class two(2);
    mpf_class quarter("0.25");
    mpf_class pi = gmpxx::const_pi();
    mpf_class denominator(static_cast<unsigned long>(degree));

    for (std::size_t i = 0; i < degree; ++i) {
        mpf_class index(static_cast<unsigned long>(i));
        mpf_class angle = two * pi * (index + quarter) / denominator;
        roots.push_back(
            {radius * gmpxx::cos(angle), radius * gmpxx::sin(angle)});
    }

    return roots;
}

std::vector<complex_mpf> solve_with_aberth(
    std::vector<mpf_class> const& coefficients, mpf_class const& radius,
    mpf_class const& tolerance, int max_iterations)
{
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }
    if (coefficients.back() == mpf_class(0)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    std::size_t degree = coefficients.size() - 1;
    std::vector<complex_mpf> roots = initial_circle(degree, radius);

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpf_class max_update(0);
        std::vector<complex_mpf> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            complex_mpf f = evaluate_polynomial(coefficients, roots[i]);
            complex_mpf df = evaluate_derivative(coefficients, roots[i]);
            complex_mpf ratio = f / df;

            complex_mpf repulsion = make_complex("0.0", "0.0");
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    repulsion = repulsion + reciprocal(roots[i] - roots[j]);
                }
            }

            complex_mpf one = make_complex("1.0", "0.0");
            complex_mpf correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpf_class update_size = abs_complex(correction);
            if (update_size > max_update) {
                max_update = update_size;
            }
        }

        roots = next_roots;
        std::cout << "iteration " << std::setw(2) << iteration
                  << ", max correction = " << max_update << '\n';

        if (max_update < tolerance) {
            break;
        }
    }

    return roots;
}

void print_root(std::size_t index, complex_mpf const& root,
                std::vector<mpf_class> const& coefficients)
{
    complex_mpf residual = evaluate_polynomial(coefficients, root);

    std::cout << "root " << index << ": " << root.re;
    if (root.im >= mpf_class(0)) {
        std::cout << " + " << root.im << " i";
    } else {
        std::cout << " - " << -root.im << " i";
    }
    std::cout << ", |f(root)| = " << abs_complex(residual) << '\n';
}

} // namespace

int main()
{
    constexpr int decimal_digits = 60;

    std::vector<mpf_class> coefficients = {
        mpf_class(13), mpf_class(-11), mpf_class(7), mpf_class(-1),
        mpf_class(0),  mpf_class(2),   mpf_class(0), mpf_class(-3),
        mpf_class(0),  mpf_class(0),   mpf_class(1)};

    mpf_class radius = cauchy_radius(coefficients);
    mpf_class tolerance =
        gmpxx::exp2(-mpf_class(gmpxx::default_mpf_precision_bits() / 2));

    std::cout << std::scientific << std::setprecision(decimal_digits);
    std::cout << "Aberth method for f(z) = z^10 - 3 z^7 + 2 z^5"
                 " - z^3 + 7 z^2 - 11 z + 13\n";
    std::cout << "coefficients a0..a10 ="
                 " {13, -11, 7, -1, 0, 2, 0, -3, 0, 0, 1}\n";
    std::cout << "initial radius = " << radius << '\n';

    std::vector<complex_mpf> roots =
        solve_with_aberth(coefficients, radius, tolerance, 80);

    for (std::size_t i = 0; i < roots.size(); ++i) {
        print_root(i, roots[i], coefficients);
    }

    return 0;
}
