#include <gmpfrxx_mkII/adapters/binary80_real.hpp>
#include <gmpfrxx_mkII/adapters/binary128_real.hpp>
#include <gmpfrxx_mkII/adapters/mpf_real.hpp>
#include <gmpxx_mkII.h>
#include <mpfrxx_mkII.h>
#include <cmath>
static int above_one(const mpfrxx::mpfr_class& x){mpfrxx::mpfr_class one=1.;return mpfr_cmp(x.mpfr_data(),one.mpfr_data())>0?0:1;}
int main(){gmpxx::mpf_class f(512);mpf_set_d(f.mpf_data(),1.);mpf_t l;mpf_init2(l,512);mpf_set_d(l,std::ldexp(1.,-100));mpf_add(f.mpf_data(),f.mpf_data(),l);mpfrxx::mpfr_class r=f;mpf_clear(l);if(above_one(r))return 1;
#if LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
if(above_one(mpfrxx::mpfr_class(gmpfrxx_mkII::adapters::binary80_real(1.L+std::ldexp(1.L,-60)))))return 2;
#endif
#if GMPFRXX_MKII_ADAPTERS_HAVE_BINARY128
if(above_one(mpfrxx::mpfr_class(gmpfrxx_mkII::adapters::binary128_real((__float128)1+(__float128)0x1p-100L))))return 3;
#endif
return 0;}
