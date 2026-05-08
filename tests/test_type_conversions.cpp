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
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace {

void require_mpf_equal(const gmpxx::mpf_class& got, const gmpxx::mpf_class& expected)
{
    if (mpf_cmp(got.mpf_data(), expected.mpf_data()) != 0) {
        std::abort();
    }
}

#if defined(__SIZEOF_INT128__)
std::string uint128_to_string(__uint128_t value)
{
    if (value == 0) {
        return "0";
    }

    std::string result;
    while (value != 0) {
        result.push_back(static_cast<char>('0' + static_cast<unsigned>(value % 10)));
        value /= 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::string int128_to_string(__int128_t value)
{
    if (value < 0) {
        const __uint128_t magnitude = static_cast<__uint128_t>(-(value + 1)) + 1u;
        return "-" + uint128_to_string(magnitude);
    }
    return uint128_to_string(static_cast<__uint128_t>(value));
}
#endif

void test_compile_time_surface()
{
    static_assert(std::is_constructible<gmpxx::mpz_class, double>::value, "");
    static_assert(!std::is_convertible<double, gmpxx::mpz_class>::value, "");
    static_assert(std::is_constructible<gmpxx::mpz_class, std::int64_t>::value, "");
    static_assert(std::is_constructible<gmpxx::mpz_class, std::uint64_t>::value, "");
    static_assert(std::is_constructible<gmpxx::mpq_class, double>::value, "");
    static_assert(std::is_constructible<gmpxx::mpq_class, const gmpxx::mpz_class&>::value, "");
    static_assert(std::is_constructible<gmpxx::mpq_class, const gmpxx::mpf_class&>::value, "");
    static_assert(std::is_constructible<gmpxx::mpq_class,
                                        const gmpxx::mpz_class&,
                                        const gmpxx::mpz_class&>::value,
                  "");
    static_assert(std::is_constructible<gmpxx::mpz_class, mpz_srcptr>::value, "");
    static_assert(std::is_constructible<gmpxx::mpq_class, mpq_srcptr>::value, "");
    static_assert(std::is_constructible<gmpxx::mpz_class, const gmpxx::mpf_class&>::value, "");
    static_assert(std::is_constructible<gmpxx::mpz_class, const gmpxx::mpq_class&>::value, "");
    static_assert(std::is_constructible<gmpxx::mpf_class, const gmpxx::mpz_class&>::value, "");
    static_assert(std::is_constructible<gmpxx::mpf_class,
                                        const gmpxx::mpz_class&,
                                        mp_bitcnt_t>::value,
                  "");
    static_assert(std::is_constructible<gmpxx::mpf_class, const gmpxx::mpq_class&>::value, "");
    static_assert(std::is_constructible<gmpxx::mpf_class,
                                        const gmpxx::mpq_class&,
                                        mp_bitcnt_t>::value,
                  "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpf_class&>().get_d()), double>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpf_class&>().get_ui()), unsigned long>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpf_class&>().get_si()), signed long>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpf_class&>().get_u64()), std::uint64_t>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpf_class&>().get_i64()), std::int64_t>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpz_class&>().get_d()), double>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpz_class&>().get_ui()), unsigned long>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpz_class&>().get_si()), signed long>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpz_class&>().get_u64()), std::uint64_t>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpz_class&>().get_i64()), std::int64_t>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpq_class&>().get_d()), double>::value, "");
    static_assert(std::is_same<decltype(std::declval<gmpxx::mpq_class&>().get_num_mpz_t()), mpz_ptr>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpq_class&>().get_num_mpz_t()), mpz_srcptr>::value, "");
    static_assert(std::is_same<decltype(std::declval<gmpxx::mpq_class&>().get_den_mpz_t()), mpz_ptr>::value, "");
    static_assert(std::is_same<decltype(std::declval<const gmpxx::mpq_class&>().get_den_mpz_t()), mpz_srcptr>::value, "");

    static_assert(!std::is_convertible<gmpxx::mpz_class, bool>::value, "");
    static_assert(!std::is_convertible<gmpxx::mpq_class, bool>::value, "");
    static_assert(!std::is_convertible<gmpxx::mpf_class, bool>::value, "");

#if defined(__SIZEOF_INT128__)
    static_assert(std::is_constructible<gmpxx::mpz_class, __int128_t>::value, "");
    static_assert(std::is_constructible<gmpxx::mpz_class, __uint128_t>::value, "");
    static_assert(std::is_convertible<__int128_t, gmpxx::mpz_class>::value, "");
    static_assert(std::is_convertible<__uint128_t, gmpxx::mpz_class>::value, "");
    static_assert(std::is_assignable<gmpxx::mpz_class&, __int128_t>::value, "");
    static_assert(std::is_assignable<gmpxx::mpz_class&, __uint128_t>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, __int128_t>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, __uint128_t>::value, "");
#endif
    static_assert(std::is_assignable<gmpxx::mpq_class&, signed char>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, unsigned char>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, int>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, unsigned int>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, long>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, unsigned long>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, double>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, const char*>::value, "");
    static_assert(std::is_assignable<gmpxx::mpq_class&, const std::string&>::value, "");
}

void test_mpz_integer_double_and_string_assignment()
{
    const std::int64_t i64 = INT64_C(-9223372036854775807);
    const std::uint64_t u64 = UINT64_C(18446744073709551614);

    gmpxx::mpz_class from_i64(i64);
    gmpxx::mpz_class from_u64(u64);
    if (from_i64.get_str() != "-9223372036854775807" ||
        from_u64.get_str() != "18446744073709551614") {
        std::abort();
    }
    if (from_i64.get_i64() != i64 || from_u64.get_u64() != u64) {
        std::abort();
    }
    if (gmpxx::mpz_class(std::numeric_limits<std::int64_t>::min()).get_i64() !=
        std::numeric_limits<std::int64_t>::min()) {
        std::abort();
    }

    gmpxx::mpz_class assigned;
    assigned = i64;
    if (assigned != from_i64) {
        std::abort();
    }
    assigned = u64;
    if (assigned != from_u64) {
        std::abort();
    }
    assigned = 31415926535.0;
    if (assigned != gmpxx::mpz_class("31415926535")) {
        std::abort();
    }

    assigned = "14142135624";
    if (assigned != gmpxx::mpz_class("14142135624")) {
        std::abort();
    }
    const std::string text = "31415926535";
    assigned = text;
    if (assigned != gmpxx::mpz_class("31415926535")) {
        std::abort();
    }

    bool threw = false;
    try {
        assigned = "not-an-integer";
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw || assigned != gmpxx::mpz_class("31415926535")) {
        std::abort();
    }
}

void test_int128_conversions()
{
#if defined(__SIZEOF_INT128__)
    const __int128_t signed_value =
        static_cast<__int128_t>(0x0123456789ABCDEFULL) *
        static_cast<__int128_t>(0x0FEDCBA987654321ULL);
    const __int128_t signed_negative = -signed_value;
    const __uint128_t unsigned_value =
        static_cast<__uint128_t>(0xFEDCBA9876543210ULL) *
        static_cast<__uint128_t>(0xFFFFFFFFFFFFFFFFULL);

    gmpxx::mpz_class z(signed_value);
    if (z.get_str() != int128_to_string(signed_value)) {
        std::abort();
    }
    z = signed_negative;
    if (z.get_str() != int128_to_string(signed_negative)) {
        std::abort();
    }
    z = unsigned_value;
    if (z.get_str() != uint128_to_string(unsigned_value)) {
        std::abort();
    }

    gmpxx::mpq_class q;
    q = signed_value;
    if (q != gmpxx::mpq_class(gmpxx::mpz_class(signed_value))) {
        std::abort();
    }
    q = signed_negative;
    if (q != gmpxx::mpq_class(gmpxx::mpz_class(signed_negative))) {
        std::abort();
    }
    q = unsigned_value;
    if (q != gmpxx::mpq_class(gmpxx::mpz_class(unsigned_value))) {
        std::abort();
    }
#endif
}

void test_mpq_scalar_string_and_raw_construction()
{
    gmpxx::mpq_class q;
    q = static_cast<signed char>(-127);
    if (q != gmpxx::mpq_class(std::int64_t{-127})) {
        std::abort();
    }
    q = static_cast<unsigned char>(255);
    if (q != gmpxx::mpq_class(std::int64_t{255})) {
        std::abort();
    }
    q = -12.375;
    if (q != gmpxx::mpq_class("-99/8")) {
        std::abort();
    }
    q = "6/8";
    if (q != gmpxx::mpq_class("3/4")) {
        std::abort();
    }
    const std::string text("10/14");
    q = text;
    if (q != gmpxx::mpq_class("5/7")) {
        std::abort();
    }

    bool threw = false;
    try {
        q = "not-a-rational";
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw || q != gmpxx::mpq_class("5/7")) {
        std::abort();
    }

    mpz_t raw_z;
    mpz_init_set_str(raw_z, "-123456789012345678901234567890", 10);
    gmpxx::mpz_class z(raw_z);
    if (z != gmpxx::mpz_class("-123456789012345678901234567890")) {
        std::abort();
    }
    mpz_clear(raw_z);

    mpq_t raw_q;
    mpq_init(raw_q);
    mpq_set_str(raw_q, "6/8", 10);
    gmpxx::mpq_class from_raw(raw_q);
    if (from_raw != gmpxx::mpq_class("3/4")) {
        std::abort();
    }
    mpq_clear(raw_q);
}

void test_wrapper_construction_and_assignment()
{
    const gmpxx::mpz_class z("-31415926535");
    const gmpxx::mpq_class q("3000/13");

    gmpxx::mpf_class f_default(z);
    if (f_default.get_prec() != gmpxx::default_mpf_precision_bits()) {
        std::abort();
    }
    require_mpf_equal(f_default, gmpxx::mpf_class("-31415926535", f_default.get_prec()));

    gmpxx::mpf_class f_from_z(z, static_cast<mp_bitcnt_t>(512));
    require_mpf_equal(f_from_z, gmpxx::mpf_class("-31415926535", f_from_z.get_prec()));

    gmpxx::mpq_class q_from_z(z);
    if (q_from_z != gmpxx::mpq_class("-31415926535")) {
        std::abort();
    }

    gmpxx::mpz_class z_from_q(q);
    if (z_from_q != gmpxx::mpz_class(230)) {
        std::abort();
    }

    gmpxx::mpf_class f_from_q(q, static_cast<mp_bitcnt_t>(384));
    gmpxx::mpf_class expected_q(0, f_from_q.get_prec());
    mpf_set_q(expected_q.mpf_data(), q.mpq_data());
    require_mpf_equal(f_from_q, expected_q);

    gmpxx::mpf_class f("4.5", 256);
    gmpxx::mpz_class z_assigned;
    z_assigned = f;
    if (z_assigned != gmpxx::mpz_class(4)) {
        std::abort();
    }
    z_assigned = q;
    if (z_assigned != gmpxx::mpz_class(230)) {
        std::abort();
    }

    gmpxx::mpq_class q_assigned;
    q_assigned = f;
    if (q_assigned != gmpxx::mpq_class("9/2")) {
        std::abort();
    }
    q_assigned = z;
    if (q_assigned != gmpxx::mpq_class("-31415926535")) {
        std::abort();
    }

    gmpxx::mpf_class f_assigned("0", 192);
    const mp_bitcnt_t old_prec = f_assigned.get_prec();
    f_assigned = z;
    if (f_assigned.get_prec() != old_prec) {
        std::abort();
    }
    gmpxx::mpf_class expected_z(0, old_prec);
    mpf_set_z(expected_z.mpf_data(), z.mpz_data());
    require_mpf_equal(f_assigned, expected_z);
    f_assigned = q;
    gmpxx::mpf_class expected_assigned_q(0, old_prec);
    mpf_set_q(expected_assigned_q.mpf_data(), q.mpq_data());
    require_mpf_equal(f_assigned, expected_assigned_q);
}

void test_accessors_fit_queries_and_bool()
{
    gmpxx::mpq_class value("3/4");
    if (value.get_num() != gmpxx::mpz_class(3) || value.get_den() != gmpxx::mpz_class(4)) {
        std::abort();
    }
    mpz_set_si(value.get_num_mpz_t(), 6);
    mpz_set_si(value.get_den_mpz_t(), 8);
    value.canonicalize();
    if (value != gmpxx::mpq_class("3/4")) {
        std::abort();
    }
    if (value.get_d() != 0.75 || gmpxx::mpq_class("-3/4").get_d() != -0.75) {
        std::abort();
    }

    gmpxx::mpf_class f("123.456");
    if (f.get_ui() != 123UL || f.get_si() != 123L ||
        f.get_u64() != 123 || f.get_i64() != 123 ||
        !f.fits_sint_p() || !f.fits_ulong_p()) {
        std::abort();
    }
    if (gmpxx::mpf_class("-123.456").get_si() != -123L ||
        gmpxx::mpf_class("999999999999999999999999999999").fits_sint_p()) {
        std::abort();
    }

    gmpxx::mpz_class small("123");
    if (small.get_d() != 123.0 || small.get_si() != 123L || small.get_ui() != 123UL ||
        !small.fits_sint_p() || !small.fits_ushort_p()) {
        std::abort();
    }
    gmpxx::mpz_class negative("-123");
    if (!negative.fits_sint_p() || negative.fits_uint_p()) {
        std::abort();
    }
    gmpxx::mpz_class ushort_max(std::to_string(USHRT_MAX));
    if (!ushort_max.fits_ushort_p()) {
        std::abort();
    }
    ushort_max += 1;
    if (ushort_max.fits_ushort_p()) {
        std::abort();
    }

    const gmpxx::mpz_class z0(0), z1(1);
    const gmpxx::mpq_class q0(0), q1(1);
    const gmpxx::mpf_class f0(0), f1(1);
    if (z0 || q0 || f0 || !z1 || !q1 || !f1) {
        std::abort();
    }
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_mpz_integer_double_and_string_assignment();
    test_int128_conversions();
    test_mpq_scalar_string_and_raw_construction();
    test_wrapper_construction_and_assignment();
    test_accessors_fit_queries_and_bool();
    return 0;
}
