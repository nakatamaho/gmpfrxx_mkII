#include <gmpxx_mkII.h>

#include <type_traits>

int main()
{
    auto a = gmpxx::mpf_class::with_precision(128, 1.0);
    auto b = gmpxx::mpf_class::with_precision(128, 2.0);
    auto c = gmpxx::mpf_class::with_precision(128, 3.0);

    auto product = b * c;
    auto expr = a + product;
    auto scalar = a + 5;
    auto unary = -a;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(product)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(scalar)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);
    static_assert(!std::is_same_v<decltype(expr), gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename decltype(expr)::result_type, gmpxx::mpf_class>);
    static_assert(std::is_same_v<typename decltype(scalar)::rhs_type::value_type, std::int64_t>);

    return 0;
}
