#include <gmpfrxx_mkII/adapters/dd_real.hpp>
#include <cassert>
#include <cmath>
int main(){dd_real x; x.x[0]=1.; x.x[1]=std::ldexp(1.,-100); mpfrxx::mpfr_class r=x; mpfrxx::mpfr_class one=1.; return mpfr_cmp(r.mpfr_data(),one.mpfr_data())>0?0:1;}
