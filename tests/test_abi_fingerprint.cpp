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

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace {

using gmpfrxx_mkII::detail::add_op;
using gmpfrxx_mkII::detail::binary_expr;
using gmpfrxx_mkII::detail::div_op;
using gmpfrxx_mkII::detail::is_expression_node_v;
using gmpfrxx_mkII::detail::mul_op;
using gmpfrxx_mkII::detail::neg_op;
using gmpfrxx_mkII::detail::object_leaf;
using gmpfrxx_mkII::detail::scalar_leaf;
using gmpfrxx_mkII::detail::sub_op;
using gmpfrxx_mkII::detail::unary_expr;

template <typename T, typename = void>
struct has_mpf_plus : std::false_type {};

template <typename T>
struct has_mpf_plus<
    T,
    std::void_t<decltype(std::declval<const gmpxx::mpf_class&>() + std::declval<T>())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_mpfr_plus : std::false_type {};

template <typename T>
struct has_mpfr_plus<
    T,
    std::void_t<decltype(std::declval<const mpfrxx::mpfr_class&>() + std::declval<T>())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_mpc_plus : std::false_type {};

template <typename T>
struct has_mpc_plus<
    T,
    std::void_t<decltype(std::declval<const mpfrxx::mpc_class&>() + std::declval<T>())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_mpfc_plus : std::false_type {};

template <typename T>
struct has_mpfc_plus<
    T,
    std::void_t<decltype(std::declval<const gmpxx::mpfc_class&>() + std::declval<T>())>>
    : std::true_type {};

void compile_time_fingerprint()
{
    using mpf_add_int = decltype(std::declval<const gmpxx::mpf_class&>() + 5LL);
    using mpf_mul_uint = decltype(5u * std::declval<const gmpxx::mpf_class&>());
    using mpf_div_float = decltype(std::declval<const gmpxx::mpf_class&>() / 0.5f);

    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpf_scalar_t<long long>, std::int64_t>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpf_scalar_t<int>, std::int64_t>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpf_scalar_t<unsigned>, std::uint64_t>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpf_scalar_t<float>, double>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpf_scalar_t<double>, double>);

    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpfr_scalar_t<long long>, std::int64_t>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpfr_scalar_t<unsigned>, std::uint64_t>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpfr_scalar_t<float>, double>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::normalized_mpfr_scalar_t<double>, double>);

    static_assert(gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<int>);
    static_assert(gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<double>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<bool>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpf_scalar_v<long double>);

    static_assert(gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<int>);
    static_assert(gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<double>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<bool>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<long double>);

    static_assert(gmpfrxx_mkII::detail::is_mpf_expression_operand_v<gmpxx::mpf_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpf_expression_operand_v<gmpxx::mpz_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpf_expression_operand_v<gmpxx::mpq_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpf_expression_operand_v<int>);
    static_assert(!gmpfrxx_mkII::detail::is_mpf_expression_operand_v<long double>);

    static_assert(gmpfrxx_mkII::detail::is_zq_expression_operand_v<gmpxx::mpz_class>);
    static_assert(gmpfrxx_mkII::detail::is_zq_expression_operand_v<gmpxx::mpq_class>);
    static_assert(!gmpfrxx_mkII::detail::is_zq_expression_operand_v<int>);
    static_assert(!gmpfrxx_mkII::detail::is_zq_expression_operand_v<gmpxx::mpf_class>);

    static_assert(gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<mpfrxx::mpfr_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<gmpxx::mpz_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<gmpxx::mpq_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<int>);
    static_assert(!gmpfrxx_mkII::detail::is_mpfr_expression_operand_v<long double>);

    static_assert(gmpfrxx_mkII::detail::is_mpc_expression_operand_v<mpfrxx::mpc_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpc_expression_operand_v<mpfrxx::mpfr_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpc_expression_operand_v<gmpxx::mpz_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpc_expression_operand_v<gmpxx::mpq_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpc_expression_operand_v<int>);
    static_assert(!gmpfrxx_mkII::detail::is_mpc_expression_operand_v<long double>);

    static_assert(gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<gmpxx::mpfc_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<gmpxx::mpf_class>);
    static_assert(gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<int>);
    static_assert(!gmpfrxx_mkII::detail::is_mpfc_expression_operand_v<long double>);

    static_assert(is_expression_node_v<mpf_add_int>);
    static_assert(std::is_same_v<typename mpf_add_int::op_type, add_op>);
    static_assert(std::is_same_v<typename mpf_add_int::lhs_type, object_leaf<gmpxx::mpf_class>>);
    static_assert(std::is_same_v<typename mpf_add_int::rhs_type, scalar_leaf<std::int64_t, gmpxx::mpf_class>>);
    static_assert(std::is_same_v<typename mpf_add_int::result_type, gmpxx::mpf_class>);

    static_assert(std::is_same_v<typename mpf_mul_uint::op_type, mul_op>);
    static_assert(std::is_same_v<typename mpf_mul_uint::lhs_type, scalar_leaf<std::uint64_t, gmpxx::mpf_class>>);
    static_assert(std::is_same_v<typename mpf_mul_uint::rhs_type, object_leaf<gmpxx::mpf_class>>);
    static_assert(std::is_same_v<typename mpf_mul_uint::result_type, gmpxx::mpf_class>);

    static_assert(std::is_same_v<typename mpf_div_float::op_type, div_op>);
    static_assert(std::is_same_v<typename mpf_div_float::rhs_type, scalar_leaf<double, gmpxx::mpf_class>>);
    static_assert(std::is_same_v<typename mpf_div_float::result_type, gmpxx::mpf_class>);

    using z_plus_z = decltype(std::declval<const gmpxx::mpz_class&>() + std::declval<const gmpxx::mpz_class&>());
    using z_plus_q = decltype(std::declval<const gmpxx::mpz_class&>() + std::declval<const gmpxx::mpq_class&>());
    using q_div_z = decltype(std::declval<const gmpxx::mpq_class&>() / std::declval<const gmpxx::mpz_class&>());
    static_assert(std::is_same_v<typename z_plus_z::result_type, gmpxx::mpz_class>);
    static_assert(std::is_same_v<typename z_plus_q::result_type, gmpxx::mpq_class>);
    static_assert(std::is_same_v<typename q_div_z::result_type, gmpxx::mpq_class>);

    using z_plus_mpf = decltype(std::declval<const gmpxx::mpz_class&>() + std::declval<const gmpxx::mpf_class&>());
    using q_plus_mpfr = decltype(std::declval<const gmpxx::mpq_class&>() + std::declval<const mpfrxx::mpfr_class&>());
    using mpfr_plus_mpc = decltype(std::declval<const mpfrxx::mpfr_class&>() + std::declval<const mpfrxx::mpc_class&>());
    using mpf_plus_mpfc = decltype(std::declval<const gmpxx::mpf_class&>() + std::declval<const gmpxx::mpfc_class&>());
    static_assert(std::is_same_v<typename z_plus_mpf::result_type, gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename q_plus_mpfr::result_type, mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<typename mpfr_plus_mpc::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename mpf_plus_mpfc::result_type, gmpxx::mpfc_class>);

    static_assert(std::is_same_v<gmpfrxx_mkII::detail::expression_result_type_t<mpf_add_int>, gmpxx::mpf_class>);
    static_assert(is_expression_node_v<decltype(-std::declval<const gmpxx::mpf_class&>())>);
    static_assert(std::is_same_v<typename decltype(-std::declval<const gmpxx::mpf_class&>())::op_type, neg_op>);
    static_assert(!is_expression_node_v<gmpxx::mpf_class>);
    static_assert(!is_expression_node_v<int>);

    static_assert(has_mpf_plus<int>::value);
    static_assert(has_mpf_plus<double>::value);
    static_assert(!has_mpf_plus<bool>::value);
    static_assert(!has_mpf_plus<long double>::value);
    static_assert(has_mpfr_plus<int>::value);
    static_assert(has_mpfr_plus<double>::value);
    static_assert(!has_mpfr_plus<bool>::value);
    static_assert(!has_mpfr_plus<long double>::value);
    static_assert(has_mpc_plus<mpfrxx::mpfr_class>::value);
    static_assert(has_mpc_plus<int>::value);
    static_assert(!has_mpc_plus<long double>::value);
    static_assert(has_mpfc_plus<gmpxx::mpf_class>::value);
    static_assert(has_mpfc_plus<int>::value);
    static_assert(!has_mpfc_plus<long double>::value);

    static_assert(std::is_same_v<decltype(1 + 2), int>);
    static_assert(!is_expression_node_v<decltype(1 + 2)>);
}

} // namespace

int main()
{
    compile_time_fingerprint();

    gmpxx::mpf_class a("1.25", 128);
    gmpxx::mpf_class b("2.5", 192);
    mpfrxx::mpfr_class r("3.75", 160);
    mpfrxx::mpfr_class s("4.5", 224);

    std::cout << "TYPE_ID(mpf a+b) = " << typeid(a + b).name() << "\n";
    std::cout << "TYPE_ID(mpf -a) = " << typeid(-a).name() << "\n";
    std::cout << "TYPE_ID(mpf (a+b)*a) = " << typeid((a + b) * a).name() << "\n";
    std::cout << "TYPE_ID(mpfr r+s) = " << typeid(r + s).name() << "\n";
    std::cout << "TYPE_ID(mpfr -r) = " << typeid(-r).name() << "\n";
    std::cout << "Note: typeid().name() is compiler-specific and not directly comparable across toolchains.\n";
    return 0;
}
