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
#include <mpcxx_mkII.h>

#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace {

template <typename Lhs, typename Rhs, typename = void>
struct has_less_than : std::false_type {};

template <typename Lhs, typename Rhs>
struct has_less_than<Lhs, Rhs, std::void_t<decltype(std::declval<Lhs>() < std::declval<Rhs>())>>
    : std::true_type {};

template <typename Function>
void require_invalid_argument(Function&& function)
{
    try {
        function();
    } catch (const std::invalid_argument&) {
        return;
    }
    std::abort();
}

template <typename T, typename = void>
struct has_real_member_accessor : std::false_type {};

template <typename T>
struct has_real_member_accessor<T, std::void_t<decltype(std::declval<T&>().real())>>
    : std::true_type {};

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

void check_mpc_division_near_32bit_exponent_edge()
{
    constexpr mpfr_exp_t edge_emax = 1073741759;
    constexpr mpfr_exp_t edge_emin = -1073741759;
    if (mpfr_get_emax_max() < edge_emax || mpfr_get_emin_min() > edge_emin) {
        return;
    }

    const mpfr_exp_t old_emin = mpfr_get_emin();
    const mpfr_exp_t old_emax = mpfr_get_emax();
    if (mpfr_set_emin(edge_emin) != 0 || mpfr_set_emax(edge_emax) != 0) {
        std::abort();
    }

    auto numerator = mpfrxx::mpc_class::with_precision(128);
    auto denominator = mpfrxx::mpc_class::with_precision(128);
    mpfr_set_ui_2exp(mpc_realref(numerator.mpc_data()), 1, edge_emax - 1, MPFR_RNDN);
    mpfr_set_ui_2exp(mpc_imagref(numerator.mpc_data()), 1, edge_emax - 1, MPFR_RNDN);
    mpfr_set_ui_2exp(mpc_realref(denominator.mpc_data()), 1, edge_emax - 1, MPFR_RNDN);
    mpfr_set_ui_2exp(mpc_imagref(denominator.mpc_data()), 1, edge_emax - 1, MPFR_RNDN);
    mpfr_check_range(mpc_realref(numerator.mpc_data()), 0, MPFR_RNDN);
    mpfr_check_range(mpc_imagref(numerator.mpc_data()), 0, MPFR_RNDN);
    mpfr_check_range(mpc_realref(denominator.mpc_data()), 0, MPFR_RNDN);
    mpfr_check_range(mpc_imagref(denominator.mpc_data()), 0, MPFR_RNDN);

    mpfr_clear_flags();
    const mpfrxx::mpc_class quotient = numerator / denominator;
    if (mpfr_overflow_p() != 0 ||
        mpfr_inf_p(mpc_realref(quotient.mpc_data())) != 0 ||
        mpfr_inf_p(mpc_imagref(quotient.mpc_data())) != 0 ||
        mpfr_cmp_ui(mpc_realref(quotient.mpc_data()), 1u) != 0 ||
        mpfr_zero_p(mpc_imagref(quotient.mpc_data())) == 0) {
        std::abort();
    }

    if (mpfr_set_emin(old_emin) != 0 || mpfr_set_emax(old_emax) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(160, 3.0, 4.0);
    auto r = mpfrxx::mpfr_class::with_precision(160, 5.0);
    mpfrxx::mpz_class exact_z(7);
    mpfrxx::mpq_class exact_q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    mpfrxx::mpfr_class ctor_real = mpfrxx::mpfr_class::with_precision(192, 1.25);
    mpfrxx::mpfr_class ctor_imag = mpfrxx::mpfr_class::with_precision(224, -2.5);
    mpfrxx::mpc_class constructed(ctor_real, ctor_imag);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, double>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, int>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, std::complex<double>>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, mpfrxx::mpz_class>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, mpfrxx::mpq_class>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, mpfrxx::mpfr_class>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, const char*>);
    static_assert(std::is_constructible_v<mpfrxx::mpc_class, std::string>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, double>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, int>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, std::complex<double>>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, mpfrxx::mpz_class>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, mpfrxx::mpq_class>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, mpfrxx::mpfr_class>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, const char*>);
    static_assert(std::is_assignable_v<mpfrxx::mpc_class&, std::string>);
    static_assert(!std::is_constructible_v<mpfrxx::mpc_class, bool>);
    static_assert(!std::is_assignable_v<mpfrxx::mpc_class&, bool>);
    static_assert(!has_real_member_accessor<mpfrxx::mpc_class>::value,
                  "mpc_class must not expose mutable real()/imag() component accessors");
    require_close(constructed.real_to_double(), 1.25);
    require_close(constructed.imag_to_double(), -2.5);
    if (constructed.real_precision() != 192 || constructed.imag_precision() != 224) {
        std::abort();
    }

    mpfrxx::mpc_class from_double(2.5);
    require_close(from_double.real_to_double(), 2.5);
    require_close(from_double.imag_to_double(), 0.0);

    mpfrxx::mpc_class from_int(42);
    require_close(from_int.real_to_double(), 42.0);
    require_close(from_int.imag_to_double(), 0.0);

    mpfrxx::mpc_class from_std_complex(std::complex<double>(-1.25, 3.5));
    require_close(from_std_complex.real_to_double(), -1.25);
    require_close(from_std_complex.imag_to_double(), 3.5);

    mpfrxx::mpc_class from_mpz(mpfrxx::mpz_class(5));
    require_close(from_mpz.real_to_double(), 5.0);
    require_close(from_mpz.imag_to_double(), 0.0);

    mpfrxx::mpc_class from_mpq(exact_q);
    require_close(from_mpq.real_to_double(), 0.5);
    require_close(from_mpq.imag_to_double(), 0.0);

    mpfrxx::mpc_class from_mpfr(ctor_real);
    require_close(from_mpfr.real_to_double(), 1.25);
    require_close(from_mpfr.imag_to_double(), 0.0);
    if (from_mpfr.real_precision() != 192 || from_mpfr.imag_precision() != 192) {
        std::abort();
    }

    mpfrxx::mpc_class from_stream_string("(3.125,-2.5)");
    require_close(from_stream_string.real_to_double(), 3.125);
    require_close(from_stream_string.imag_to_double(), -2.5);

    mpfrxx::mpc_class from_i_string("3.125+2.5i");
    require_close(from_i_string.real_to_double(), 3.125);
    require_close(from_i_string.imag_to_double(), 2.5);

    mpfrxx::mpc_class from_std_string(std::string("3.25-1.5i"));
    require_close(from_std_string.real_to_double(), 3.25);
    require_close(from_std_string.imag_to_double(), -1.5);

    mpfrxx::mpc_class from_negative_i_string("-3.125-2.5i");
    require_close(from_negative_i_string.real_to_double(), -3.125);
    require_close(from_negative_i_string.imag_to_double(), -2.5);

    mpfrxx::mpc_class from_exponent_i_string("1.25e+2-3.75e-1i");
    require_close(from_exponent_i_string.real_to_double(), 125.0);
    require_close(from_exponent_i_string.imag_to_double(), -0.375);

    mpfrxx::mpc_class from_hex_string("0xff+0x1.8i");
    require_close(from_hex_string.real_to_double(), 255.0);
    require_close(from_hex_string.imag_to_double(), 1.5);

    mpfrxx::mpc_class assigned = mpfrxx::mpc_class::with_precision(193, 257);
    assigned = 2.5;
    require_close(assigned.real_to_double(), 2.5);
    require_close(assigned.imag_to_double(), 0.0);
    if (assigned.real_precision() != 193 || assigned.imag_precision() != 257) {
        std::abort();
    }
    assigned = ctor_real;
    require_close(assigned.real_to_double(), 1.25);
    require_close(assigned.imag_to_double(), 0.0);
    assigned = exact_z;
    require_close(assigned.real_to_double(), 7.0);
    require_close(assigned.imag_to_double(), 0.0);
    assigned = exact_q;
    require_close(assigned.real_to_double(), 0.5);
    require_close(assigned.imag_to_double(), 0.0);
    assigned = "4.5-1.25i";
    require_close(assigned.real_to_double(), 4.5);
    require_close(assigned.imag_to_double(), -1.25);
    assigned = std::string("6.5+0.75i");
    require_close(assigned.real_to_double(), 6.5);
    require_close(assigned.imag_to_double(), 0.75);
    assigned = std::complex<double>(-2.25, 1.5);
    require_close(assigned.real_to_double(), -2.25);
    require_close(assigned.imag_to_double(), 1.5);
    if (assigned.real_precision() != 193 || assigned.imag_precision() != 257) {
        std::abort();
    }

    mpfrxx::mpc_class result = z + w;
    require_close(result.real_to_double(), 4.0);
    require_close(result.imag_to_double(), 6.0);
    if (result.precision() != 160) {
        std::abort();
    }

    result = z * w;
    require_close(result.real_to_double(), -5.0);
    require_close(result.imag_to_double(), 10.0);

    result = z + r - exact_z + exact_q + 2;
    require_close(result.real_to_double(), 1.5);
    require_close(result.imag_to_double(), 2.0);

    result = z + std::complex<double>(0.5, -1.25);
    require_close(result.real_to_double(), 1.5);
    require_close(result.imag_to_double(), 0.75);

    result = std::complex<double>(0.5, -1.25) + r;
    require_close(result.real_to_double(), 5.5);
    require_close(result.imag_to_double(), -1.25);

    result = r * std::complex<double>(2.0, -0.5);
    require_close(result.real_to_double(), 10.0);
    require_close(result.imag_to_double(), -2.5);

    result = -z;
    require_close(result.real_to_double(), -1.0);
    require_close(result.imag_to_double(), -2.0);

    auto zero = mpfrxx::mpc_class::with_precision(160, 192, 0.0, 0.0);
    auto real_three = mpfrxx::mpc_class::with_precision(160, 192, 3.0, 0.0);
    auto complex_three = mpfrxx::mpc_class::with_precision(160, 192, 3.0, 1.0);
    auto same_three = mpfrxx::mpc_class::with_precision(224, 256, 3.0, 0.0);
    auto mpfr_three = mpfrxx::mpfr_class::with_precision(224, 3.0);
    if (!(zero == 0) || !(0 == zero) || zero != 0) {
        std::abort();
    }
    if (!(real_three == same_three) || real_three != same_three) {
        std::abort();
    }
    if (!(real_three == mpfr_three) || !(mpfr_three == real_three)) {
        std::abort();
    }
    mpfrxx::mpz_class exact_three(3);
    if (!(real_three == exact_three) || !(exact_three == real_three)) {
        std::abort();
    }
    mpfrxx::mpq_class exact_three_q(mpfrxx::mpz_class(6), mpfrxx::mpz_class(2));
    if (!(real_three == exact_three_q) || !(exact_three_q == real_three)) {
        std::abort();
    }
    if (!(real_three == 3) || !(3 == real_three) || !(real_three == 3.0) || !(3.0 == real_three)) {
        std::abort();
    }
    if (!(complex_three == std::complex<double>(3.0, 1.0)) ||
        !(std::complex<double>(3.0, 1.0) == complex_three) ||
        complex_three != std::complex<double>(3.0, 1.0)) {
        std::abort();
    }
    if (!(complex_three != std::complex<double>(3.0, 0.0)) ||
        !(std::complex<double>(3.0, 0.0) != complex_three)) {
        std::abort();
    }
    if (!(complex_three != 3) || !(3 != complex_three) || !(complex_three != mpfr_three)) {
        std::abort();
    }

    auto exact_edge_complex = mpfrxx::mpc_class::with_precision(53, 53, 0.0, 0.0);
    mpfr_set_ui_2exp(mpc_realref(exact_edge_complex.mpc_data()), 1u, 100, MPFR_RNDN);
    mpfrxx::mpz_class two_to_100_plus_one(1);
    mpz_mul_2exp(two_to_100_plus_one.mpz_data(), two_to_100_plus_one.mpz_data(), 100);
    two_to_100_plus_one += 1;
    if (!(exact_edge_complex != two_to_100_plus_one) || two_to_100_plus_one == exact_edge_complex) {
        std::abort();
    }

    mpfrxx::mpz_class rational_numerator(3);
    mpz_mul_2exp(rational_numerator.mpz_data(), rational_numerator.mpz_data(), 100);
    rational_numerator += 1;
    const mpfrxx::mpq_class rational_above(rational_numerator, mpfrxx::mpz_class(3));
    if (!(exact_edge_complex != rational_above) || rational_above == exact_edge_complex) {
        std::abort();
    }

    const std::uint64_t max_u = std::numeric_limits<std::uint64_t>::max();
    const mpfrxx::mpz_class max_u_z(max_u);
    auto rounded_uint64_complex = mpfrxx::mpc_class::with_precision(53, 53, 0.0, 0.0);
    mpfr_set_z(mpc_realref(rounded_uint64_complex.mpc_data()), max_u_z.mpz_data(), MPFR_RNDN);
    if (!(rounded_uint64_complex != max_u) || max_u == rounded_uint64_complex) {
        std::abort();
    }

    require_invalid_argument([] { (void)mpfrxx::mpc_class::with_precision(0); });
    require_invalid_argument([] { (void)mpfrxx::mpc_class::with_precision(64, 0); });
    require_invalid_argument([] { (void)mpfrxx::mpc_class("1+2i", 0); });
    require_invalid_argument([] { (void)mpfrxx::mpc_class("1+2i", 64, 0, 0); });
    if (MPFR_PREC_MAX < std::numeric_limits<mpfr_prec_t>::max()) {
        const auto too_large_precision =
            static_cast<mpfr_prec_t>(static_cast<unsigned long long>(MPFR_PREC_MAX) + 1ull);
        require_invalid_argument([&] { (void)mpfrxx::mpc_class::with_precision(too_large_precision); });
        require_invalid_argument([&] { (void)mpfrxx::mpc_class::with_precision(64, too_large_precision); });
    }

    auto nan_complex = mpfrxx::mpc_class::with_precision(160, 192, 0.0, 0.0);
    mpfr_set_nan(mpc_realref(nan_complex.mpc_data()));
    if (nan_complex == nan_complex) {
        std::abort();
    }
    if (!(nan_complex != nan_complex)) {
        std::abort();
    }
    static_assert(!has_less_than<mpfrxx::mpc_class, mpfrxx::mpc_class>::value,
                  "mpc_class intentionally has no ordering comparison");
    static_assert(!has_less_than<mpfrxx::mpc_class, int>::value,
                  "mpc_class intentionally has no scalar ordering comparison");
    static_assert(!has_less_than<int, mpfrxx::mpc_class>::value,
                  "mpc_class intentionally has no scalar ordering comparison");

    mpfrxx::mpc_class compound = mpfrxx::mpc_class::with_precision(192, 224, 1.0, 2.0);
    compound += w;
    require_close(compound.real_to_double(), 4.0);
    require_close(compound.imag_to_double(), 6.0);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound -= r;
    require_close(compound.real_to_double(), -1.0);
    require_close(compound.imag_to_double(), 6.0);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound *= mpfrxx::mpc_class::with_precision(160, 2.0, -1.0);
    require_close(compound.real_to_double(), 4.0);
    require_close(compound.imag_to_double(), 13.0);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound /= mpfrxx::mpfr_class::with_precision(160, 2.0);
    require_close(compound.real_to_double(), 2.0);
    require_close(compound.imag_to_double(), 6.5);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    compound += z * w;
    require_close(compound.real_to_double(), -3.0);
    require_close(compound.imag_to_double(), 16.5);
    if (compound.real_precision() != 192 || compound.imag_precision() != 224) {
        std::abort();
    }

    check_mpc_division_near_32bit_exponent_edge();

    return 0;
}
