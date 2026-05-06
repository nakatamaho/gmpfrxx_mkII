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
 * Example 13: SIAM 100-Digit Challenge Problem 1.
 *
 * The original endpoint-singular, infinitely oscillatory integral is
 *
 *     lim_{eps -> 0+} integral_eps^1 cos(x^{-1} log(x)) / x dx.
 *
 * With t = -log(x), this becomes
 *
 *     integral_0^infinity cos(t exp(t)) dt.
 *
 * Direct real-axis quadrature is unpleasant: the frequency grows without
 * bound.  A standard way to make the problem numerical is to integrate
 * exp(i z exp(z)) on a rectangular contour and move the infinite ray to
 * z = s + i pi/2.  Taking real parts gives two ordinary real integrals:
 *
 *     integral_0^{pi/2} exp(-y cos(y)) sin(y sin(y)) dy
 *   + integral_0^infinity exp(-s exp(s)) cos((pi/2) exp(s)) ds.
 *
 * The second integral decays double-exponentially.  This example uses
 * composite Simpson quadrature on the two transformed integrals.  The point
 * is not that Simpson's rule is special; the point is that the correct
 * transformation changes an essentially impossible real-axis integral into a
 * stable high-precision computation.
 *
 * Primary reference:
 *
 * - Folkmar Bornemann, Dirk Laurie, Stan Wagon, and Jorg Waldvogel,
 *   "The SIAM 100-Digit Challenge: A Study in High-Accuracy Numerical
 *   Computing", SIAM, 2004.
 *   DOI: 10.1137/1.9780898717969
 */

#include <mpfrxx_mkII.h>

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

using mpfrxx::mpfr_class;

struct options {
    int decimal_digits = 40;
    int first_panels = 128;
    int max_panels = 1024;
    int reference_panels = 2048;
    int upper_tail = 6;
    bool reference_panels_was_set = false;
};

void print_usage(const char* program)
{
    std::cout
        << "Usage: " << program << " [options]\n"
        << "\n"
        << "SIAM 100-Digit Challenge Problem 1 example.\n"
        << "Computes lim_{eps->0+} integral_eps^1 "
           "cos(x^{-1} log x)/x dx\n"
        << "after t=-log(x) and contour deformation.\n"
        << "\n"
        << "Options:\n"
        << "  --help                 Show this help and exit.\n"
        << "  --digits N             Decimal digits to print/use "
           "(default: 40).\n"
        << "  --first-panels N       First Simpson panel count "
           "(default: 128).\n"
        << "  --max-panels N         Largest reported panel count "
           "(default: 1024).\n"
        << "                         If --reference-panels is omitted, "
           "the reference is\n"
        << "                         raised to at least 2*N.\n"
        << "  --reference-panels N   Simpson panel count used as reference "
           "(default: 2048).\n"
        << "  --tail N               Upper limit for the shifted ray integral "
           "(default: 6).\n"
        << "\n"
        << "Examples:\n"
        << "  " << program << "\n"
        << "  " << program << " --max-panels 2048\n"
        << "  " << program
        << " --digits 60 --max-panels 4096 --reference-panels 8192\n";
}

int parse_positive_int(const char* option, const char* text)
{
    try {
        std::string value(text);
        std::size_t parsed = 0;
        int result = std::stoi(value, &parsed);
        if (parsed != value.size() || result <= 0) {
            throw std::invalid_argument("not a positive integer");
        }
        return result;
    } catch (const std::exception&) {
        throw std::invalid_argument(std::string(option) +
                                    " expects a positive integer");
    }
}

const char* require_value(int argc, char** argv, int& index)
{
    if (index + 1 >= argc) {
        throw std::invalid_argument(std::string(argv[index]) +
                                    " requires a value");
    }
    ++index;
    return argv[index];
}

options parse_options(int argc, char** argv)
{
    options result;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else if (arg == "--digits") {
            result.decimal_digits =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--first-panels") {
            result.first_panels =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--max-panels") {
            result.max_panels =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else if (arg == "--reference-panels") {
            result.reference_panels =
                parse_positive_int(argv[i], require_value(argc, argv, i));
            result.reference_panels_was_set = true;
        } else if (arg == "--tail") {
            result.upper_tail =
                parse_positive_int(argv[i], require_value(argc, argv, i));
        } else {
            throw std::invalid_argument("unknown option: " + arg);
        }
    }

    if (result.first_panels > result.max_panels) {
        throw std::invalid_argument(
            "--first-panels must be <= --max-panels");
    }
    if (!result.reference_panels_was_set &&
        result.reference_panels < result.max_panels * 2) {
        result.reference_panels = result.max_panels * 2;
    }
    if (result.reference_panels < result.max_panels) {
        throw std::invalid_argument(
            "--reference-panels must be >= --max-panels");
    }

    return result;
}

