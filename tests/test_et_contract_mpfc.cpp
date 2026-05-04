#include <gmpxx_mkII.h>

#include <type_traits>

int main()
{
    auto a = gmpxx::mpfc_class::with_precision(128, 1.0, 2.0);
    auto b = gmpxx::mpfc_class::with_precision(128, 3.0, 4.0);
    auto r = gmpxx::mpf_class::with_precision(128, 5.0);

    auto sum = a + b;
    auto mixed = r + a;
    auto scalar = a + 2;
    auto unary = -a;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(sum)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(mixed)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(scalar)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);
    static_assert(!std::is_same_v<decltype(sum), gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(sum)::result_type, gmpxx::mpfc_class>);
    static_assert(std::is_same_v<typename decltype(mixed)::result_type, gmpxx::mpfc_class>);

    return 0;
}
