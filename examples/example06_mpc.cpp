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
 * Example 06: Aberth-Ehrlich root finding with mpfrxx::mpc_class.
 *
 * This is the mpc_class version of example05.  It keeps the same
 * mathematical method and polynomial, but the complex arithmetic is now
 * expressed through the library's complex MPFR/MPC type.  The example
 * therefore demonstrates the intended high-level API: real coefficients are
 * stored as mpfr_class, complex iterates as mpc_class, and stream output
 * follows the std::complex-compatible style implemented by mpfrxx_mkII.
 *
 * The Aberth-Ehrlich correction combines a Newton step with a sum over the
 * other current approximations.  That sum is the practical reason this method
 * is attractive for examples: all roots are advanced simultaneously, and the
 * code naturally exercises complex division, absolute value, expression
 * templates, and formatted I/O.
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

#include <mpfrxx_mkII.h>

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using mpfrxx::mpc_class;
using mpfrxx::mpfr_class;

mpc_class make_complex(char const* real_value, char const* imag_value)
{
    mpfr_class real(real_value);
    mpfr_class imag(imag_value);
    return mpc_class(real, imag);
}

mpc_class make_complex(mpfr_class const& real_value,
                       mpfr_class const& imag_value)
{
    return mpc_class(real_value, imag_value);
}

mpfr_class cauchy_radius(std::vector<mpfr_class> const& coefficients)
{
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }

    mpfr_class leading = mpfrxx::abs(coefficients.back());
    if (leading == mpfr_class(0)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    mpfr_class max_ratio(0);
    for (std::size_t i = 0; i + 1 < coefficients.size(); ++i) {
        mpfr_class ratio = mpfrxx::abs(coefficients[i]) / leading;
        if (ratio > max_ratio) {
            max_ratio = ratio;
        }
    }

    return mpfr_class(1) + max_ratio;
}

mpc_class evaluate_polynomial(std::vector<mpfr_class> const& coefficients,
                              mpc_class const& z)
{
    if (coefficients.empty()) {
        return make_complex("0.0", "0.0");
    }

    mpc_class value = make_complex(coefficients.back(), mpfr_class(0));
    for (std::size_t k = coefficients.size() - 1; k-- > 0;) {
        value *= z;
        value += coefficients[k];
    }
    return value;
}

mpc_class evaluate_derivative(std::vector<mpfr_class> const& coefficients,
                              mpc_class const& z)
{
    if (coefficients.size() < 2) {
        return make_complex("0.0", "0.0");
    }

    std::size_t degree = coefficients.size() - 1;
    mpc_class value =
        make_complex(coefficients[degree] *
                         mpfr_class(static_cast<unsigned long>(degree)),
                     mpfr_class(0));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        value += mpfr_class(
            coefficients[k] *
            mpfr_class(static_cast<unsigned long>(k)));
    }
    return value;
}

std::vector<mpc_class>
initial_circle(std::size_t degree, mpfr_class const& radius)
{
    std::vector<mpc_class> roots;
    roots.reserve(degree);

    mpfr_class two(2);
    mpfr_class quarter("0.25");
    mpfr_class pi = mpfrxx::const_pi();
    mpfr_class denominator(static_cast<unsigned long>(degree));

    for (std::size_t i = 0; i < degree; ++i) {
        mpfr_class index(static_cast<unsigned long>(i));
        mpfr_class angle = two * pi * (index + quarter) / denominator;
        roots.push_back(make_complex(mpfr_class(radius * mpfrxx::cos(angle)),
                                     mpfr_class(radius * mpfrxx::sin(angle))));
    }

    return roots;
}

std::vector<mpc_class> solve_with_aberth(
    std::vector<mpfr_class> const& coefficients, mpfr_class const& radius,
    mpfr_class const& tolerance, int max_iterations)
{
    if (coefficients.size() < 2) {
        throw std::invalid_argument("polynomial degree must be at least one");
    }
    if (coefficients.back() == mpfr_class(0)) {
        throw std::invalid_argument("leading coefficient must be nonzero");
    }

    std::size_t degree = coefficients.size() - 1;
    std::vector<mpc_class> roots = initial_circle(degree, radius);

    for (int iteration = 1; iteration <= max_iterations; ++iteration) {
        mpfr_class max_update(0);
        std::vector<mpc_class> next_roots = roots;

        for (std::size_t i = 0; i < degree; ++i) {
            mpc_class f = evaluate_polynomial(coefficients, roots[i]);
            mpc_class df = evaluate_derivative(coefficients, roots[i]);
            mpc_class ratio = f / df;

            mpc_class repulsion = make_complex("0.0", "0.0");
            for (std::size_t j = 0; j < degree; ++j) {
                if (i != j) {
                    mpc_class separation = roots[i] - roots[j];
                    repulsion += mpfr_class(1) / separation;
                }
            }

            mpc_class one = make_complex("1.0", "0.0");
            mpc_class correction = ratio / (one - ratio * repulsion);
            next_roots[i] = roots[i] - correction;

            mpfr_class update_size = mpfrxx::abs(correction);
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

void print_root(std::size_t index, mpc_class const& root,
                std::vector<mpfr_class> const& coefficients)
{
    mpc_class residual = evaluate_polynomial(coefficients, root);

    std::cout << "root " << index << ": " << root;
    std::cout << ", |f(root)| = " << mpfrxx::abs(residual) << '\n';
}

} // namespace

int main()
{
    constexpr int decimal_digits = 60;

    std::vector<mpfr_class> coefficients = {
        mpfr_class(13), mpfr_class(-11), mpfr_class(7), mpfr_class(-1),
        mpfr_class(0),  mpfr_class(2),   mpfr_class(0), mpfr_class(-3),
        mpfr_class(0),  mpfr_class(0),   mpfr_class(1)};

    mpfr_class radius = cauchy_radius(coefficients);
    mpfr_class tolerance =
        mpfrxx::exp2(-mpfr_class(mpfrxx::default_precision_bits() / 2));

    std::cout << std::scientific << std::setprecision(decimal_digits);
    std::cout << "Aberth method using mpfrxx::mpc_class for f(z) = "
                 "z^10 - 3 z^7 + 2 z^5 - z^3 + 7 z^2 - 11 z + 13\n";
    std::cout << "coefficients a0..a10 ="
                 " {13, -11, 7, -1, 0, 2, 0, -3, 0, 0, 1}\n";
    std::cout << "initial radius = " << radius << '\n';

    std::vector<mpc_class> roots =
        solve_with_aberth(coefficients, radius, tolerance, 80);

    for (std::size_t i = 0; i < roots.size(); ++i) {
        print_root(i, roots[i], coefficients);
    }

    return 0;
}