mpfr_prec_t bits_for_decimal_digits(int digits, int guard_bits)
{
    double raw_bits = std::ceil(static_cast<double>(digits) * std::log2(10.0));
    return static_cast<mpfr_prec_t>(raw_bits) +
           static_cast<mpfr_prec_t>(guard_bits);
}

template <typename Function>
mpfr_class simpson_integral(Function function, const mpfr_class& lower,
                            const mpfr_class& upper, int panels,
                            mpfr_prec_t precision)
{
    if ((panels % 2) != 0) {
        ++panels;
    }

    mpfr_class h = (upper - lower) / mpfr_class(panels, precision);
    mpfr_class sum = function(lower, precision) + function(upper, precision);

    for (int i = 1; i < panels; ++i) {
        mpfr_class x = lower + h * mpfr_class(i, precision);
        mpfr_class weight((i % 2) == 0 ? 2 : 4, precision);
        sum += weight * function(x, precision);
    }

    return sum * h / mpfr_class(3, precision);
}

mpfr_class vertical_integrand(const mpfr_class& y, mpfr_prec_t precision)
{
    (void)precision;
    return mpfrxx::exp(-y * mpfrxx::cos(y)) *
           mpfrxx::sin(y * mpfrxx::sin(y));
}

mpfr_class shifted_ray_integrand(const mpfr_class& s, mpfr_prec_t precision)
{
    mpfr_class exp_s = mpfrxx::exp(s);
    mpfr_class half_pi = mpfrxx::const_pi(precision) / mpfr_class(2, precision);
    return mpfrxx::exp(-s * exp_s) * mpfrxx::cos(half_pi * exp_s);
}

mpfr_class siam_problem_one(int panels, int upper_tail_extent,
                            mpfr_prec_t precision)
{
    mpfr_class zero(0, precision);
    mpfr_class half_pi = mpfrxx::const_pi(precision) / mpfr_class(2, precision);
    mpfr_class upper_tail(upper_tail_extent, precision);

    mpfr_class vertical = simpson_integral(
        vertical_integrand, zero, half_pi, panels, precision);
    mpfr_class shifted = simpson_integral(
        shifted_ray_integrand, zero, upper_tail, panels, precision);
    return vertical + shifted;
}

void run_case(const options& opts)
{
    mpfr_prec_t precision = bits_for_decimal_digits(opts.decimal_digits, 128);
    mpfrxx::set_default_precision_bits(precision);

    mpfr_class reference =
        siam_problem_one(opts.reference_panels, opts.upper_tail, precision);
    int label_width = 24;
    int value_width = opts.decimal_digits + 8;

    std::cout << "\nprecision target = " << opts.decimal_digits
              << " decimal digits\n";
    std::cout << std::setw(label_width) << "case" << "  "
              << std::setw(value_width) << "value" << "  "
              << "|delta from reference|\n";

    for (int panels = opts.first_panels; panels <= opts.max_panels;
         panels *= 2) {
        mpfr_class value = siam_problem_one(panels, opts.upper_tail, precision);
        std::cout << std::setw(label_width) << panels << "  "
                  << std::setw(value_width) << value << "  "
                  << mpfrxx::abs(value - reference) << '\n';
        if (panels > opts.max_panels / 2) {
            break;
        }
    }

    std::string reference_label =
        "reference(" + std::to_string(opts.reference_panels) + ")";
    std::cout << std::setw(label_width) << reference_label << "  "
              << std::setw(value_width) << reference << '\n';
    std::cout << " shifted-ray tail upper limit = " << opts.upper_tail
              << '\n';
}

}  // namespace

int main(int argc, char** argv)
{
    options opts;
    try {
        opts = parse_options(argc, argv);
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << "\n\n";
        print_usage(argv[0]);
        return 1;
    }

    std::cout << std::scientific << std::setprecision(opts.decimal_digits);
    std::cout << "SIAM 100-Digit Challenge Problem 1 example using mpfr_class\n";
    std::cout << "lim_{eps->0+} integral_eps^1 cos(x^{-1} log x)/x dx\n";
    std::cout << "Computed after t=-log(x) and contour deformation.\n";

    run_case(opts);

    return 0;
}
