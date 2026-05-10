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

#include <gmpfrxx_mkII.h>

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

void assert_mpf_equal(const gmpxx::mpf_class& got, const gmpxx::mpf_class& expected)
{
    assert(mpf_cmp(got.mpf_data(), expected.mpf_data()) == 0);
}

void assert_mpfr_equal(const mpfrxx::mpfr_class& got, const mpfrxx::mpfr_class& expected)
{
    assert(mpfr_cmp(got.mpfr_data(), expected.mpfr_data()) == 0);
}

void test_compile_time_surface()
{
    static_assert(std::is_default_constructible_v<gmpxx::mpf_class>);
    static_assert(std::is_copy_constructible_v<gmpxx::mpf_class>);
    static_assert(std::is_move_constructible_v<gmpxx::mpf_class>);
    static_assert(std::is_nothrow_move_constructible_v<gmpxx::mpf_class>);
    static_assert(std::is_copy_assignable_v<gmpxx::mpf_class>);
    static_assert(std::is_move_assignable_v<gmpxx::mpf_class>);
    static_assert(noexcept(std::declval<gmpxx::mpf_class&>() = std::declval<gmpxx::mpf_class&&>()));
    static_assert(std::is_constructible_v<gmpxx::mpf_class, double>);
    static_assert(std::is_convertible_v<double, gmpxx::mpf_class>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, double, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, int>);
    static_assert(std::is_convertible_v<int, gmpxx::mpf_class>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, int, mp_bitcnt_t>);
    static_assert(std::is_convertible_v<gmpxx::mpz_class, gmpxx::mpf_class>);
    static_assert(std::is_convertible_v<gmpxx::mpq_class, gmpxx::mpf_class>);
    using neg_mpz_expr = decltype(-std::declval<const gmpxx::mpz_class&>());
    using neg_mpq_expr = decltype(-std::declval<const gmpxx::mpq_class&>());
    static_assert(std::is_convertible_v<neg_mpz_expr, gmpxx::mpf_class>);
    static_assert(std::is_convertible_v<neg_mpq_expr, gmpxx::mpf_class>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, long long, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, std::uint64_t, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, const char*, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, std::string, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, mpf_srcptr>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, mpf_srcptr, mp_bitcnt_t>);
    static_assert(std::is_constructible_v<gmpxx::mpf_class, const gmpxx::mpf_class&, mp_bitcnt_t>);
    static_assert(!std::is_constructible_v<gmpxx::mpf_class, bool>);
    static_assert(!std::is_constructible_v<gmpxx::mpf_class, bool, mp_bitcnt_t>);

    static_assert(std::is_default_constructible_v<gmpxx::mpfc_class>);
    static_assert(std::is_copy_constructible_v<gmpxx::mpfc_class>);
    static_assert(std::is_move_constructible_v<gmpxx::mpfc_class>);
    static_assert(std::is_nothrow_move_constructible_v<gmpxx::mpfc_class>);
    static_assert(std::is_copy_assignable_v<gmpxx::mpfc_class>);
    static_assert(std::is_move_assignable_v<gmpxx::mpfc_class>);
    static_assert(noexcept(std::declval<gmpxx::mpfc_class&>() = std::declval<gmpxx::mpfc_class&&>()));

    static_assert(std::is_default_constructible_v<gmpxx::mpz_class>);
    static_assert(std::is_nothrow_default_constructible_v<gmpxx::mpz_class>);
    static_assert(std::is_copy_constructible_v<gmpxx::mpz_class>);
    static_assert(std::is_nothrow_move_constructible_v<gmpxx::mpz_class>);
    static_assert(std::is_copy_assignable_v<gmpxx::mpz_class>);
    static_assert(std::is_move_assignable_v<gmpxx::mpz_class>);
    static_assert(noexcept(std::declval<gmpxx::mpz_class&>() = std::declval<gmpxx::mpz_class&&>()));
    static_assert(std::is_constructible_v<gmpxx::mpz_class, int>);
    static_assert(!std::is_constructible_v<gmpxx::mpz_class, bool>);

    static_assert(std::is_default_constructible_v<gmpxx::mpq_class>);
    static_assert(std::is_copy_constructible_v<gmpxx::mpq_class>);
    static_assert(std::is_nothrow_move_constructible_v<gmpxx::mpq_class>);
    static_assert(std::is_copy_assignable_v<gmpxx::mpq_class>);
    static_assert(std::is_move_assignable_v<gmpxx::mpq_class>);
    static_assert(noexcept(std::declval<gmpxx::mpq_class&>() = std::declval<gmpxx::mpq_class&&>()));
    static_assert(noexcept(std::declval<gmpxx::mpq_class&>() = std::declval<gmpxx::mpz_class&&>()));
    static_assert(std::is_constructible_v<gmpxx::mpq_class, int>);
    static_assert(std::is_constructible_v<gmpxx::mpq_class, int, int>);
    static_assert(!std::is_constructible_v<gmpxx::mpq_class, bool>);

    static_assert(std::is_default_constructible_v<mpfrxx::mpfr_class>);
    static_assert(std::is_copy_constructible_v<mpfrxx::mpfr_class>);
    static_assert(std::is_move_constructible_v<mpfrxx::mpfr_class>);
    static_assert(std::is_nothrow_move_constructible_v<mpfrxx::mpfr_class>);
    static_assert(std::is_copy_assignable_v<mpfrxx::mpfr_class>);
    static_assert(std::is_move_assignable_v<mpfrxx::mpfr_class>);
    static_assert(noexcept(std::declval<mpfrxx::mpfr_class&>() = std::declval<mpfrxx::mpfr_class&&>()));
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, double>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, double, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, int>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, int, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, long long, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, std::uint64_t, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, const char*, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, std::string, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, mpfr_srcptr>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, mpfr_srcptr, mpfr_prec_t>);
    static_assert(std::is_constructible_v<mpfrxx::mpfr_class, const mpfrxx::mpfr_class&, mpfr_prec_t>);
    static_assert(!std::is_constructible_v<mpfrxx::mpfr_class, bool>);
    static_assert(!std::is_constructible_v<mpfrxx::mpfr_class, bool, mpfr_prec_t>);
}

