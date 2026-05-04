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

#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace {

mpfrxx::mpz_class z(std::int64_t value)
{
    return mpfrxx::mpz_class(value);
}

void assert_mpz_equal(const mpfrxx::mpz_class& lhs, const mpfrxx::mpz_class& rhs)
{
    assert(mpz_cmp(lhs.mpz_data(), rhs.mpz_data()) == 0);
}

void assert_mpfr_equal(const mpfrxx::mpfr_class& lhs, const mpfrxx::mpfr_class& rhs)
{
    assert(mpfr_cmp(lhs.mpfr_data(), rhs.mpfr_data()) == 0);
}

void test_compile_time_surface()
{
    static_assert(std::is_default_constructible_v<mpfrxx::gmp_randclass>);
    static_assert(std::is_constructible_v<mpfrxx::gmp_randclass, void (*)(gmp_randstate_t)>);
    static_assert(!std::is_copy_constructible_v<mpfrxx::gmp_randclass>);
    static_assert(!std::is_copy_assignable_v<mpfrxx::gmp_randclass>);
    static_assert(!std::is_move_constructible_v<mpfrxx::gmp_randclass>);
    static_assert(!std::is_move_assignable_v<mpfrxx::gmp_randclass>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<mpfrxx::random_mpfr_expr>);
    static_assert(std::is_same_v<mpfrxx::random_mpfr_expr::result_type, mpfrxx::mpfr_class>);
}

void test_deterministic_seed_with_ui()
{
    mpfrxx::gmp_randclass r1(gmp_randinit_default);
    mpfrxx::gmp_randclass r2(gmp_randinit_default);

    r1.seed(12345ul);
    r2.seed(12345ul);

    assert_mpz_equal(
        r1.get_z_bits(static_cast<mp_bitcnt_t>(192)),
        r2.get_z_bits(static_cast<mp_bitcnt_t>(192)));

    const mpfrxx::mpfr_class f1 = r1.get_fr(static_cast<mpfr_prec_t>(256));
    const mpfrxx::mpfr_class f2 = r2.get_fr(static_cast<mpfr_prec_t>(256));
    assert(f1.get_prec() == 256);
    assert(f2.get_prec() == f1.get_prec());
    assert_mpfr_equal(f1, f2);
    assert(mpfr_sgn(f1.mpfr_data()) >= 0);
    assert(mpfr_cmp_ui(f1.mpfr_data(), 1) < 0);
}

void test_deterministic_seed_with_mpz()
{
    mpfrxx::gmp_randclass r1;
    mpfrxx::gmp_randclass r2;
    const mpfrxx::mpz_class seed("123456789012345678901234567890");

    r1.seed(seed);
    r2.seed(seed);

    assert_mpfr_equal(
        mpfrxx::mpfr_class(r1.get_fr_uniform(static_cast<mpfr_prec_t>(160))),
        mpfrxx::mpfr_class(r2.get_fr_uniform(static_cast<mpfr_prec_t>(160))));
}

