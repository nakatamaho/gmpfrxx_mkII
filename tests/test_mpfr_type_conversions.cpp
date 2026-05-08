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
 * OR SERVICES; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <mpfrxx_mkII.h>

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace {

void require_mpfr_equal(const mpfrxx::mpfr_class& got, mpfr_srcptr expected)
{
    if (mpfr_cmp(got.mpfr_data(), expected) != 0) {
        std::abort();
    }
}

void test_compile_time_surface()
{
    static_assert(std::is_same<mpfrxx::mpz_class, gmpxx::mpz_class>::value, "");
    static_assert(std::is_same<mpfrxx::mpq_class, gmpxx::mpq_class>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, double>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, double, mpfr_prec_t>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, std::int64_t>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, std::uint64_t>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, const char*>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, const std::string&>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, mpfr_srcptr>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, mpfr_srcptr, mpfr_prec_t>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, const mpfrxx::mpz_class&>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class,
                                        const mpfrxx::mpz_class&,
                                        mpfr_prec_t>::value,
                  "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, const mpfrxx::mpq_class&>::value, "");
    static_assert(std::is_convertible<mpfrxx::mpz_class, mpfrxx::mpfr_class>::value, "");
    static_assert(std::is_convertible<mpfrxx::mpq_class, mpfrxx::mpfr_class>::value, "");
    static_assert(std::is_convertible<int, mpfrxx::mpfr_class>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class,
                                        const mpfrxx::mpq_class&,
                                        mpfr_prec_t>::value,
                  "");
    static_assert(std::is_assignable<mpfrxx::mpfr_class&, const mpfrxx::mpz_class&>::value, "");
    static_assert(std::is_assignable<mpfrxx::mpfr_class&, const mpfrxx::mpq_class&>::value, "");
    static_assert(std::is_assignable<mpfrxx::mpfr_class&, const char*>::value, "");
    static_assert(std::is_assignable<mpfrxx::mpfr_class&, const std::string&>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().to_double()), double>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().get_d()), double>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().get_si()), signed long>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().get_ui()), unsigned long>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().get_i64()), std::int64_t>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().get_u64()), std::uint64_t>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().fits_sint_p()), bool>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().fits_uint_p()), bool>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().fits_slong_p()), bool>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().fits_ulong_p()), bool>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().fits_sshort_p()), bool>::value, "");
    static_assert(std::is_same<decltype(std::declval<const mpfrxx::mpfr_class&>().fits_ushort_p()), bool>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpz_class, mpfrxx::mpq_class>::value, "");
    static_assert(std::is_assignable<mpfrxx::mpz_class&, mpfrxx::mpq_class>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpz_class, mpfrxx::mpz_class>::value, "");
    static_assert(!std::is_convertible<mpfrxx::mpq_class, mpfrxx::mpz_class>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpq_class, decltype(-std::declval<mpfrxx::mpz_class>())>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, decltype(-std::declval<mpfrxx::mpz_class>())>::value, "");
    static_assert(std::is_constructible<mpfrxx::mpfr_class, decltype(-std::declval<mpfrxx::mpq_class>())>::value, "");
    static_assert(!std::is_convertible<mpfrxx::mpfr_class, bool>::value, "");
}

