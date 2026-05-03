#ifndef GMPFRXX_MKII_DETAIL_EVAL_CONTEXT_HPP
#define GMPFRXX_MKII_DETAIL_EVAL_CONTEXT_HPP

#include <gmpfrxx_mkII/detail/environment.hpp>

#include <mpfr.h>

namespace gmpfrxx_mkII {
namespace detail {

struct eval_context {
    mpfr_prec_t precision_bits;
    mpfr_rnd_t rounding_mode;
    mpfr_exp_t emin;
    mpfr_exp_t emax;
};

inline eval_context current_eval_context(mpfr_prec_t precision_bits)
{
    const auto defaults = ::mpfrxx::default_options();
    return eval_context{
        precision_bits,
        defaults.rounding_mode,
        defaults.emin,
        defaults.emax,
    };
}

} // namespace detail
} // namespace gmpfrxx_mkII

#endif // GMPFRXX_MKII_DETAIL_EVAL_CONTEXT_HPP
