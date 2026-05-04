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

#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace {

gmpxx::mpz_class z(std::int64_t value)
{
    return gmpxx::mpz_class(value);
}

void assert_mpz_equal(const gmpxx::mpz_class& lhs, const gmpxx::mpz_class& rhs)
{
    assert(mpz_cmp(lhs.mpz_data(), rhs.mpz_data()) == 0);
}

void assert_mpf_equal(const gmpxx::mpf_class& lhs, const gmpxx::mpf_class& rhs)
{
    assert(mpf_cmp(lhs.get_mpf_t(), rhs.get_mpf_t()) == 0);
}

void test_compile_time_surface()
{
    static_assert(std::is_default_constructible_v<gmpxx::gmp_randclass>);
    static_assert(std::is_constructible_v<gmpxx::gmp_randclass, void (*)(gmp_randstate_t)>);
    static_assert(!std::is_copy_constructible_v<gmpxx::gmp_randclass>);
    static_assert(!std::is_copy_assignable_v<gmpxx::gmp_randclass>);
    static_assert(!std::is_move_constructible_v<gmpxx::gmp_randclass>);
    static_assert(!std::is_move_assignable_v<gmpxx::gmp_randclass>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<gmpxx::random_mpf_expr>);
    static_assert(std::is_same_v<gmpxx::random_mpf_expr::result_type, gmpxx::mpf_class>);
}

void test_deterministic_seed_with_ui()
{
    gmpxx::gmp_randclass r1(gmp_randinit_default);
    gmpxx::gmp_randclass r2(gmp_randinit_default);

    r1.seed(12345ul);
    r2.seed(12345ul);

    const gmpxx::mpz_class z1 = r1.get_z_bits(static_cast<mp_bitcnt_t>(192));
    const gmpxx::mpz_class z2 = r2.get_z_bits(static_cast<mp_bitcnt_t>(192));
    assert_mpz_equal(z1, z2);

    const gmpxx::mpf_class f1 = r1.get_f(static_cast<mp_bitcnt_t>(256));
    const gmpxx::mpf_class f2 = r2.get_f(static_cast<mp_bitcnt_t>(256));
    assert(f1.get_prec() >= 256);
    assert(f2.get_prec() == f1.get_prec());
    assert_mpf_equal(f1, f2);
    assert(mpf_sgn(f1.get_mpf_t()) >= 0);
    assert(mpf_cmp_ui(f1.get_mpf_t(), 1) < 0);
}

void test_deterministic_seed_with_mpz()
{
    gmpxx::gmp_randclass r1;
    gmpxx::gmp_randclass r2;
    const gmpxx::mpz_class seed("123456789012345678901234567890");

    r1.seed(seed);
    r2.seed(seed);

    assert_mpz_equal(
        r1.get_z_bits(static_cast<mp_bitcnt_t>(160)),
        r2.get_z_bits(static_cast<mp_bitcnt_t>(160)));
}

