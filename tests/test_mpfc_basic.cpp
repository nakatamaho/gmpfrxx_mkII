// SPDX-License-Identifier: BSD-2-Clause

#include <cassert>
#include <cmath>
#include <complex>
#include <random>
#include <type_traits>
#include <utility>

#include <gmpxx_mkII.h>

namespace {

using complex_ref = std::complex<double>;

template <typename Lhs, typename Rhs, typename = void>
struct has_less_than : std::false_type {};

template <typename Lhs, typename Rhs>
struct has_less_than<Lhs, Rhs, std::void_t<decltype(std::declval<Lhs>() < std::declval<Rhs>())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_mutable_real_set_prec : std::false_type {};

template <typename T>
struct has_mutable_real_set_prec<T, std::void_t<decltype(std::declval<T&>().real().set_prec(64))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_mutable_imag_assignment : std::false_type {};

template <typename T>
struct has_mutable_imag_assignment<
    T,
    std::void_t<decltype(std::declval<T&>().imag() = std::declval<gmpxx::mpf_class>())>>
    : std::true_type {};

constexpr double complex_test_values[][2] = {
    {0.0, 0.0},
    {1.0, 0.0},
    {-1.0, 0.5},
    {2.25, -3.5},
    {-4.0, 1.75},
    {0.125, -0.25},
};

gmpxx::mpfc_class make_mpfc(double real_value, double imag_value) {
    return gmpxx::mpfc_class(gmpxx::mpf_class(real_value, 256),
                             gmpxx::mpf_class(imag_value, 256));
}

complex_ref to_ref(gmpxx::mpfc_class const& value) {
    return complex_ref(value.real().get_d(), value.imag().get_d());
}

void check_close(complex_ref expected, gmpxx::mpfc_class const& got) {
    complex_ref actual = to_ref(got);
    double scale = 1.0 + std::abs(expected);
    assert(std::abs(actual.real() - expected.real()) <= 1e-10 * scale);
    assert(std::abs(actual.imag() - expected.imag()) <= 1e-10 * scale);
}

void check_binary_arithmetic(complex_ref a_ref, complex_ref b_ref,
                             gmpxx::mpfc_class const& a,
                             gmpxx::mpfc_class const& b) {
    using gmpxx::mpfc_class;

    mpfc_class sum = a + b;
    check_close(a_ref + b_ref, sum);
    mpfc_class sum_compound = a;
    sum_compound += b;
    check_close(a_ref + b_ref, sum_compound);

    mpfc_class difference = a - b;
    check_close(a_ref - b_ref, difference);
    mpfc_class difference_compound = a;
    difference_compound -= b;
    check_close(a_ref - b_ref, difference_compound);

    mpfc_class product = a * b;
    check_close(a_ref * b_ref, product);
    mpfc_class product_compound = a;
    product_compound *= b;
    check_close(a_ref * b_ref, product_compound);

    constexpr double denom_min = 0.25;
    if (std::abs(b_ref) <= denom_min) {
        b_ref = complex_ref(1.25, 0.75);
        mpfc_class safe_b = make_mpfc(1.25, 0.75);
        mpfc_class quotient = a / safe_b;
        check_close(a_ref / b_ref, quotient);
        mpfc_class quotient_compound = a;
        quotient_compound /= safe_b;
        check_close(a_ref / b_ref, quotient_compound);
        return;
    }

    mpfc_class quotient = a / b;
    check_close(a_ref / b_ref, quotient);
    mpfc_class quotient_compound = a;
    quotient_compound /= b;
    check_close(a_ref / b_ref, quotient_compound);
}

void test_construction_and_accessors() {
    using namespace gmpxx;

    mpfc_class scalar_value(128);
    assert(scalar_value.real() == 128);
    assert(scalar_value.imag() == 0);

    mpfc_class scalar_pair(128, 64);
    assert(scalar_pair.real() == 128);
    assert(scalar_pair.imag() == 64);

    mpfc_class z(mpf_class(3, 192), mpf_class(-4, 128));
    static_assert(!std::is_assignable<decltype(z.real()), mpf_class>::value,
                  "mpfc_class::real() must not expose mutable component assignment");
    static_assert(!has_mutable_real_set_prec<mpfc_class>::value,
                  "mpfc_class::real() must not expose mutable precision changes");
    static_assert(!has_mutable_imag_assignment<mpfc_class>::value,
                  "mpfc_class::imag() must not expose mutable component assignment");

    assert(z.real() == mpf_class(3, 192));
    assert(z.imag() == mpf_class(-4, 128));
    assert(z.real().get_prec() == mpf_class(3, 192).get_prec());
    assert(z.imag().get_prec() == mpf_class(-4, 128).get_prec());
    assert(z.get_prec() == z.real().get_prec());

    mpfc_class moved_lhs(mpf_class(1, 192), mpf_class(2, 160));
    const mp_bitcnt_t moved_lhs_real_precision = moved_lhs.real().precision();
    const mp_bitcnt_t moved_lhs_imag_precision = moved_lhs.imag().precision();
    mpfc_class moved_owner(std::move(moved_lhs));
    moved_lhs = mpf_class(3, 224);
    assert(moved_lhs.real().precision() == moved_lhs_real_precision);
    assert(moved_lhs.imag().precision() == moved_lhs_imag_precision);
    assert(moved_lhs.real() == 3);
    assert(moved_lhs.imag() == 0);

    mpfc_class moved_scalar_lhs(mpf_class(1, 192), mpf_class(2, 160));
    const mp_bitcnt_t moved_scalar_real_precision = moved_scalar_lhs.real().precision();
    const mp_bitcnt_t moved_scalar_imag_precision = moved_scalar_lhs.imag().precision();
    mpfc_class moved_scalar_owner(std::move(moved_scalar_lhs));
    moved_scalar_lhs = 4;
    assert(moved_scalar_lhs.real().precision() == moved_scalar_real_precision);
    assert(moved_scalar_lhs.imag().precision() == moved_scalar_imag_precision);
    assert(moved_scalar_lhs.real() == 4);
    assert(moved_scalar_lhs.imag() == 0);

    mpfc_class moved_expr_lhs(mpf_class(1, 192), mpf_class(2, 160));
    const mp_bitcnt_t moved_expr_real_precision = moved_expr_lhs.real().precision();
    const mp_bitcnt_t moved_expr_imag_precision = moved_expr_lhs.imag().precision();
    mpfc_class moved_expr_owner(std::move(moved_expr_lhs));
    const mpfc_class expr_base(mpf_class(3, 192), mpf_class(-4, 128));
    moved_expr_lhs = expr_base + mpfc_class(mpf_class(1, 192), mpf_class(2, 128));
    assert(moved_expr_lhs.real().precision() == moved_expr_real_precision);
    assert(moved_expr_lhs.imag().precision() == moved_expr_imag_precision);
    assert(moved_expr_lhs.real() == 4);
    assert(moved_expr_lhs.imag() == -2);
    (void)moved_owner;
    (void)moved_scalar_owner;
    (void)moved_expr_owner;

    z.real(mpf_class(5, 160));
    z.imag(mpf_class(6, 160));
    assert(z.real() == 5);
    assert(z.imag() == 6);
}

void test_swap() {
    using namespace gmpxx;

    mpfc_class lhs(mpf_class(1, 128), mpf_class(2, 160));
    mpfc_class rhs(mpf_class(3, 192), mpf_class(4, 224));

    mp_bitcnt_t lhs_real_prec = lhs.real().get_prec();
    mp_bitcnt_t lhs_imag_prec = lhs.imag().get_prec();
    mp_bitcnt_t rhs_real_prec = rhs.real().get_prec();
    mp_bitcnt_t rhs_imag_prec = rhs.imag().get_prec();

    lhs.swap(rhs);
    assert(lhs.real() == 3);
    assert(lhs.imag() == 4);
    assert(rhs.real() == 1);
    assert(rhs.imag() == 2);
    assert(lhs.real().get_prec() == rhs_real_prec);
    assert(lhs.imag().get_prec() == rhs_imag_prec);
    assert(rhs.real().get_prec() == lhs_real_prec);
    assert(rhs.imag().get_prec() == lhs_imag_prec);

    using std::swap;
    swap(lhs, rhs);
    assert(lhs.real() == 1);
    assert(lhs.imag() == 2);
    assert(rhs.real() == 3);
    assert(rhs.imag() == 4);
    assert(lhs.real().get_prec() == lhs_real_prec);
    assert(lhs.imag().get_prec() == lhs_imag_prec);
    assert(rhs.real().get_prec() == rhs_real_prec);
    assert(rhs.imag().get_prec() == rhs_imag_prec);
}

void test_lazy_arithmetic() {
    using namespace gmpxx;

    mpfc_class a(mpf_class(1, 160), mpf_class(2, 160));
    mpfc_class b(mpf_class(3, 224), mpf_class(-4, 224));

    mpfc_class sum = a + b;
    assert(sum.real() == 4);
    assert(sum.imag() == -2);
    assert(sum.get_prec() == b.get_prec());

    mpfc_class product = a * b;
    assert(product.real() == 11);
    assert(product.imag() == 2);
    assert(product.get_prec() == b.get_prec());

    mpfc_class chained = a + b * a - mpf_class(5, 192);
    assert(chained.real() == 7);
    assert(chained.imag() == 4);
}

void test_real_operands_and_division() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 192));

    mpfc_class half = z / mpf_class(2, 192);
    assert(half.real() == mpf_class("1.5", 192));
    assert(half.imag() == 2);

    mpfc_class shifted = mpf_class(5, 192) + z;
    assert(shifted.real() == 8);
    assert(shifted.imag() == 4);

    mpfc_class one_plus_i(mpf_class(1, 192), mpf_class(1, 192));
    mpfc_class one_minus_i(mpf_class(1, 192), mpf_class(-1, 192));
    mpfc_class quotient = one_plus_i / one_minus_i;
    assert(quotient.real() == 0);
    assert(quotient.imag() == 1);
}

