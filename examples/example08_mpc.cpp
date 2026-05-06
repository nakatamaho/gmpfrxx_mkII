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
 * Example 08: Wilkinson polynomial sensitivity.
 *
 * Wilkinson's degree-20 polynomial
 *
 *     W_20(x) = product_{k=1}^{20} (x - k)
 *
 * has exactly known integer roots, but its roots are highly sensitive to
 * small coefficient perturbations.  This example solves the exact polynomial
 * and then repeats the solve after perturbing only the x^19 coefficient by
 * 1e-10.  The printed root shifts make coefficient/root conditioning visible
 * without relying on a double-precision reference calculation.
 *
 * Aberth-Ehrlich is used as the simultaneous solver, so the root-finding
 * update has the same references as examples 05 and 06.  Wilkinson's original
 * discussion appeared in a 1963 book before DOI assignment was normal; the
 * DOI below is for the modern SIAM Classics reissue.
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
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

using mpfrxx::mpfr_class;
using mpfrxx::mpc_class;

mpc_class make_complex(mpfr_class const& real_value,
                        mpfr_class const& imag_value) {
    return mpc_class(real_value, imag_value);
}

mpc_class make_real(mpfr_class const& real_value) {
    return make_complex(real_value, mpfr_class(0));
}

std::vector<mpfr_class> wilkinson_coefficients(std::size_t degree) {
    std::vector<mpfr_class> coefficients(1, mpfr_class(1));

    for (std::size_t root = 1; root <= degree; ++root) {
        std::vector<mpfr_class> next(coefficients.size() + 1, mpfr_class(0));
        mpfr_class root_value(static_cast<unsigned long>(root));
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            next[i] -= coefficients[i] * root_value;
            next[i + 1] += coefficients[i];
        }
        coefficients = next;
    }

    return coefficients;
}

mpc_class evaluate_polynomial(std::vector<mpfr_class> const& coefficients,
                               mpc_class const& z) {
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

mpc_class evaluate_derivative(std::vector<mpfr_class> const& coefficients,
                               mpc_class const& z) {
    if (coefficients.size() < 2) {
        return make_real(mpfr_class(0));
    }

    std::size_t degree = coefficients.size() - 1;
    mpc_class value = make_real(
        coefficients[degree] *
            mpfr_class(static_cast<unsigned long>(degree)));

    for (std::size_t k = degree - 1; k > 0; --k) {
        value *= z;
        mpfr_class derivative_coefficient =
            coefficients[k] *
            mpfr_class(static_cast<unsigned long>(k));
        value += derivative_coefficient;
    }
    return value;
}

std::vector<mpc_class> initial_wilkinson_guesses(std::size_t degree) {
    std::vector<mpc_class> guesses;
    guesses.reserve(degree);

    mpfr_class imag_scale("0.01");
    for (std::size_t i = 1; i <= degree; ++i) {
        mpfr_class real(static_cast<unsigned long>(i));
        mpfr_class imag =
            imag_scale / mpfr_class(static_cast<unsigned long>(i));
        guesses.push_back(make_complex(real, imag));
    }
    return guesses;
}

std::vector<mpc_class> solve_with_aberth(
    std::vector<mpfr_class> const& coefficients,
    std::vector<mpc_class> roots, mpfr_class const& tolerance,
    int max_iterations) {
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
                    mpc_class separation = roots[i] - roots[j];
                    repulsion += one / separation;
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
        if (iteration <= 6 || iteration % 5 == 0) {
            std::cout << "iteration " << std::setw(2) << iteration
                      << ", max correction = " << max_update << '\n';
        }
        if (max_update < tolerance) {
            break;
        }
    }

    return roots;
}

void sort_by_real_part(std::vector<mpc_class>& roots) {
    std::sort(roots.begin(), roots.end(),
              [](mpc_class const& a, mpc_class const& b) {
                  return mpfrxx::real(a) < mpfrxx::real(b);
              });
}

void print_roots(char const* title, std::vector<mpc_class> roots,
                 std::vector<mpfr_class> const& coefficients) {
    sort_by_real_part(roots);
    std::cout << '\n' << title << '\n';
    std::cout << " index                 root"
                 "                         |f(root)|\n";

    for (std::size_t i = 0; i < roots.size(); ++i) {
        mpc_class residual = evaluate_polynomial(coefficients, roots[i]);
        std::cout << std::setw(5) << (i + 1) << "  " << roots[i]
                  << "  " << mpfrxx::abs(residual) << '\n';
    }
}

void print_root_shift(std::vector<mpc_class> exact_roots,
                      std::vector<mpc_class> perturbed_roots) {
    sort_by_real_part(exact_roots);
    sort_by_real_part(perturbed_roots);

    std::cout << "\nRoot movement after perturbing the x^19 coefficient\n";
    std::cout << " index            |delta root|\n";
    for (std::size_t i = 0; i < exact_roots.size(); ++i) {
        mpfr_class shift = mpfrxx::abs(perturbed_roots[i] - exact_roots[i]);
        std::cout << std::setw(5) << (i + 1) << "  " << shift << '\n';
    }
}

}  // namespace

int main() {
    constexpr std::size_t degree = 20;
    constexpr int decimal_digits = 60;

    std::vector<mpfr_class> exact = wilkinson_coefficients(degree);
    std::vector<mpfr_class> perturbed = exact;
    perturbed[degree - 1] += mpfr_class("1e-10");

    mpfr_class tolerance =
        mpfrxx::exp2(-mpfr_class(mpfrxx::default_precision_bits() / 2));

    std::cout << std::scientific << std::setprecision(decimal_digits);
    std::cout << "Wilkinson polynomial example using mpfrxx::mpc_class\n";
    std::cout << "W_20(x) = product_{k=1}^{20} (x-k)\n";
    std::cout << "The second solve perturbs only the x^19 coefficient by"
                 " 1e-10.\n\n";

    std::cout << "Solving exact W_20\n";
    std::vector<mpc_class> exact_roots = solve_with_aberth(
        exact, initial_wilkinson_guesses(degree), tolerance, 40);

    std::cout << "\nSolving perturbed W_20\n";
    std::vector<mpc_class> perturbed_roots = solve_with_aberth(
        perturbed, initial_wilkinson_guesses(degree), tolerance, 40);

    print_roots("Exact polynomial roots", exact_roots, exact);
    print_roots("Perturbed polynomial roots", perturbed_roots, perturbed);
    print_root_shift(exact_roots, perturbed_roots);

    return 0;
}