void test_default_constructor_value_zero()
{
    gmpxx::mpf_class value;
    assert(mpf_sgn(value.mpf_data()) == 0);
    assert(value.precision() >= gmpxx::default_mpf_precision_bits());
}

void test_bare_integral_constructor_is_value_not_precision()
{
    gmpxx::mpf_class value(384);
    gmpxx::mpf_class expected(384, gmpxx::default_mpf_precision_bits());

    assert(value.precision() >= gmpxx::default_mpf_precision_bits());
    assert_mpf_equal(value, expected);
}

void test_zero_with_explicit_precision_replaces_precision_only_ctor()
{
    const mp_bitcnt_t requested_precision = 384;

    gmpxx::mpf_class value(0.0, requested_precision);

    assert(mpf_sgn(value.mpf_data()) == 0);
    assert(value.precision() >= requested_precision);
}

void test_integral_constructor_with_explicit_precision()
{
    const mp_bitcnt_t requested_precision = 384;

    gmpxx::mpf_class zero(0, requested_precision);
    assert(mpf_sgn(zero.mpf_data()) == 0);
    assert(zero.precision() >= requested_precision);

    gmpxx::mpf_class negative(-42, requested_precision);
    gmpxx::mpf_class negative_expected(0.0, requested_precision);
    mpf_set_si(negative_expected.mpf_data(), -42);
    assert(negative.precision() == negative_expected.precision());
    assert_mpf_equal(negative, negative_expected);

    const std::uint64_t large = std::numeric_limits<std::uint64_t>::max();
    gmpxx::mpf_class unsigned_value(large, requested_precision);
    gmpxx::mpz_class large_z(large);
    gmpxx::mpf_class unsigned_expected(0.0, requested_precision);
    mpf_set_z(unsigned_expected.mpf_data(), large_z.mpz_data());
    assert(unsigned_value.precision() == unsigned_expected.precision());
    assert_mpf_equal(unsigned_value, unsigned_expected);
}

void test_zq_mpf_implicit_conversions()
{
    gmpxx::mpz_class z = 42;
    gmpxx::mpq_class q(33, 2);

    gmpxx::mpf_class from_z = z;
    gmpxx::mpf_class from_q = q;
    gmpxx::mpf_class from_neg_z = -z;
    gmpxx::mpf_class from_neg_q = -q;

    assert_mpf_equal(from_z, gmpxx::mpf_class(42));
    assert_mpf_equal(from_q, gmpxx::mpf_class(gmpxx::mpq_class(33, 2)));
    assert_mpf_equal(from_neg_z, gmpxx::mpf_class(-42));
    assert_mpf_equal(from_neg_q, gmpxx::mpf_class(gmpxx::mpq_class(-33, 2)));
}

