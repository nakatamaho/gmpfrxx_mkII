#include <gmpxx_mkII.h>

#include <type_traits>

int main()
{
    gmpxx::mpz_class z(3);
    gmpxx::mpq_class q(gmpxx::mpz_class(1), gmpxx::mpz_class(4));
    auto f = gmpxx::mpf_class::with_precision(128, 2.5);

    auto zf = z + f;
    auto fq = f + q;
    auto exact_then_f = (z + q) * f;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zf)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(fq)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(exact_then_f)>);

    static_assert(std::is_same_v<typename decltype(zf)::result_type, gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename decltype(fq)::result_type, gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename decltype(exact_then_f)::result_type, gmpxx::mpf_class>);

    static_assert(!std::is_same_v<decltype(zf), gmpxx::mpf_class>);

    return 0;
}
