#ifndef GMPFRXX_MKII_DETAIL_INTEGER_CONVERSION_HPP
#define GMPFRXX_MKII_DETAIL_INTEGER_CONVERSION_HPP

#include <cstdint>
#include <type_traits>

namespace gmpfrxx_mkII {
namespace detail {

template <typename T>
struct is_supported_expression_integral
    : std::bool_constant<std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool> &&
                         (sizeof(T) <= sizeof(std::uint64_t))> {};

template <typename T>
inline constexpr bool is_supported_expression_integral_v =
    is_supported_expression_integral<std::remove_cv_t<T>>::value;

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_INTEGER_CONVERSION_HPP