void test_mpf_vector_reallocation_uses_nothrow_move_surface()
{
    static_assert(std::is_nothrow_move_constructible_v<gmpxx::mpf_class>);

    std::vector<gmpxx::mpf_class> values;
    values.reserve(1);
    values.emplace_back("1.25", 200);
    const gmpxx::mpf_class expected_first(values.front());

    const std::size_t old_capacity = values.capacity();
    while (values.capacity() == old_capacity) {
        values.emplace_back("2.5", 200);
    }

    assert_mpf_equal(values.front(), expected_first);
    assert(values.front().precision() == expected_first.precision());
}

void test_mpq_vector_reallocation_uses_nothrow_move_surface()
{
    static_assert(std::is_nothrow_move_constructible_v<gmpxx::mpq_class>);

    std::vector<gmpxx::mpq_class> values;
    values.reserve(1);
    values.emplace_back(3, 7);
    const gmpxx::mpq_class expected_first(values.front());

    const std::size_t old_capacity = values.capacity();
    while (values.capacity() == old_capacity) {
        values.emplace_back(5, 11);
    }

    assert(values.front() == expected_first);
}

void test_expression_constructor_with_explicit_precision()
{
    gmpxx::mpf_class a("1.25", 128);
    gmpxx::mpf_class b("2.5", 512);
    const mp_bitcnt_t requested_precision = 320;

    gmpxx::mpf_class value(a + b, requested_precision);
    gmpxx::mpf_class expected(0.0, requested_precision);
    mpf_add(expected.mpf_data(), a.mpf_data(), b.mpf_data());

    assert(value.precision() == expected.precision());
    assert_mpf_equal(value, expected);
}

void test_raw_mpf_t_constructor()
{
    mpf_t raw;
    mpf_init2(raw, 192);
    const int rc = mpf_set_str(raw, "0.0390625", 10);
    assert(rc == 0);

    gmpxx::mpf_class value(raw);
    assert(value.precision() == mpf_get_prec(raw));
    assert(mpf_cmp(value.mpf_data(), raw) == 0);

    gmpxx::mpf_class with_precision(raw, 384);
    assert(with_precision.precision() >= 384);
    assert(mpf_cmp(with_precision.mpf_data(), raw) == 0);

    mpf_clear(raw);
}

void test_copy_and_move()
{
    gmpxx::mpf_class original("3.1415926535", 384);
    gmpxx::mpf_class copy(original);
    assert(copy.precision() == original.precision());
    assert_mpf_equal(copy, original);

    gmpxx::mpf_class resized(original, 128);
    gmpxx::mpf_class resized_expected(0.0, 128);
    mpf_set(resized_expected.mpf_data(), original.mpf_data());
    assert(resized.precision() == resized_expected.precision());
    assert_mpf_equal(resized, resized_expected);

    gmpxx::mpf_class moved(std::move(original));
    assert_mpf_equal(moved, copy);
}

void test_assignment_preserves_destination_precision()
{
    gmpxx::mpf_class source("-2.5", 512);
    gmpxx::mpf_class destination("1.25", 64);
    const mp_bitcnt_t old_precision = destination.precision();

    destination = source;
    assert(destination.precision() == old_precision);
    assert_mpf_equal(destination, gmpxx::mpf_class("-2.5", old_precision));

    destination = 3.1415926535;
    assert(destination.precision() == old_precision);
    assert_mpf_equal(destination, gmpxx::mpf_class(3.1415926535, old_precision));

    destination = -42;
    assert(destination.precision() == old_precision);
    assert_mpf_equal(destination, gmpxx::mpf_class(-42, old_precision));

    destination = "1.4142135624";
    assert(destination.precision() == old_precision);
    assert_mpf_equal(destination, gmpxx::mpf_class("1.4142135624", old_precision));

    const std::string text = "2.7182818284";
    destination = text;
    assert(destination.precision() == old_precision);
    assert_mpf_equal(destination, gmpxx::mpf_class(text, old_precision));
}

void test_move_assignment_precision_paths()
{
    gmpxx::mpf_class same_source("3.75", 256);
    gmpxx::mpf_class same_destination("-1.5", same_source.precision());
    gmpxx::mpf_class same_expected(same_source);
    const mp_bitcnt_t same_precision = same_destination.precision();

    same_destination = std::move(same_source);
    assert(same_destination.precision() == same_precision);
    assert_mpf_equal(same_destination, same_expected);

    gmpxx::mpf_class source("2.5", 512);
    gmpxx::mpf_class destination("1.25", 64);
    const mp_bitcnt_t destination_precision = destination.precision();
    destination = std::move(source);
    assert(destination.precision() == destination_precision);
    assert_mpf_equal(destination, gmpxx::mpf_class("2.5", destination_precision));
}

