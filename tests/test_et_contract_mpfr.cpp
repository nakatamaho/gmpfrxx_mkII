#include <mpfrxx_mkII.h>

#include <type_traits>

int main()
{
    auto a = mpfrxx::mpfr_class::with_precision(128);
    auto b = mpfrxx::mpfr_class::with_precision(128);
    auto c = mpfrxx::mpfr_class::with_precision(128);

    auto product = b * c;
    auto expr = a + product;
    auto unary = -a;
    auto positive = +a;
    auto quotient = a / b;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(product)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unary)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(positive)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(quotient)>);

    static_assert(!std::is_same_v<decltype(expr), mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<typename decltype(expr)::result_type, mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<gmpfrxx_mkII::detail::expression_result_type_t<decltype(expr)>,
                                 mpfrxx::mpfr_class>);

    return 0;
}
