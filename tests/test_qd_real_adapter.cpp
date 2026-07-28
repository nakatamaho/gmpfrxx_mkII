#include <gmpfrxx_mkII/adapters/qd_real.hpp>
#include <cmath>
int main(){qd_real x; x.x[0]=1.; x.x[1]=std::ldexp(1.,-100); x.x[2]=0.; x.x[3]=0.; mpfrxx::mpfr_class r=x; mpfrxx::mpfr_class one=1.; return mpfr_cmp(r.mpfr_data(),one.mpfr_data())>0?0:1;}