void test_hex_string_constructor_with_explicit_base()
{
    const char* hex_digits = "3.243F6A8885A308D313198A2E03707344A4093822299F31D008";
    gmpxx::mpf_class value(hex_digits, 512, 16);

    gmpxx::mpf_class expected(0.0, 512);
    const int rc = mpf_set_str(expected.mpf_data(), hex_digits, 16);
    assert(rc == 0);

    assert(value.precision() == expected.precision());
    assert_mpf_equal(value, expected);
}

void test_mpf_swap_member_and_free_function()
{
    gmpxx::mpf_class a("123.456", 192);
    gmpxx::mpf_class b("789.012", 320);
    gmpxx::mpf_class a_original(a);
    gmpxx::mpf_class b_original(b);

    a.swap(b);
    assert(a.precision() == b_original.precision());
    assert(b.precision() == a_original.precision());
    assert_mpf_equal(a, b_original);
    assert_mpf_equal(b, a_original);

    swap(a, b);
    assert(a.precision() == a_original.precision());
    assert(b.precision() == b_original.precision());
    assert_mpf_equal(a, a_original);
    assert_mpf_equal(b, b_original);
}

void test_mpfr_default_constructor_value_zero()
{
    mpfrxx::mpfr_class value;
    assert(mpfr_zero_p(value.mpfr_data()) != 0);
    assert(value.precision() >= mpfrxx::default_precision_bits());
}

void test_mpfr_bare_integral_constructor_is_value_not_precision()
{
    mpfrxx::mpfr_class value(384);
    mpfrxx::mpfr_class expected(384, mpfrxx::default_precision_bits());

    assert(value.precision() >= mpfrxx::default_precision_bits());
    assert_mpfr_equal(value, expected);
}

void test_mpfr_zero_with_explicit_precision_replaces_precision_only_ctor()
{
    const mpfr_prec_t requested_precision = 384;

    mpfrxx::mpfr_class value(0.0, requested_precision);

    assert(mpfr_zero_p(value.mpfr_data()) != 0);
    assert(value.precision() == requested_precision);
}

void test_mpfr_integral_constructor_with_explicit_precision()
{
    const mpfr_prec_t requested_precision = 384;

    mpfrxx::mpfr_class zero(0, requested_precision);
    assert(mpfr_zero_p(zero.mpfr_data()) != 0);
    assert(zero.precision() == requested_precision);

    mpfrxx::mpfr_class negative(-42, requested_precision);
    mpfrxx::mpfr_class negative_expected(0.0, requested_precision);
    mpfr_set_si(negative_expected.mpfr_data(), -42, mpfrxx::default_rounding_mode());
    assert(negative.precision() == negative_expected.precision());
    assert_mpfr_equal(negative, negative_expected);

    const std::uint64_t large = std::numeric_limits<std::uint64_t>::max();
    mpfrxx::mpfr_class unsigned_value(large, requested_precision);
    gmpxx::mpz_class large_z(large);
    mpfrxx::mpfr_class unsigned_expected(0.0, requested_precision);
    mpfr_set_z(unsigned_expected.mpfr_data(), large_z.mpz_data(), mpfrxx::default_rounding_mode());
    assert(unsigned_value.precision() == unsigned_expected.precision());
    assert_mpfr_equal(unsigned_value, unsigned_expected);
}

void test_mpfr_expression_constructor_with_explicit_precision()
{
    mpfrxx::mpfr_class a("1.25", 128);
    mpfrxx::mpfr_class b("2.5", 512);
    const mpfr_prec_t requested_precision = 320;

    mpfrxx::mpfr_class value(a + b, requested_precision);
    mpfrxx::mpfr_class expected(0.0, requested_precision);
    mpfr_add(expected.mpfr_data(), a.mpfr_data(), b.mpfr_data(), mpfrxx::default_rounding_mode());

    assert(value.precision() == expected.precision());
    assert_mpfr_equal(value, expected);
}

