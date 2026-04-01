#include "gmpfrxx_mkII.h"
#include <cassert>
#include <cmath>
#include <iostream>

void testDefaultConstructor() {
    mpfr_class a;
    assert(a.to_double() == 0.0);
    assert(a.get_prec() == mpfr_get_default_prec());
}

int main() {
    testDefaultConstructor();
    // prec_tag constructor
    {
        mpfr_class a(prec_tag, 256);
        assert(a.get_prec() == 256);
        assert(a.to_double() == 0.0);
    }
    // From int
    {
        mpfr_class a(42);
        assert(a.to_double() == 42.0);
    }
    // From long
    {
        mpfr_class a(100L);
        assert(a.to_double() == 100.0);
    }
    // From unsigned long
    {
        mpfr_class a(200UL);
        assert(a.to_double() == 200.0);
    }
    // From double
    {
        mpfr_class a(3.14);
        assert(std::fabs(a.to_double() - 3.14) < 1e-15);
    }
    // From double with explicit precision
    {
        mpfr_class a(1.0, 512);
        assert(a.get_prec() == 512);
    }
    // From string
    {
        mpfr_class a("1.23456789012345678901234567890");
        assert(std::fabs(a.to_double() - 1.2345678901234568) < 1e-15);
    }
    // From mpz_class
    {
        mpz_class z(42);
        mpfr_class a(z);
        assert(a.to_double() == 42.0);
    }
    // From mpq_class
    {
        mpq_class q(1, 3);
        mpfr_class a(q);
        assert(std::fabs(a.to_double() - 1.0 / 3.0) < 1e-15);
    }
    // Copy constructor
    {
        mpfr_class a(3.14, 256);
        mpfr_class b(a);
        assert(b.get_prec() == 256);
        assert(std::fabs(b.to_double() - 3.14) < 1e-15);
    }
    // Move constructor
    {
        mpfr_class a(2.72, 128);
        double val = a.to_double();
        mpfr_class b(std::move(a));
        assert(std::fabs(b.to_double() - val) < 1e-15);
        assert(b.get_prec() == 128);
    }
    // mpz_class construction
    {
        mpz_class z;
        assert(mpz_cmp_si(z.get_mpz_t(), 0) == 0);

        mpz_class z2(123);
        assert(mpz_cmp_si(z2.get_mpz_t(), 123) == 0);

        mpz_class z3("999999999999999999");
        // just check it doesn't throw

        mpz_class z4(z2);
        assert(mpz_cmp(z4.get_mpz_t(), z2.get_mpz_t()) == 0);

        mpz_class z5(std::move(z4));
        assert(mpz_cmp_si(z5.get_mpz_t(), 123) == 0);
    }
    // mpq_class construction
    {
        mpq_class q;
        mpq_class q2(3, 4);
        mpq_class q3("7/11");
        mpq_class q4(q2);
        mpq_class q5(std::move(q4));
    }

    std::cout << "test_construction: ALL PASSED" << std::endl;
    return 0;
}
