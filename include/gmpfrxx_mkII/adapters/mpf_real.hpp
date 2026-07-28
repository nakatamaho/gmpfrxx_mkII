#ifndef GMPFRXX_MKII_ADAPTERS_MPF_REAL_HPP
#define GMPFRXX_MKII_ADAPTERS_MPF_REAL_HPP
#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>
namespace gmpfrxx_mkII::detail {
template<> struct external_mpfr_real_traits<gmpxx::mpf_class>{
 static constexpr bool enabled=true;
 static void set(mpfr_t d,const gmpxx::mpf_class& v,mpfr_rnd_t r){mpfr_set_f(d,v.mpf_data(),r);}
};
}
#endif