void test_raw_mpfr_t_constructor()
{
    mpfr_t raw;
    mpfr_init2(raw, 192);
    const int rc = mpfr_set_str(raw, "0.0390625", 10, mpfrxx::default_rounding_mode());
    assert(rc == 0);

    mpfrxx::mpfr_class value(raw);
    assert(value.precision() == mpfr_get_prec(raw));
    assert(mpfr_cmp(value.mpfr_data(), raw) == 0);

    mpfrxx::mpfr_class with_precision(raw, 384);
    assert(with_precision.precision() == 384);
    assert(mpfr_cmp(with_precision.mpfr_data(), raw) == 0);

    mpfr_clear(raw);
}

void test_mpfr_copy_and_move()
{
    mpfrxx::mpfr_class original("3.1415926535", 384);
    mpfrxx::mpfr_class copy(original);
    assert(copy.precision() == original.precision());
    assert_mpfr_equal(copy, original);

    mpfrxx::mpfr_class resized(original, 128);
    mpfrxx::mpfr_class resized_expected(0.0, 128);
    mpfr_set(resized_expected.mpfr_data(), original.mpfr_data(), mpfrxx::default_rounding_mode());
    assert(resized.precision() == resized_expected.precision());
    assert_mpfr_equal(resized, resized_expected);

    mpfrxx::mpfr_class moved(std::move(original));
    assert_mpfr_equal(moved, copy);
}

void test_mpfr_assignment_preserves_destination_precision()
{
    mpfrxx::mpfr_class source("-2.5", 512);
    mpfrxx::mpfr_class destination("1.25", 64);
    const mpfr_prec_t old_precision = destination.precision();

    destination = source;
    assert(destination.precision() == old_precision);
    assert_mpfr_equal(destination, mpfrxx::mpfr_class("-2.5", old_precision));

    destination = 3.1415926535;
    assert(destination.precision() == old_precision);
    assert_mpfr_equal(destination, mpfrxx::mpfr_class(3.1415926535, old_precision));

    destination = -42;
    assert(destination.precision() == old_precision);
    assert_mpfr_equal(destination, mpfrxx::mpfr_class(-42, old_precision));

    destination = "1.4142135624";
    assert(destination.precision() == old_precision);
    assert_mpfr_equal(destination, mpfrxx::mpfr_class("1.4142135624", old_precision));

    const std::string text = "2.7182818284";
    destination = text;
    assert(destination.precision() == old_precision);
    assert_mpfr_equal(destination, mpfrxx::mpfr_class(text, old_precision));
}

void test_mpfr_move_assignment_precision_paths()
{
    mpfrxx::mpfr_class same_source("3.75", 256);
    mpfrxx::mpfr_class same_destination("-1.5", same_source.precision());
    mpfrxx::mpfr_class same_expected(same_source);
    const mpfr_prec_t same_precision = same_destination.precision();

    same_destination = std::move(same_source);
    assert(same_destination.precision() == same_precision);
    assert_mpfr_equal(same_destination, same_expected);

    mpfrxx::mpfr_class source("2.5", 512);
    mpfrxx::mpfr_class destination("1.25", 64);
    const mpfr_prec_t destination_precision = destination.precision();
    destination = std::move(source);
    assert(destination.precision() == destination_precision);
    assert_mpfr_equal(destination, mpfrxx::mpfr_class("2.5", destination_precision));
}

void test_mpfr_hex_string_constructor_with_explicit_base()
{
    const char* hex_digits = "3.243F6A8885A308D313198A2E03707344A4093822299F31D008";
    mpfrxx::mpfr_class value(hex_digits, 512, 16);

    mpfrxx::mpfr_class expected(0.0, 512);
    const int rc = mpfr_set_str(expected.mpfr_data(), hex_digits, 16, mpfrxx::default_rounding_mode());
    assert(rc == 0);

    assert(value.precision() == expected.precision());
    assert_mpfr_equal(value, expected);
}

void test_mpfr_swap_member_and_free_function()
{
    mpfrxx::mpfr_class a("123.456", 192);
    mpfrxx::mpfr_class b("789.012", 320);
    mpfrxx::mpfr_class a_original(a);
    mpfrxx::mpfr_class b_original(b);

    a.swap(b);
    assert(a.precision() == b_original.precision());
    assert(b.precision() == a_original.precision());
    assert_mpfr_equal(a, b_original);
    assert_mpfr_equal(b, a_original);

    swap(a, b);
    assert(a.precision() == a_original.precision());
    assert(b.precision() == b_original.precision());
    assert_mpfr_equal(a, a_original);
    assert_mpfr_equal(b, b_original);
}

