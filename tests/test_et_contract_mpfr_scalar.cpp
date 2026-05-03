#include <mpfrxx_mkII.h>

#include <cstdint>
#include <type_traits>

int main()
{
    auto x = mpfrxx::mpfr_class::with_precision(128);

    auto signed_expr = x + 3;
    auto unsigned_expr = x + std::uint64_t{4};
    auto float_expr = x + 1.25f;
    auto double_expr = 2.5 + x;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(signed_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(unsigned_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(float_expr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(double_expr)>);

    static_assert(std::is_same_v<typename decltype(signed_expr)::result_type, mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<typename decltype(signed_expr)::rhs_type::value_type, std::int64_t>);
    static_assert(std::is_same_v<typename decltype(unsigned_expr)::rhs_type::value_type, std::uint64_t>);
    static_assert(std::is_same_v<typename decltype(float_expr)::rhs_type::value_type, double>);
    static_assert(std::is_same_v<typename decltype(double_expr)::lhs_type::value_type, double>);

    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<bool>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<long double>);

#if defined(__SIZEOF_INT128__)
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<__int128>);
    static_assert(!gmpfrxx_mkII::detail::is_supported_mpfr_scalar_v<unsigned __int128>);
#endif

    return 0;
}