void test_exact_real_operands() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 160));
    mpz_class exact_z(4);
    mpq_class exact_q(mpz_class(3), mpz_class(2));

    mpfc_class sum_z_rhs = z + exact_z;
    assert(sum_z_rhs.real() == 7);
    assert(sum_z_rhs.imag() == 4);
    assert(sum_z_rhs.real().get_prec() == z.real().get_prec());
    assert(sum_z_rhs.imag().get_prec() == z.imag().get_prec());

    mpfc_class sum_z_lhs = exact_z + z;
    assert(sum_z_lhs.real() == 7);
    assert(sum_z_lhs.imag() == 4);

    mpfc_class diff_z_lhs = exact_z - z;
    assert(diff_z_lhs.real() == 1);
    assert(diff_z_lhs.imag() == -4);

    mpfc_class sum_q_rhs = z + exact_q;
    assert(sum_q_rhs.real() == mpf_class("4.5", 192));
    assert(sum_q_rhs.imag() == 4);

    mpfc_class sum_q_lhs = exact_q + z;
    assert(sum_q_lhs.real() == mpf_class("4.5", 192));
    assert(sum_q_lhs.imag() == 4);

    mpfc_class product_q = exact_q * z;
    assert(product_q.real() == mpf_class("4.5", 192));
    assert(product_q.imag() == 6);

    mpfc_class quotient_z = z / exact_z;
    assert(quotient_z.real() == mpf_class("0.75", 192));
    assert(quotient_z.imag() == 1);

    mpfc_class chained = (exact_z + exact_q) + z;
    assert(chained.real() == mpf_class("8.5", 192));
    assert(chained.imag() == 4);

    mpfc_class compound = z;
    compound += exact_z;
    assert(compound.real() == 7);
    assert(compound.imag() == 4);
    compound -= exact_q;
    assert(compound.real() == mpf_class("5.5", 192));
    assert(compound.imag() == 4);
    compound *= exact_q;
    assert(compound.real() == mpf_class("8.25", 192));
    assert(compound.imag() == 6);
    compound /= exact_z;
    assert(compound.real() == mpf_class("2.0625", 192));
    assert(compound.imag() == mpf_class("1.5", 160));
}

