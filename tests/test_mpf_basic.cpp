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

#include <gmpxx_mkII.h>

#include <algorithm>
#include <cfloat>
#include <cstdio>
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

mp_bitcnt_t scalar_integer_prec(mp_bitcnt_t dst_prec)
{
    return std::max(dst_prec, static_cast<mp_bitcnt_t>(64));
}

mp_bitcnt_t scalar_double_prec(mp_bitcnt_t dst_prec)
{
    return std::max(dst_prec, static_cast<mp_bitcnt_t>(DBL_MANT_DIG));
}

std::uint64_t abs_i64(std::int64_t value)
{
    if (value == std::numeric_limits<std::int64_t>::min()) {
        return static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + std::uint64_t{1};
    }
    return value < 0 ? static_cast<std::uint64_t>(-(value + 1)) + std::uint64_t{1}
                     : static_cast<std::uint64_t>(value);
}

void set_uint(mpf_t dst, std::uint64_t value, mp_bitcnt_t precision)
{
    mpf_set_prec(dst, scalar_integer_prec(precision));
    char buffer[24];
    std::snprintf(buffer, sizeof(buffer), "%llu", static_cast<unsigned long long>(value));
    if (mpf_set_str(dst, buffer, 10) != 0) {
        std::abort();
    }
}

void set_scalar(mpf_t dst, std::uint64_t value, mp_bitcnt_t precision)
{
    set_uint(dst, value, precision);
}

void set_scalar(mpf_t dst, std::int64_t value, mp_bitcnt_t precision)
{
    set_uint(dst, abs_i64(value), precision);
    if (value < 0) {
        mpf_neg(dst, dst);
    }
}

void set_scalar(mpf_t dst, double value, mp_bitcnt_t precision)
{
    mpf_set_prec(dst, scalar_double_prec(precision));
    mpf_set_d(dst, value);
}

void apply_ref(mpf_t ref, const gmpxx::mpf_class& value, const mpf_t scalar, char op, bool scalar_left)
{
    if (op == '+') {
        mpf_add(ref, value.mpf_data(), scalar);
    } else if (op == '-') {
        scalar_left ? mpf_sub(ref, scalar, value.mpf_data()) : mpf_sub(ref, value.mpf_data(), scalar);
    } else if (op == '*') {
        mpf_mul(ref, value.mpf_data(), scalar);
    } else {
        scalar_left ? mpf_div(ref, scalar, value.mpf_data()) : mpf_div(ref, value.mpf_data(), scalar);
    }
}

template <typename Scalar>
using normalized_scalar_t = std::conditional_t<
    std::is_floating_point_v<std::remove_cv_t<Scalar>>,
    double,
    std::conditional_t<std::is_signed_v<std::remove_cv_t<Scalar>>, std::int64_t, std::uint64_t>>;

template <typename Scalar>
void require_scalar_match(
    const gmpxx::mpf_class& got,
    const gmpxx::mpf_class& value,
    Scalar scalar,
    char op,
    bool scalar_left)
{
    mpf_t ref;
    mpf_t scalar_value;
    mpf_init2(ref, got.get_prec());
    mpf_init2(scalar_value, got.get_prec());
    set_scalar(scalar_value, static_cast<normalized_scalar_t<Scalar>>(scalar), got.get_prec());
    apply_ref(ref, value, scalar_value, op, scalar_left);
    if (mpf_cmp(got.mpf_data(), ref) != 0) {
        std::abort();
    }
    mpf_clear(scalar_value);
    mpf_clear(ref);
}