void test_integral_string_and_raw_construction()
{
    const std::int64_t min_i = std::numeric_limits<std::int64_t>::min();
    const std::uint64_t max_u = std::numeric_limits<std::uint64_t>::max();

    mpfrxx::mpfr_class from_i64(min_i, 256);
    mpfrxx::mpfr_class from_u64(max_u, 256);
    if (mpfr_cmp_z(from_i64.mpfr_data(), mpfrxx::mpz_class(min_i).mpz_data()) != 0 ||
        mpfr_cmp_z(from_u64.mpfr_data(), mpfrxx::mpz_class(max_u).mpz_data()) != 0) {
        std::abort();
    }

    mpfrxx::mpfr_class assigned("0", 192);
    assigned = min_i;
    if (mpfr_cmp_z(assigned.mpfr_data(), mpfrxx::mpz_class(min_i).mpz_data()) != 0 ||
        assigned.precision() != 192) {
        std::abort();
    }
    assigned = max_u;
    if (mpfr_cmp_z(assigned.mpfr_data(), mpfrxx::mpz_class(max_u).mpz_data()) != 0 ||
        assigned.precision() != 192) {
        std::abort();
    }
    assigned = "123.5";
    if (assigned.to_double() != 123.5 || assigned.precision() != 192) {
        std::abort();
    }
    const std::string text("-0.25");
    assigned = text;
    if (assigned.to_double() != -0.25 || assigned.precision() != 192) {
        std::abort();
    }

    bool threw = false;
    try {
        assigned = "not-an-mpfr";
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw || assigned.to_double() != -0.25 || assigned.precision() != 192) {
        std::abort();
    }

    mpfr_t raw;
    mpfr_init2(raw, 224);
    mpfr_set_str(raw, "314159.25", 10, mpfrxx::mpfr_class::default_rounding());
    mpfrxx::mpfr_class from_raw(raw);
    if (from_raw.precision() != 224) {
        std::abort();
    }
    require_mpfr_equal(from_raw, raw);

    mpfrxx::mpfr_class from_raw_rounded(raw, 160);
    mpfr_t expected;
    mpfr_init2(expected, 160);
    mpfr_set(expected, raw, mpfrxx::mpfr_class::default_rounding());
    require_mpfr_equal(from_raw_rounded, expected);
    mpfr_clear(expected);
    mpfr_clear(raw);
}

void test_exact_wrapper_construction_and_assignment()
{
    const mpfrxx::mpz_class z("-314159265358979323846");
    const mpfrxx::mpq_class q("355/113");

    mpfrxx::mpfr_class from_z(z);
    if (from_z.precision() != mpfrxx::mpfr_class::default_precision() ||
        mpfr_cmp_z(from_z.mpfr_data(), z.mpz_data()) != 0) {
        std::abort();
    }

    mpfrxx::mpfr_class from_z_prec(z, 384);
    if (from_z_prec.precision() != 384 ||
        mpfr_cmp_z(from_z_prec.mpfr_data(), z.mpz_data()) != 0) {
        std::abort();
    }

    mpfrxx::mpfr_class from_q(q, 448);
    mpfr_t expected_q;
    mpfr_init2(expected_q, 448);
    mpfr_set_q(expected_q, q.mpq_data(), mpfrxx::mpfr_class::default_rounding());
    if (from_q.precision() != 448) {
        std::abort();
    }
    require_mpfr_equal(from_q, expected_q);

    mpfrxx::mpfr_class assigned("0", 192);
    assigned = z;
    if (assigned.precision() != 192 ||
        mpfr_cmp_z(assigned.mpfr_data(), z.mpz_data()) != 0) {
        std::abort();
    }
    assigned = q;
    mpfr_t expected_assigned_q;
    mpfr_init2(expected_assigned_q, 192);
    mpfr_set_q(expected_assigned_q, q.mpq_data(), mpfrxx::mpfr_class::default_rounding());
    require_mpfr_equal(assigned, expected_assigned_q);

    mpfr_clear(expected_assigned_q);
    mpfr_clear(expected_q);
}

void test_accessors_and_bool()
{
    const mpfrxx::mpfr_class zero("0", 128);
    const mpfrxx::mpfr_class one("1", 128);
    if (zero || !static_cast<bool>(one)) {
        std::abort();
    }

    mpfrxx::mpfr_class value("1.25", 192);
    mpfr_exp_t exponent = 0;
    const std::string digits = value.get_str(exponent, 10, 0);
    if (digits.empty() || exponent != 1 || value.to_double() != 1.25) {
        std::abort();
    }

    const mpfrxx::mpfr_class small("123", 192);
    if (small.get_d() != 123.0 || small.get_si() != 123L || small.get_ui() != 123UL ||
        small.get_i64() != 123 || small.get_u64() != 123 ||
        !small.fits_sint_p() || !small.fits_uint_p() ||
        !small.fits_slong_p() || !small.fits_ulong_p() ||
        !small.fits_sshort_p() || !small.fits_ushort_p()) {
        std::abort();
    }

    const mpfrxx::mpfr_class negative("-123", 192);
    if (negative.get_d() != -123.0 || negative.get_si() != -123L ||
        !negative.fits_sint_p() || negative.fits_uint_p()) {
        std::abort();
    }

    const mpfrxx::mpfr_class too_large("1e100", 192);
    if (too_large.fits_sint_p() || too_large.fits_uint_p() ||
        too_large.fits_slong_p() || too_large.fits_ulong_p()) {
        std::abort();
    }
}