void test_assignment_preserves_destination_precision() {
    using namespace gmpxx;

    mpfc_class dst(mpf_class(0, 96), mpf_class(0, 128));
    mp_bitcnt_t real_prec = dst.real().get_prec();
    mp_bitcnt_t imag_prec = dst.imag().get_prec();

    mpfc_class lhs(mpf_class(1, 256), mpf_class(2, 256));
    mpfc_class rhs(mpf_class(3, 320), mpf_class(4, 320));

    dst = rhs;
    assert(dst.real() == 3);
    assert(dst.imag() == 4);
    assert(dst.real().get_prec() == real_prec);
    assert(dst.imag().get_prec() == imag_prec);

    dst = lhs + rhs;
    assert(dst.real() == 4);
    assert(dst.imag() == 6);
    assert(dst.real().get_prec() == real_prec);
    assert(dst.imag().get_prec() == imag_prec);

    dst *= mpf_class(2, 256);
    assert(dst.real() == 8);
    assert(dst.imag() == 12);
    assert(dst.real().get_prec() == real_prec);
    assert(dst.imag().get_prec() == imag_prec);
}

void test_conjugate_norm_and_abs() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 192));
    mpfc_class c = conj(z);

    assert(c.real() == 3);
    assert(c.imag() == -4);
    assert(norm(z) == 25);
    assert(abs(z) == 5);
}