template <typename Scalar>
void check_scalar(const gmpxx::mpf_class& value, Scalar scalar)
{
    {
        gmpxx::mpf_class dst("0", 8);
        const mp_bitcnt_t old_prec = dst.get_prec();
        dst = value + scalar;
        if (dst.get_prec() != old_prec) {
            std::abort();
        }
        require_scalar_match(dst, value, scalar, '+', false);
    }
    {
        gmpxx::mpf_class dst("0", 8);
        const mp_bitcnt_t old_prec = dst.get_prec();
        dst = scalar + value;
        if (dst.get_prec() != old_prec) {
            std::abort();
        }
        require_scalar_match(dst, value, scalar, '+', true);
    }

    const auto normalized = static_cast<normalized_scalar_t<Scalar>>(scalar);
    require_scalar_match(gmpxx::mpf_class(value + scalar), value, scalar, '+', false);
    require_scalar_match(gmpxx::mpf_class(scalar + value), value, scalar, '+', true);
    require_scalar_match(gmpxx::mpf_class(value - scalar), value, scalar, '-', false);
    require_scalar_match(gmpxx::mpf_class(scalar - value), value, scalar, '-', true);
    require_scalar_match(gmpxx::mpf_class(value * scalar), value, scalar, '*', false);
    require_scalar_match(gmpxx::mpf_class(scalar * value), value, scalar, '*', true);
    if (normalized != normalized_scalar_t<Scalar>{0}) {
        require_scalar_match(gmpxx::mpf_class(value / scalar), value, scalar, '/', false);
        require_scalar_match(gmpxx::mpf_class(scalar / value), value, scalar, '/', true);
    }
}

template <typename Scalar>
void check_expression_scalar(const gmpxx::mpf_class& a, const gmpxx::mpf_class& b, Scalar scalar)
{
    gmpxx::mpf_class got1 = (a + b) + scalar;
    gmpxx::mpf_class got2 = scalar + (a + b);
    gmpxx::mpf_class sum(got1.get_prec());
    mpf_add(sum.mpf_data(), a.mpf_data(), b.mpf_data());
    require_scalar_match(got1, sum, scalar, '+', false);
    require_scalar_match(got2, sum, scalar, '+', true);
}

void check_increment_decrement()
{
    gmpxx::mpf_class value(3.5, static_cast<mp_bitcnt_t>(128));
    const mp_bitcnt_t precision = value.get_prec();
    gmpxx::mpf_class old = value++;
    if (old.get_prec() != precision || value.get_prec() != precision ||
        mpf_cmp_d(old.mpf_data(), 3.5) != 0 ||
        mpf_cmp_d(value.mpf_data(), 4.5) != 0) {
        std::abort();
    }
    if ((++value).get_prec() != precision || mpf_cmp_d(value.mpf_data(), 5.5) != 0) {
        std::abort();
    }
    old = value--;
    if (old.get_prec() != precision || value.get_prec() != precision ||
        mpf_cmp_d(old.mpf_data(), 5.5) != 0 ||
        mpf_cmp_d(value.mpf_data(), 4.5) != 0) {
        std::abort();
    }
    if ((--value).get_prec() != precision || mpf_cmp_d(value.mpf_data(), 3.5) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto a = gmpxx::mpf_class::with_precision(128, 1.5);
    auto b = gmpxx::mpf_class::with_precision(128, 2.0);
    auto c = gmpxx::mpf_class::with_precision(128, 3.0);

    gmpxx::mpf_class result = a + b * c;
    require_close(result.to_double(), 7.5);

    result = c / b - a;
    require_close(result.to_double(), 0.0);

    result = -a + +c + 2;
    require_close(result.to_double(), 3.5);

    result = 2.5 + b * 4;
    require_close(result.to_double(), 10.5);

    check_increment_decrement();

    const std::vector<mp_bitcnt_t> precisions{8, 64, 256, 1024};
    for (const mp_bitcnt_t precision : precisions) {
        const gmpxx::mpf_class x("1.75", precision);
        const gmpxx::mpf_class y("-2.25", precision);

        check_scalar(x, int{0});
        check_scalar(x, int{1});
        check_scalar(x, int{-1});
        check_scalar(x, std::numeric_limits<int>::max());
        check_scalar(x, std::numeric_limits<int>::min());
        check_scalar(x, long{17});
        check_scalar(x, -19LL);
        check_scalar(x, std::numeric_limits<long long>::max());
        check_scalar(x, std::numeric_limits<std::int64_t>::min());
        check_scalar(x, unsigned{0});
        check_scalar(x, std::numeric_limits<unsigned>::max());
        check_scalar(x, static_cast<std::size_t>(123));
        check_scalar(x, std::numeric_limits<std::uint64_t>::max());
        check_scalar(x, 0.5f);
        check_scalar(x, -0.5);
        check_scalar(x, 1e-100);
        check_scalar(x, 1e+100);

        check_expression_scalar(x, y, 5LL);
        check_expression_scalar(x, y, 0.5);
    }

    return 0;
}
