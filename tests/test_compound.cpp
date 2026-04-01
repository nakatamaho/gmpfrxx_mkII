#include "gmpfrxx_mkII.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(double a, double b, double eps = 1e-14) {
    return std::fabs(a - b) < eps;
}

int main() {
    // += with mpfr_class
    {
        mpfr_class a(10.0), b(3.0);
        a += b;
        assert(near(a.to_double(), 13.0));
    }
    // -= with mpfr_class
    {
        mpfr_class a(10.0), b(3.0);
        a -= b;
        assert(near(a.to_double(), 7.0));
    }
    // *= with mpfr_class
    {
        mpfr_class a(10.0), b(3.0);
        a *= b;
        assert(near(a.to_double(), 30.0));
    }
    // /= with mpfr_class
    {
        mpfr_class a(10.0), b(4.0);
        a /= b;
        assert(near(a.to_double(), 2.5));
    }
    // += with int
    {
        mpfr_class a(10.0);
        a += 5;
        assert(near(a.to_double(), 15.0));
    }
    // -= with double
    {
        mpfr_class a(10.0);
        a -= 2.5;
        assert(near(a.to_double(), 7.5));
    }
    // *= with unsigned
    {
        mpfr_class a(10.0);
        a *= 3u;
        assert(near(a.to_double(), 30.0));
    }
    // /= with long
    {
        mpfr_class a(10.0);
        a /= 4L;
        assert(near(a.to_double(), 2.5));
    }
    // += with expression
    {
        mpfr_class a(10.0), b(3.0), c(2.0);
        a += b + c;
        assert(near(a.to_double(), 15.0));
    }
    // -= with expression
    {
        mpfr_class a(10.0), b(3.0), c(2.0);
        a -= b * c;
        assert(near(a.to_double(), 4.0));
    }
    // Compound preserves precision
    {
        mpfr_class a(prec_tag, 256);
        a = 10.0;
        a += 5;
        assert(a.get_prec() == 256);
        assert(near(a.to_double(), 15.0));
    }
    // += with mpz_class
    {
        mpfr_class a(10.0);
        mpz_class z(7);
        a += z;
        assert(near(a.to_double(), 17.0));
    }
    // *= with mpq_class
    {
        mpfr_class a(10.0);
        mpq_class q(1, 2);
        a *= q;
        assert(near(a.to_double(), 5.0));
    }
    // Self-compound: a += a
    {
        mpfr_class a(10.0);
        a += a;
        assert(near(a.to_double(), 20.0));
    }

    std::cout << "test_compound: ALL PASSED" << std::endl;
    return 0;
}