void test_mpz_construction_copy_move_assignment_and_swap()
{
    gmpxx::mpz_class zero;
    assert(zero == gmpxx::mpz_class(std::int64_t{0}));

    gmpxx::mpz_class from_int(-123);
    assert(from_int == gmpxx::mpz_class(std::int64_t{-123}));

    const std::int32_t i32 = INT32_C(-0x12345678);
    gmpxx::mpz_class from_i32(i32);
    assert(from_i32.get_str() == std::to_string(i32));

    const std::uint32_t u32 = UINT32_C(0xfedcba98);
    gmpxx::mpz_class from_u32(u32);
    assert(from_u32.get_str() == std::to_string(u32));

    gmpxx::mpz_class assigned_from_i32;
    assigned_from_i32 = i32;
    assert(assigned_from_i32 == from_i32);

    gmpxx::mpz_class assigned_from_u32;
    assigned_from_u32 = u32;
    assert(assigned_from_u32 == from_u32);

    gmpxx::mpz_class original("12345678901234567890");
    gmpxx::mpz_class copied(original);
    assert(copied == original);

    gmpxx::mpz_class moved(std::move(original));
    assert(moved == copied);

    gmpxx::mpz_class assigned;
    assigned = copied;
    assert(assigned == copied);

    gmpxx::mpz_class move_assigned;
    move_assigned = std::move(moved);
    assert(move_assigned == copied);

    gmpxx::mpz_class a("123456");
    gmpxx::mpz_class b("789012");
    a.swap(b);
    assert(a == gmpxx::mpz_class("789012"));
    assert(b == gmpxx::mpz_class("123456"));

    swap(a, b);
    assert(a == gmpxx::mpz_class("123456"));
    assert(b == gmpxx::mpz_class("789012"));
}

void test_mpq_construction_copy_move_assignment_and_swap()
{
    gmpxx::mpq_class zero;
    assert(zero == gmpxx::mpq_class(std::int64_t{0}));

    gmpxx::mpq_class from_int_pair(0, 1);
    assert(from_int_pair == zero);

    gmpxx::mpq_class original("355/113");
    gmpxx::mpq_class copied(original);
    assert(copied == original);

    gmpxx::mpq_class moved(std::move(original));
    assert(moved == copied);

    gmpxx::mpq_class assigned;
    assigned = copied;
    assert(assigned == copied);

    gmpxx::mpq_class move_assigned;
    move_assigned = std::move(moved);
    assert(move_assigned == copied);

    gmpxx::mpq_class a("1/2");
    gmpxx::mpq_class b("-3/4");
    a.swap(b);
    assert(a == gmpxx::mpq_class("-3/4"));
    assert(b == gmpxx::mpq_class("1/2"));

    swap(a, b);
    assert(a == gmpxx::mpq_class("1/2"));
    assert(b == gmpxx::mpq_class("-3/4"));
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_default_constructor_value_zero();
    test_bare_integral_constructor_is_value_not_precision();
    test_zero_with_explicit_precision_replaces_precision_only_ctor();
    test_integral_constructor_with_explicit_precision();
    test_zq_mpf_implicit_conversions();
    test_mpf_vector_reallocation_uses_nothrow_move_surface();
    test_mpq_vector_reallocation_uses_nothrow_move_surface();
    test_expression_constructor_with_explicit_precision();
    test_raw_mpf_t_constructor();
    test_copy_and_move();
    test_assignment_preserves_destination_precision();
    test_move_assignment_precision_paths();
    test_hex_string_constructor_with_explicit_base();
    test_mpf_swap_member_and_free_function();
    test_mpfr_default_constructor_value_zero();
    test_mpfr_bare_integral_constructor_is_value_not_precision();
    test_mpfr_zero_with_explicit_precision_replaces_precision_only_ctor();
    test_mpfr_integral_constructor_with_explicit_precision();
    test_mpfr_expression_constructor_with_explicit_precision();
    test_raw_mpfr_t_constructor();
    test_mpfr_copy_and_move();
    test_mpfr_assignment_preserves_destination_precision();
    test_mpfr_move_assignment_precision_paths();
    test_mpfr_hex_string_constructor_with_explicit_base();
    test_mpfr_swap_member_and_free_function();
    test_mpz_construction_copy_move_assignment_and_swap();
    test_mpq_construction_copy_move_assignment_and_swap();
    return 0;
}