void test_comparison_and_free_helpers() {
    using namespace gmpxx;

    mpfc_class z(mpf_class(3, 192), mpf_class(4, 192));
    mpfc_class same(mpf_class(3, 256), mpf_class(4, 256));
    mpfc_class different(mpf_class(3, 192), mpf_class(-4, 192));
    mpfc_class real_value(mpf_class(3, 192), mpf_class(0, 192));

    assert(z == same);
    assert(z != different);
    assert(real_value == mpf_class(3, 192));
    assert(mpf_class(3, 192) == real_value);
    assert(z != mpf_class(3, 192));
    assert(mpfc_class(mpf_class(0, 192), mpf_class(0, 160)) == 0);
    assert(0 == mpfc_class(mpf_class(0, 192), mpf_class(0, 160)));
    assert(mpfc_class(mpf_class(3, 192), mpf_class(0, 160)) == 3);
    assert(3 == mpfc_class(mpf_class(3, 192), mpf_class(0, 160)));
    assert(mpfc_class(mpf_class("1.25", 192), mpf_class(0, 160)) == 1.25);
    assert(1.25 == mpfc_class(mpf_class("1.25", 192), mpf_class(0, 160)));
    assert(mpfc_class(mpf_class(3, 192), mpf_class(1, 160)) != 3);
    assert(3 != mpfc_class(mpf_class(3, 192), mpf_class(1, 160)));
    static_assert(!has_less_than<mpfc_class, mpfc_class>::value,
                  "mpfc_class intentionally has no ordering comparison");
    static_assert(!has_less_than<mpfc_class, int>::value,
                  "mpfc_class intentionally has no scalar ordering comparison");
    static_assert(!has_less_than<int, mpfc_class>::value,
                  "mpfc_class intentionally has no scalar ordering comparison");

    assert(real(z) == 3);
    assert(imag(z) == 4);
    assert(real(z + same) == 6);
    assert(imag(z + same) == 8);

    mpf_class angle = arg(z);
    assert(std::abs(angle.get_d() - std::atan2(4.0, 3.0)) < 1e-12);

    mpfc_class from_polar =
        polar(mpf_class(5, 192), mpf_class(std::atan2(4.0, 3.0), 192));
    check_close(complex_ref(3.0, 4.0), from_polar);
}

void test_arithmetic_smoke_against_std_complex() {
    using gmpxx::mpfc_class;

    for (auto const& lhs : complex_test_values) {
        for (auto const& rhs : complex_test_values) {
            complex_ref a_ref(lhs[0], lhs[1]);
            complex_ref b_ref(rhs[0], rhs[1]);
            mpfc_class a = make_mpfc(lhs[0], lhs[1]);
            mpfc_class b = make_mpfc(rhs[0], rhs[1]);
            check_binary_arithmetic(a_ref, b_ref, a, b);
        }
    }

    std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
    std::uniform_real_distribution<double> dist(-4.0, 4.0);
    for (int iter = 0; iter < 200; ++iter) {
        double ar = dist(rng);
        double ai = dist(rng);
        double br = dist(rng);
        double bi = dist(rng);

        complex_ref a_ref(ar, ai);
        complex_ref b_ref(br, bi);
        mpfc_class a = make_mpfc(ar, ai);
        mpfc_class b = make_mpfc(br, bi);
        check_binary_arithmetic(a_ref, b_ref, a, b);
    }
}

}  // namespace

int main() {
    test_construction_and_accessors();
    test_swap();
    test_lazy_arithmetic();
    test_real_operands_and_division();
    test_exact_real_operands();
    test_assignment_preserves_destination_precision();
    test_conjugate_norm_and_abs();
    test_comparison_and_free_helpers();
    test_arithmetic_smoke_against_std_complex();
    return 0;
}
