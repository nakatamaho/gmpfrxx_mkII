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

#include <mpfrxx_mkII.h>

#include <algorithm>
#include <cfloat>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <vector>

namespace {

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

mpfr_prec_t scalar_integer_prec(mpfr_prec_t dst_prec)
{
    return std::max(dst_prec, static_cast<mpfr_prec_t>(64));
}

mpfr_prec_t scalar_double_prec(mpfr_prec_t dst_prec)
{
    return std::max(dst_prec, static_cast<mpfr_prec_t>(DBL_MANT_DIG));
}

void set_scalar(mpfr_t dst, std::uint64_t value, mpfr_prec_t precision)
{
    mpfr_set_prec(dst, scalar_integer_prec(precision));
    const mpfrxx::mpz_class integer(value);
    mpfr_set_z(dst, integer.mpz_data(), mpfrxx::mpfr_class::default_rounding());
}

void set_scalar(mpfr_t dst, std::int64_t value, mpfr_prec_t precision)
{
    mpfr_set_prec(dst, scalar_integer_prec(precision));
    const mpfrxx::mpz_class integer(value);
    mpfr_set_z(dst, integer.mpz_data(), mpfrxx::mpfr_class::default_rounding());
}

void set_scalar(mpfr_t dst, double value, mpfr_prec_t precision)
{
    mpfr_set_prec(dst, scalar_double_prec(precision));
    mpfr_set_d(dst, value, mpfrxx::mpfr_class::default_rounding());
}

void apply_ref(mpfr_t ref, const mpfrxx::mpfr_class& value, const mpfr_t scalar, char op, bool scalar_left)
{
    const mpfr_rnd_t rnd = mpfrxx::mpfr_class::default_rounding();
    if (op == '+') {
        mpfr_add(ref, value.mpfr_data(), scalar, rnd);
    } else if (op == '-') {
        scalar_left ? mpfr_sub(ref, scalar, value.mpfr_data(), rnd)
                    : mpfr_sub(ref, value.mpfr_data(), scalar, rnd);
    } else if (op == '*') {
        mpfr_mul(ref, value.mpfr_data(), scalar, rnd);
    } else {
        scalar_left ? mpfr_div(ref, scalar, value.mpfr_data(), rnd)
                    : mpfr_div(ref, value.mpfr_data(), scalar, rnd);
    }
}

template <typename Scalar>
using normalized_scalar_t = std::conditional_t<
    std::is_floating_point_v<std::remove_cv_t<Scalar>>,
    double,
    std::conditional_t<std::is_signed_v<std::remove_cv_t<Scalar>>, std::int64_t, std::uint64_t>>;

template <typename Scalar>
void require_scalar_match(
    const mpfrxx::mpfr_class& got,
    const mpfrxx::mpfr_class& value,
    Scalar scalar,
    char op,
    bool scalar_left)
{
    mpfr_t ref;
    mpfr_t scalar_value;
    mpfr_init2(ref, got.get_prec());
    mpfr_init2(scalar_value, got.get_prec());
    set_scalar(scalar_value, static_cast<normalized_scalar_t<Scalar>>(scalar), got.get_prec());
    apply_ref(ref, value, scalar_value, op, scalar_left);
    if (mpfr_cmp(got.mpfr_data(), ref) != 0) {
        std::abort();
    }
    mpfr_clear(scalar_value);
    mpfr_clear(ref);
}

template <typename Scalar>
void check_scalar(const mpfrxx::mpfr_class& value, Scalar scalar)
{
    {
        mpfrxx::mpfr_class dst("0", 8);
        const mpfr_prec_t old_prec = dst.get_prec();
        dst = value + scalar;
        if (dst.get_prec() != old_prec) {
            std::abort();
        }
        require_scalar_match(dst, value, scalar, '+', false);
    }
    {
        mpfrxx::mpfr_class dst("0", 8);
        const mpfr_prec_t old_prec = dst.get_prec();
        dst = scalar + value;
        if (dst.get_prec() != old_prec) {
            std::abort();
        }
        require_scalar_match(dst, value, scalar, '+', true);
    }

    const auto normalized = static_cast<normalized_scalar_t<Scalar>>(scalar);
    require_scalar_match(mpfrxx::mpfr_class(value + scalar), value, scalar, '+', false);
    require_scalar_match(mpfrxx::mpfr_class(scalar + value), value, scalar, '+', true);
    require_scalar_match(mpfrxx::mpfr_class(value - scalar), value, scalar, '-', false);
    require_scalar_match(mpfrxx::mpfr_class(scalar - value), value, scalar, '-', true);
    require_scalar_match(mpfrxx::mpfr_class(value * scalar), value, scalar, '*', false);
    require_scalar_match(mpfrxx::mpfr_class(scalar * value), value, scalar, '*', true);
    if (normalized != normalized_scalar_t<Scalar>{0}) {
        require_scalar_match(mpfrxx::mpfr_class(value / scalar), value, scalar, '/', false);
        require_scalar_match(mpfrxx::mpfr_class(scalar / value), value, scalar, '/', true);
    }
}

template <typename Scalar>
void check_expression_scalar(const mpfrxx::mpfr_class& a, const mpfrxx::mpfr_class& b, Scalar scalar)
{
    mpfrxx::mpfr_class got1 = (a + b) + scalar;
    mpfrxx::mpfr_class got2 = scalar + (a + b);
    mpfrxx::mpfr_class sum(got1.get_prec());
    mpfr_add(sum.mpfr_data(), a.mpfr_data(), b.mpfr_data(), mpfrxx::mpfr_class::default_rounding());
    require_scalar_match(got1, sum, scalar, '+', false);
    require_scalar_match(got2, sum, scalar, '+', true);
}

void check_increment_decrement()
{
    mpfrxx::mpfr_class value(3.5, static_cast<mpfr_prec_t>(128));
    const mpfr_prec_t precision = value.get_prec();
    mpfrxx::mpfr_class old = value++;
    if (old.get_prec() != precision || value.get_prec() != precision ||
        old.to_double() != 3.5 || value.to_double() != 4.5) {
        std::abort();
    }
    if ((++value).get_prec() != precision || value.to_double() != 5.5) {
        std::abort();
    }
    old = value--;
    if (old.get_prec() != precision || value.get_prec() != precision ||
        old.to_double() != 5.5 || value.to_double() != 4.5) {
        std::abort();
    }
    if ((--value).get_prec() != precision || value.to_double() != 3.5) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto x = mpfrxx::mpfr_class::with_precision(128, 10.0);

    mpfrxx::mpfr_class result = x + 5;
    require_close(result.to_double(), 15.0);

    result = 20u - x;
    require_close(result.to_double(), 10.0);

    result = x * 1.5f;
    require_close(result.to_double(), 15.0);

    result = 2.5 + x / 2;
    require_close(result.to_double(), 7.5);

    result = (x + 2) * (3 + x);
    require_close(result.to_double(), 156.0);

    check_increment_decrement();

    const std::vector<mpfr_prec_t> precisions{8, 64, 256, 1024};
    for (const mpfr_prec_t precision : precisions) {
        const mpfrxx::mpfr_class a("1.75", precision);
        const mpfrxx::mpfr_class b("-2.25", precision);

        check_scalar(a, int{0});
        check_scalar(a, int{1});
        check_scalar(a, int{-1});
        check_scalar(a, std::numeric_limits<int>::max());
        check_scalar(a, std::numeric_limits<int>::min());
        check_scalar(a, long{17});
        check_scalar(a, -19LL);
        check_scalar(a, std::numeric_limits<long long>::max());
        check_scalar(a, std::numeric_limits<std::int64_t>::min());
        check_scalar(a, unsigned{0});
        check_scalar(a, std::numeric_limits<unsigned>::max());
        check_scalar(a, static_cast<std::size_t>(123));
        check_scalar(a, std::numeric_limits<std::uint64_t>::max());
        check_scalar(a, 0.5f);
        check_scalar(a, -0.5);
        check_scalar(a, 1e-100);
        check_scalar(a, 1e+100);

        check_expression_scalar(a, b, 5LL);
        check_expression_scalar(a, b, 0.5);
    }

    return 0;
}
