#include <mpfrxx_mkII.h>

#include <type_traits>

int main()
{
    auto z = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(128, 3.0, 4.0);
    auto r = mpfrxx::mpfr_class::with_precision(128, 5.0);
    mpfrxx::mpz_class exact_z(7);
    mpfrxx::mpq_class exact_q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(2));

    auto zw = z + w;
    auto zr = z + r;
    auto rz = r + z;
    auto zexact = z + exact_z;
    auto qz = exact_q + z;
    auto scalar = z + 3;
    auto unary = -z;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zw)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(rz)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zexact)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(qz)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(scalar)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);

    static_assert(std::is_same_v<typename decltype(zw)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(zr)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(rz)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(zexact)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(qz)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(scalar)::result_type, mpfrxx::mpc_class>);
    static_assert(std::is_same_v<typename decltype(unary)::result_type, mpfrxx::mpc_class>);

    static_assert(!std::is_same_v<decltype(zw), mpfrxx::mpc_class>);

    return 0;
}
