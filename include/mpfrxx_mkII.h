#ifndef MPFRXX_MKII_H
#define MPFRXX_MKII_H

#include <gmp.h>
#include <mpfr.h>
#include <mpc.h>

#include <gmpfrxx_mkII/detail/config.hpp>
#include <gmpfrxx_mkII/detail/environment.hpp>
#include <gmpfrxx_mkII/detail/eval_context.hpp>
#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/mpc_impl.hpp>
#include <gmpfrxx_mkII/detail/mpfr_impl.hpp>
#include <gmpfrxx_mkII/detail/type_traits.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

namespace mpfrxx {

using ::gmpxx::mpq_class;
using ::gmpxx::mpz_class;

} // namespace mpfrxx

#endif // MPFRXX_MKII_H
