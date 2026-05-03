#ifndef GMPFRXX_MKII_DETAIL_TYPE_TRAITS_HPP
#define GMPFRXX_MKII_DETAIL_TYPE_TRAITS_HPP

#include <type_traits>

namespace gmpfrxx_mkII {
namespace detail {

template <typename T>
struct is_expression_node : std::false_type {};

template <typename T>
inline constexpr bool is_expression_node_v = is_expression_node<T>::value;

template <typename T, typename = void>
struct expression_result_type {};

template <typename T>
struct expression_result_type<T, std::void_t<typename T::result_type>> {
    using type = typename T::result_type;
};

template <typename T>
using expression_result_type_t = typename expression_result_type<T>::type;

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_TYPE_TRAITS_HPP
