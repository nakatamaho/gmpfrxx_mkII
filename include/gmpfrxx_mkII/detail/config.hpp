#ifndef GMPFRXX_MKII_DETAIL_CONFIG_HPP
#define GMPFRXX_MKII_DETAIL_CONFIG_HPP

namespace gmpfrxx_mkII {
namespace detail {

struct build_options {
#ifdef GMPFRXX_MKII_ASSUME_FIXED_PRECISION_FASTPATH
    static constexpr bool assume_fixed_precision_fastpath = true;
#else
    static constexpr bool assume_fixed_precision_fastpath = false;
#endif
};

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_CONFIG_HPP