int overload_z(mpfrxx::mpz_class)
{
    return 0;
}

int overload_q(mpfrxx::mpq_class)
{
    return 1;
}

int overload_r(mpfrxx::mpfr_class)
{
    return 2;
}

int overload_zq(mpfrxx::mpz_class)
{
    return 0;
}

int overload_zq(mpfrxx::mpq_class)
{
    return 1;
}

int overload_zr(mpfrxx::mpz_class)
{
    return 0;
}

int overload_zr(mpfrxx::mpfr_class)
{
    return 2;
}

int overload_qr(mpfrxx::mpq_class)
{
    return 1;
}

int overload_qr(mpfrxx::mpfr_class)
{
    return 2;
}

int overload_zqr(mpfrxx::mpz_class)
{
    return 0;
}

int overload_zqr(mpfrxx::mpq_class)
{
    return 1;
}

int overload_zqr(mpfrxx::mpfr_class)
{
    return 2;
}

void test_mixed_exact_mpfr_conversion_legality()
{
    mpfrxx::mpz_class z = 42;
    mpfrxx::mpq_class q = 33;
    mpfrxx::mpfr_class r = 18;

    if (overload_z(z) != 0 || overload_z(-z) != 0) {
        std::abort();
    }
    if (overload_q(z) != 1 || overload_q(-z) != 1 ||
        overload_q(q) != 1 || overload_q(-q) != 1) {
        std::abort();
    }
    if (overload_r(z) != 2 || overload_r(-z) != 2 ||
        overload_r(q) != 2 || overload_r(-q) != 2 ||
        overload_r(r) != 2 || overload_r(-r) != 2) {
        std::abort();
    }
    if (overload_zq(z) != 0 || overload_zq(q) != 1 || overload_zq(-q) != 1) {
        std::abort();
    }
    if (overload_zr(z) != 0 || overload_zr(r) != 2 || overload_zr(-r) != 2) {
        std::abort();
    }
    if (overload_qr(q) != 1 || overload_qr(r) != 2 || overload_qr(-r) != 2) {
        std::abort();
    }
    if (overload_zqr(z) != 0 || overload_zqr(q) != 1 ||
        overload_zqr(r) != 2 || overload_zqr(-r) != 2) {
        std::abort();
    }

    if (overload_zqr(mpfrxx::mpz_class(z)) != 0 ||
        overload_zqr(mpfrxx::mpz_class(-z)) != 0 ||
        overload_zqr(mpfrxx::mpz_class(q)) != 0 ||
        overload_zqr(mpfrxx::mpz_class(-q)) != 0 ||
        overload_zqr(static_cast<mpfrxx::mpz_class>(r)) != 0 ||
        overload_zqr(static_cast<mpfrxx::mpz_class>(mpfrxx::mpfr_class(-r))) != 0) {
        std::abort();
    }
    if (overload_zqr(mpfrxx::mpq_class(z)) != 1 ||
        overload_zqr(mpfrxx::mpq_class(-z)) != 1 ||
        overload_zqr(mpfrxx::mpq_class(q)) != 1 ||
        overload_zqr(mpfrxx::mpq_class(-q)) != 1 ||
        overload_zqr(static_cast<mpfrxx::mpq_class>(r)) != 1 ||
        overload_zqr(static_cast<mpfrxx::mpq_class>(mpfrxx::mpfr_class(-r))) != 1) {
        std::abort();
    }
    if (overload_zqr(mpfrxx::mpfr_class(z)) != 2 ||
        overload_zqr(mpfrxx::mpfr_class(-z)) != 2 ||
        overload_zqr(mpfrxx::mpfr_class(q)) != 2 ||
        overload_zqr(mpfrxx::mpfr_class(-q)) != 2 ||
        overload_zqr(mpfrxx::mpfr_class(r)) != 2 ||
        overload_zqr(mpfrxx::mpfr_class(-r)) != 2) {
        std::abort();
    }
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_integral_string_and_raw_construction();
    test_exact_wrapper_construction_and_assignment();
    test_accessors_and_bool();
    test_mixed_exact_mpfr_conversion_legality();
    return 0;
}