void test_z_range_and_bits()
{
    mpfrxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(7ul);

    const mpfrxx::mpz_class limit("1000000000000000000000000000000");
    for (int i = 0; i < 16; ++i) {
        const mpfrxx::mpz_class value = r.get_z_range(limit);
        assert(mpz_sgn(value.mpz_data()) >= 0);
        assert(mpz_cmp(value.mpz_data(), limit.mpz_data()) < 0);
    }

    const mpfrxx::mpz_class bits = z(96);
    const mpfrxx::mpz_class value = r.get_z_bits(bits);
    assert(mpz_sgn(value.mpz_data()) >= 0);

    bool threw = false;
    try {
        (void)r.get_z_bits(z(-1));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        (void)r.get_z_range(z(0));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

void test_fr_precision_forms()
{
    mpfrxx::gmp_randclass r1;
    mpfrxx::gmp_randclass r2;
    r1.seed(17ul);
    r2.seed(17ul);

    const mpfrxx::mpfr_class prototype("0", static_cast<mpfr_prec_t>(384));
    const mpfrxx::mpfr_class by_object = r1.get_fr(prototype);
    const mpfrxx::mpfr_class by_prec = r2.get_fr(prototype.get_prec());

    assert(by_object.get_prec() == prototype.get_prec());
    assert(by_prec.get_prec() == prototype.get_prec());
    assert_mpfr_equal(by_object, by_prec);

    mpfrxx::set_default_precision_bits(512);
    mpfrxx::gmp_randclass r_default;
    r_default.seed(19ul);
    const mpfrxx::mpfr_class by_default = r_default.get_fr();
    assert(by_default.get_prec() == mpfrxx::default_precision_bits());

    mpfrxx::gmp_randclass r3;
    mpfrxx::gmp_randclass r4;
    r3.seed(23ul);
    r4.seed(23ul);

    mpfrxx::mpfr_class assigned("0", static_cast<mpfr_prec_t>(448));
    const mpfrxx::mpfr_class expected = r4.get_fr(assigned.get_prec());
    assigned = r3.get_fr();

    assert(assigned.get_prec() == expected.get_prec());
    assert_mpfr_equal(assigned, expected);

    mpfrxx::gmp_randclass r5;
    r5.seed(431ul);
    mpfrxx::mpfr_class assigned_from_prec;
    const mpfr_prec_t assigned_old_prec = assigned_from_prec.get_prec();
    assigned_from_prec = r5.get_fr(static_cast<mpfr_prec_t>(211));
    assert(assigned_from_prec.get_prec() == assigned_old_prec);

    const mpfrxx::mpfr_class constructed_from_prec(r5.get_fr(static_cast<mpfr_prec_t>(211)));
    assert(constructed_from_prec.get_prec() == 211);
}

void test_distribution_entry_points()
{
    mpfrxx::gmp_randclass r;
    r.seed(37ul);

    const mpfrxx::mpfr_class urandomb = r.get_fr(static_cast<mpfr_prec_t>(192));
    assert(urandomb.get_prec() == 192);
    assert(mpfr_sgn(urandomb.mpfr_data()) >= 0);
    assert(mpfr_cmp_ui(urandomb.mpfr_data(), 1) < 0);

    const mpfrxx::mpfr_class uniform = r.get_fr_uniform(static_cast<mpfr_prec_t>(192));
    assert(uniform.get_prec() == 192);
    assert(mpfr_sgn(uniform.mpfr_data()) >= 0);
    assert(mpfr_cmp_ui(uniform.mpfr_data(), 1) <= 0);

    const mpfrxx::mpfr_class normal = r.get_fr_normal(static_cast<mpfr_prec_t>(192));
    assert(normal.get_prec() == 192);
    assert(mpfr_number_p(normal.mpfr_data()) != 0);

    const mpfrxx::mpfr_class exponential = r.get_fr_exponential(static_cast<mpfr_prec_t>(192));
    assert(exponential.get_prec() == 192);
    assert(mpfr_sgn(exponential.mpfr_data()) >= 0);

    mpfrxx::gmp_randclass alias_a;
    mpfrxx::gmp_randclass alias_b;
    alias_a.seed(41ul);
    alias_b.seed(41ul);
    assert_mpfr_equal(
        mpfrxx::mpfr_class(alias_a.get_fr_urandomb(static_cast<mpfr_prec_t>(160))),
        mpfrxx::mpfr_class(alias_b.get_fr(static_cast<mpfr_prec_t>(160))));

    alias_a.seed(43ul);
    alias_b.seed(43ul);
    assert_mpfr_equal(
        mpfrxx::mpfr_class(alias_a.get_fr_urandom(static_cast<mpfr_prec_t>(160))),
        mpfrxx::mpfr_class(alias_b.get_fr_uniform(static_cast<mpfr_prec_t>(160))));

    alias_a.seed(47ul);
    alias_b.seed(47ul);
    assert_mpfr_equal(
        mpfrxx::mpfr_class(alias_a.get_fr_nrandom(static_cast<mpfr_prec_t>(160))),
        mpfrxx::mpfr_class(alias_b.get_fr_normal(static_cast<mpfr_prec_t>(160))));

    alias_a.seed(53ul);
    alias_b.seed(53ul);
    assert_mpfr_equal(
        mpfrxx::mpfr_class(alias_a.get_fr_erandom(static_cast<mpfr_prec_t>(160))),
        mpfrxx::mpfr_class(alias_b.get_fr_exponential(static_cast<mpfr_prec_t>(160))));
}

void test_lc_constructors()
{
    mpfrxx::gmp_randclass by_default_function(gmp_randinit_default);
    by_default_function.seed(37ul);
    assert(mpz_sgn(by_default_function.get_z_bits(static_cast<mp_bitcnt_t>(43)).mpz_data()) >= 0);

    mpfrxx::gmp_randclass by_mt(gmp_randinit_mt);
    by_mt.seed(41ul);
    assert(mpz_sgn(by_mt.get_z_bits(static_cast<mp_bitcnt_t>(47)).mpz_data()) >= 0);

    mpfrxx::gmp_randclass by_size(gmp_randinit_lc_2exp_size, static_cast<mp_bitcnt_t>(48));
    by_size.seed(43ul);
    assert(mpz_sgn(by_size.get_z_bits(static_cast<mp_bitcnt_t>(29)).mpz_data()) >= 0);

    const mpfrxx::mpz_class a = z(9);
    mpfrxx::gmp_randclass by_params(gmp_randinit_lc_2exp, a, 7ul, static_cast<mp_bitcnt_t>(40));
    by_params.seed(47ul);
    assert(mpz_sgn(by_params.get_z_bits(static_cast<mp_bitcnt_t>(31)).mpz_data()) >= 0);

    mpfrxx::gmp_randclass obsolete(GMP_RAND_ALG_LC, static_cast<mp_bitcnt_t>(56));
    obsolete.seed(53ul);
    assert(mpz_sgn(obsolete.get_z_bits(static_cast<mp_bitcnt_t>(37)).mpz_data()) >= 0);

    mpfrxx::gmp_randclass obsolete_default(GMP_RAND_ALG_DEFAULT, static_cast<mp_bitcnt_t>(64));
    obsolete_default.seed(59ul);
    assert(mpz_sgn(obsolete_default.get_z_bits(static_cast<mp_bitcnt_t>(41)).mpz_data()) >= 0);

    bool threw = false;
    try {
        mpfrxx::gmp_randclass too_large(gmp_randinit_lc_2exp_size, static_cast<mp_bitcnt_t>(129));
        (void)too_large;
    } catch (const std::length_error&) {
        threw = true;
    }
    assert(threw);
}

void test_z_range_statistics()
{
    constexpr int sample_count = 4096;
    constexpr double range = 1000.0;
    constexpr double expected_mean = (range - 1.0) / 2.0;
    constexpr double expected_variance = (range * range - 1.0) / 12.0;

    mpfrxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(2028ul);
    const mpfrxx::mpz_class limit(1000);

    double sum = 0.0;
    double sumsq = 0.0;
    for (int i = 0; i < sample_count; ++i) {
        const mpfrxx::mpz_class value = r.get_z_range(limit);
        assert(mpz_sgn(value.mpz_data()) >= 0);
        assert(mpz_cmp(value.mpz_data(), limit.mpz_data()) < 0);
        const double x = static_cast<double>(mpz_get_ui(value.mpz_data()));
        sum += x;
        sumsq += x * x;
    }

    const double mean = sum / sample_count;
    const double variance = sumsq / sample_count - mean * mean;
    assert(std::abs(mean - expected_mean) < 25.0);
    assert(std::abs(variance - expected_variance) < 12000.0);
}

void test_uniform_statistics()
{
    constexpr int sample_count = 4096;
    constexpr double expected_mean = 0.5;
    constexpr double expected_variance = 1.0 / 12.0;

    mpfrxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(2029ul);

    double sum = 0.0;
    double sumsq = 0.0;
    for (int i = 0; i < sample_count; ++i) {
        const mpfrxx::mpfr_class value = r.get_fr(static_cast<mpfr_prec_t>(256));
        assert(mpfr_sgn(value.mpfr_data()) >= 0);
        assert(mpfr_cmp_ui(value.mpfr_data(), 1) < 0);
        const double x = value.to_double();
        sum += x;
        sumsq += x * x;
    }

    const double mean = sum / sample_count;
    const double variance = sumsq / sample_count - mean * mean;
    assert(std::abs(mean - expected_mean) < 0.035);
    assert(std::abs(variance - expected_variance) < 0.02);
}

void test_normal_statistics()
{
    constexpr int sample_count = 8192;
    constexpr double expected_mean = 0.0;
    constexpr double expected_variance = 1.0;

    mpfrxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(2030ul);

    double sum = 0.0;
    double sumsq = 0.0;
    for (int i = 0; i < sample_count; ++i) {
        const mpfrxx::mpfr_class value = r.get_fr_normal(static_cast<mpfr_prec_t>(256));
        assert(mpfr_number_p(value.mpfr_data()) != 0);
        const double x = value.to_double();
        sum += x;
        sumsq += x * x;
    }

    const double mean = sum / sample_count;
    const double variance = sumsq / sample_count - mean * mean;
    assert(std::abs(mean - expected_mean) < 0.08);
    assert(std::abs(variance - expected_variance) < 0.16);
}

void test_exponential_statistics()
{
    constexpr int sample_count = 8192;
    constexpr double expected_mean = 1.0;
    constexpr double expected_variance = 1.0;

    mpfrxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(2031ul);

    double sum = 0.0;
    double sumsq = 0.0;
    for (int i = 0; i < sample_count; ++i) {
        const mpfrxx::mpfr_class value = r.get_fr_exponential(static_cast<mpfr_prec_t>(256));
        assert(mpfr_sgn(value.mpfr_data()) >= 0);
        const double x = value.to_double();
        sum += x;
        sumsq += x * x;
    }

    const double mean = sum / sample_count;
    const double variance = sumsq / sample_count - mean * mean;
    assert(std::abs(mean - expected_mean) < 0.08);
    assert(std::abs(variance - expected_variance) < 0.22);
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_deterministic_seed_with_ui();
    test_deterministic_seed_with_mpz();
    test_z_range_and_bits();
    test_fr_precision_forms();
    test_distribution_entry_points();
    test_lc_constructors();
    test_z_range_statistics();
    test_uniform_statistics();
    test_normal_statistics();
    test_exponential_statistics();
    return 0;
}
