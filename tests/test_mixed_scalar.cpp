#include "gmpfrxx_mkII.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(double a, double b, double eps = 1e-14) {
    return std::fabs(a - b) < eps;
}

int main() {
    mpfr_class a(10.0);

    // mpfr + int
    {
        mpfr_class c = a + 5;
        assert(near(c.to_double(), 15.0));
    }
    // int + mpfr
    {
        mpfr_class c = 5 + a;
        assert(near(c.to_double(), 15.0));
    }
    // mpfr - double
    {
        mpfr_class c = a - 2.5;
        assert(near(c.to_double(), 7.5));
    }
    // double - mpfr
    {
        mpfr_class c = 100.0 - a;
        assert(near(c.to_double(), 90.0));
    }
    // mpfr * unsigned
    {
        mpfr_class c = a * 3u;
        assert(near(c.to_double(), 30.0));
    }
    // unsigned * mpfr
    {
        mpfr_class c = 3u * a;
        assert(near(c.to_double(), 30.0));
    }
    // mpfr / long
    {
        mpfr_class c = a / 4L;
        assert(near(c.to_double(), 2.5));
    }
    // long / mpfr
    {
        mpfr_class c = 100L / a;
        assert(near(c.to_double(), 10.0));
    }
    // Chained: mpfr + int + double
    {
        mpfr_class c = a + 5 + 0.5;
        assert(near(c.to_double(), 15.5));
    }
    // Complex: (mpfr + int) * double
    {
        mpfr_class c = (a + 5) * 2.0;
        assert(near(c.to_double(), 30.0));
    }
    // Assign scalar to mpfr_class
    {
        mpfr_class c(prec_tag, 128);
        c = 42;
        assert(near(c.to_double(), 42.0));
        c = 3.14;
        assert(near(c.to_double(), 3.14));
    }
    // Mixed with mpz
    {
        mpz_class z(7);
        mpfr_class c = z + 3;
        assert(near(c.to_double(), 10.0));
    }
    // Mixed with mpq
    {
        mpq_class q(1, 4);
        mpfr_class c = q + 0.75;
        assert(near(c.to_double(), 1.0));
    }

    std::cout << "test_mixed_scalar: ALL PASSED" << std::endl;
    return 0;
}
