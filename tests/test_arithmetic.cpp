#include "gmpfrxx_mkII.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(double a, double b, double eps = 1e-14) {
    return std::fabs(a - b) < eps;
}

int main() {
    mpfr_class a(10.0);
    mpfr_class b(3.0);

    // Addition
    {
        mpfr_class c = a + b;
        assert(near(c.to_double(), 13.0));
    }
    // Subtraction
    {
        mpfr_class c = a - b;
        assert(near(c.to_double(), 7.0));
    }
    // Multiplication
    {
        mpfr_class c = a * b;
        assert(near(c.to_double(), 30.0));
    }
    // Division
    {
        mpfr_class c = a / b;
        assert(near(c.to_double(), 10.0 / 3.0));
    }
    // Chained: a + b + b
    {
        mpfr_class c = a + b + b;
        assert(near(c.to_double(), 16.0));
    }
    // Chained: (a - b) * b
    {
        mpfr_class c = (a - b) * b;
        assert(near(c.to_double(), 21.0));
    }
    // Complex expression: (a + b) / (a - b)
    {
        mpfr_class c = (a + b) / (a - b);
        assert(near(c.to_double(), 13.0 / 7.0));
    }
    // mpz in expression
    {
        mpz_class z(5);
        mpfr_class c = a + z;
        assert(near(c.to_double(), 15.0));
    }
    // mpq in expression
    {
        mpq_class q(1, 2);
        mpfr_class c = a * q;
        assert(near(c.to_double(), 5.0));
    }
    // mpz + mpz -> mpfr_class
    {
        mpz_class z1(7), z2(8);
        mpfr_class c = z1 + z2;
        assert(near(c.to_double(), 15.0));
    }
    // Assignment from expression
    {
        mpfr_class c(prec_tag, 128);
        c = a + b;
        assert(c.get_prec() == 128); // precision of *this is preserved
        assert(near(c.to_double(), 13.0));
    }
    // Unary minus
    {
        mpfr_class c = -a;
        assert(near(c.to_double(), -10.0));
    }
    // auto with chained ops (tests dangling-reference safety)
    {
        auto expr = a + b;       // BinaryExpr stored by value
        mpfr_class c = expr * b; // should not dangle
        assert(near(c.to_double(), 39.0));
    }

    std::cout << "test_arithmetic: ALL PASSED" << std::endl;
    return 0;
}
