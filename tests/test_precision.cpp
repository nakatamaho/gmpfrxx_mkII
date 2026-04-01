#include "gmpfrxx_mkII.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    // Expression precision = max of operands
    {
        mpfr_class a(prec_tag, 128);
        mpfr_class b(prec_tag, 256);
        a = 1.0;
        b = 2.0;
        auto expr = a + b;
        assert(expr.get_prec() == 256);
    }
    // Constructed from expression inherits expression precision
    {
        mpfr_class a(1.0, 128);
        mpfr_class b(2.0, 512);
        mpfr_class c = a + b;
        assert(c.get_prec() == 512);
    }
    // Assignment uses *this precision (not expression precision)
    {
        mpfr_class a(1.0, 128);
        mpfr_class b(2.0, 512);
        mpfr_class c(prec_tag, 64);
        c = a + b;
        assert(c.get_prec() == 64); // kept *this precision
    }
    // Scalar + mpfr: precision = max(scalar_prec, mpfr_prec)
    {
        mpfr_class a(prec_tag, 256);
        a = 1.0;
        auto expr = a + 1;      // ScalarExpr<int>.get_prec() == 64
        assert(expr.get_prec() == 256);

        auto expr2 = a + 1.0;   // ScalarExpr<double>.get_prec() == 53
        assert(expr2.get_prec() == 256);
    }
    // Chained expression: max over all operands
    {
        mpfr_class a(1.0, 64);
        mpfr_class b(2.0, 128);
        mpfr_class c(3.0, 256);
        auto expr = a + b + c;
        assert(expr.get_prec() == 256);
    }
    // set_prec then assign
    {
        mpfr_class a(1.0, 128);
        a.set_prec(512);
        assert(a.get_prec() == 512);
    }
    // prec_tag constructor vs int constructor
    {
        mpfr_class a(prec_tag, 1024);
        assert(a.get_prec() == 1024);
        assert(a.to_double() == 0.0);

        mpfr_class b(1024);
        assert(b.to_double() == 1024.0); // int constructor, not precision
    }
    // Compound assignment preserves precision
    {
        mpfr_class a(prec_tag, 512);
        a = 10.0;
        mpfr_class b(3.0, 64);
        a += b;
        assert(a.get_prec() == 512);
    }
    // TmpMpfr RAII check (scope-based)
    {
        mpfr_prec_t p = 256;
        TmpMpfr t(p);
        mpfr_set_d(t.ptr(), 3.14, MPFR_RNDN);
        assert(mpfr_get_prec(t.ptr()) == 256);
        // destructor runs at end of scope — no leak
    }
    // Copy preserves source precision
    {
        mpfr_class a(1.0, 384);
        mpfr_class b(a);
        assert(b.get_prec() == 384);
    }
    // mpz precision: at least default
    {
        mpz_class z(1);
        assert(z.get_prec() >= mpfr_get_default_prec());
    }
    // Mixed mpz + mpfr: max
    {
        mpfr_class a(prec_tag, 512);
        a = 1.0;
        mpz_class z(2);
        auto expr = a + z;
        assert(expr.get_prec() == 512);
    }

    std::cout << "test_precision: ALL PASSED" << std::endl;
    return 0;
}