void test_z_range()
{
    gmpxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(7ul);

    const gmpxx::mpz_class limit("1000000000000000000000000000000");
    for (int i = 0; i < 16; ++i) {
        const gmpxx::mpz_class value = r.get_z_range(limit);
        assert(mpz_sgn(value.mpz_data()) >= 0);
        assert(mpz_cmp(value.mpz_data(), limit.mpz_data()) < 0);
    }

    bool threw = false;
    try {
        (void)r.get_z_range(z(0));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

void test_z_bits_from_mpz()
{
    gmpxx::gmp_randclass r;
    r.seed(99ul);

    const gmpxx::mpz_class bits = z(96);
    const gmpxx::mpz_class value = r.get_z_bits(bits);
    assert(mpz_sgn(value.mpz_data()) >= 0);

    bool threw = false;
    try {
        (void)r.get_z_bits(z(-1));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

void test_f_precision_forms()
{
    gmpxx::gmp_randclass r1;
    gmpxx::gmp_randclass r2;
    r1.seed(17ul);
    r2.seed(17ul);

    const gmpxx::mpf_class prototype("0", static_cast<mp_bitcnt_t>(384));
    const gmpxx::mpf_class by_object = r1.get_f(prototype);
    const gmpxx::mpf_class by_prec = r2.get_f(prototype.get_prec());

    assert(by_object.get_prec() == prototype.get_prec());
    assert(by_prec.get_prec() == prototype.get_prec());
    assert_mpf_equal(by_object, by_prec);

    gmpxx::set_default_mpf_precision_bits(512);
    gmpxx::gmp_randclass r_default;
    r_default.seed(19ul);
    const gmpxx::mpf_class by_default = r_default.get_f();
    assert(by_default.get_prec() >= gmpxx::default_mpf_precision_bits());

    gmpxx::gmp_randclass r3;
    gmpxx::gmp_randclass r4;
    r3.seed(23ul);
    r4.seed(23ul);

    gmpxx::mpf_class assigned("0", static_cast<mp_bitcnt_t>(448));
    const gmpxx::mpf_class expected = r4.get_f(assigned.get_prec());
    assigned = r3.get_f();

    assert(assigned.get_prec() == expected.get_prec());
    assert_mpf_equal(assigned, expected);

    gmpxx::gmp_randclass r5;
    r5.seed(431ul);
    gmpxx::mpf_class assigned_from_prec;
    const mp_bitcnt_t assigned_old_prec = assigned_from_prec.get_prec();
    assigned_from_prec = r5.get_f(static_cast<mp_bitcnt_t>(211));
    assert(assigned_from_prec.get_prec() == assigned_old_prec);

    const gmpxx::mpf_class constructed_from_prec(r5.get_f(static_cast<mp_bitcnt_t>(211)));
    assert(constructed_from_prec.get_prec() >= 211);
}

void test_lc_constructors()
{
    gmpxx::gmp_randclass by_default_function(gmp_randinit_default);
    by_default_function.seed(37ul);
    assert(mpz_sgn(by_default_function.get_z_bits(static_cast<mp_bitcnt_t>(43)).mpz_data()) >= 0);

    gmpxx::gmp_randclass by_mt(gmp_randinit_mt);
    by_mt.seed(41ul);
    assert(mpz_sgn(by_mt.get_z_bits(static_cast<mp_bitcnt_t>(47)).mpz_data()) >= 0);

    gmpxx::gmp_randclass by_size(gmp_randinit_lc_2exp_size, static_cast<mp_bitcnt_t>(48));
    by_size.seed(43ul);
    assert(mpz_sgn(by_size.get_z_bits(static_cast<mp_bitcnt_t>(29)).mpz_data()) >= 0);

    const gmpxx::mpz_class a = z(9);
    gmpxx::gmp_randclass by_params(gmp_randinit_lc_2exp, a, 7ul, static_cast<mp_bitcnt_t>(40));
    by_params.seed(47ul);
    assert(mpz_sgn(by_params.get_z_bits(static_cast<mp_bitcnt_t>(31)).mpz_data()) >= 0);

    gmpxx::gmp_randclass obsolete(GMP_RAND_ALG_LC, static_cast<mp_bitcnt_t>(56));
    obsolete.seed(53ul);
    assert(mpz_sgn(obsolete.get_z_bits(static_cast<mp_bitcnt_t>(37)).mpz_data()) >= 0);

    gmpxx::gmp_randclass obsolete_default(GMP_RAND_ALG_DEFAULT, static_cast<mp_bitcnt_t>(64));
    obsolete_default.seed(59ul);
    assert(mpz_sgn(obsolete_default.get_z_bits(static_cast<mp_bitcnt_t>(41)).mpz_data()) >= 0);

    gmpxx::gmp_randclass obsolete_zero(static_cast<gmp_randalg_t>(0), static_cast<mp_bitcnt_t>(72));
    obsolete_zero.seed(61ul);
    assert(mpz_sgn(obsolete_zero.get_z_bits(static_cast<mp_bitcnt_t>(19)).mpz_data()) >= 0);

    bool threw = false;
    try {
        gmpxx::gmp_randclass too_large(gmp_randinit_lc_2exp_size, static_cast<mp_bitcnt_t>(129));
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

    gmpxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(2026ul);
    const gmpxx::mpz_class limit(1000);

    double sum = 0.0;
    double sumsq = 0.0;
    for (int i = 0; i < sample_count; ++i) {
        const gmpxx::mpz_class value = r.get_z_range(limit);
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

void test_f_statistics()
{
    constexpr int sample_count = 4096;
    constexpr double expected_mean = 0.5;
    constexpr double expected_variance = 1.0 / 12.0;

    gmpxx::gmp_randclass r(gmp_randinit_mt);
    r.seed(2027ul);

    double sum = 0.0;
    double sumsq = 0.0;
    for (int i = 0; i < sample_count; ++i) {
        const gmpxx::mpf_class value = r.get_f(static_cast<mp_bitcnt_t>(256));
        assert(mpf_sgn(value.get_mpf_t()) >= 0);
        assert(mpf_cmp_ui(value.get_mpf_t(), 1) < 0);
        const double x = value.to_double();
        sum += x;
        sumsq += x * x;
    }

    const double mean = sum / sample_count;
    const double variance = sumsq / sample_count - mean * mean;
    assert(std::abs(mean - expected_mean) < 0.035);
    assert(std::abs(variance - expected_variance) < 0.02);
}

} // namespace

int main()
{
    test_compile_time_surface();
    test_deterministic_seed_with_ui();
    test_deterministic_seed_with_mpz();
    test_z_range();
    test_z_bits_from_mpz();
    test_f_precision_forms();
    test_lc_constructors();
    test_z_range_statistics();
    test_f_statistics();
    return 0;
}
