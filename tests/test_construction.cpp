#include "gmpfrxx_mkII.h"
#include <cassert>
#include <cmath>
#include <iostream>

namespace {

void assertDoubleEqual(double actual, double expected, double tolerance = 1e-15) {
    assert(std::fabs(actual - expected) < tolerance);
}

} // namespace

void testDefaultConstructorMpfrClass() {
    mpfr_class a;

    // Default-constructed mpfr_class should be zero-initialized
    // and use MPFR's current default precision.
    assert(mpfr_zero_p(a.get_mpfr_t()) != 0);
    assert(a.get_prec() == mpfr_get_default_prec());

    std::cout << "testDefaultConstructorMpfrClass passed." << std::endl;
}

void testCopyConstructorMpfrClass() {
    mpfr_class a("3.14159265358979323846264338327950288", 256);
    mpfr_class b(a);

    assert(b.get_prec() == a.get_prec());
    assert(mpfr_cmp(b.get_mpfr_t(), a.get_mpfr_t()) == 0);

    std::cout << "testCopyConstructorMpfrClass passed." << std::endl;
}

void testCopyAssignmentOperatorMpfrClass() {
    mpfr_class a("3.14159265358979323846264338327950288", 256);
    mpfr_class b(prec_tag, 128);
    mpfr_prec_t original_prec = b.get_prec();

    b = a;

    assert(b.get_prec() == original_prec);
    assertDoubleEqual(b.to_double(), a.to_double());

    std::cout << "testCopyAssignmentOperatorMpfrClass passed." << std::endl;
}

void testValueConstructorsMpfrClass() {
    {
        mpfr_class a(prec_tag, 256);
        assert(a.get_prec() == 256);
        assert(mpfr_zero_p(a.get_mpfr_t()) != 0);
    }
    {
        mpfr_class a(42);
        assertDoubleEqual(a.to_double(), 42.0);
    }
    {
        mpfr_class a(100L);
        assertDoubleEqual(a.to_double(), 100.0);
    }
    {
        mpfr_class a(200UL);
        assertDoubleEqual(a.to_double(), 200.0);
    }
    {
        mpfr_class a(3.14);
        assertDoubleEqual(a.to_double(), 3.14);
    }
    {
        mpfr_class a(1.0, 512);
        assert(a.get_prec() == 512);
        assertDoubleEqual(a.to_double(), 1.0);
    }
    {
        mpfr_class a("1.23456789012345678901234567890");
        assertDoubleEqual(a.to_double(), 1.2345678901234568);
    }
    {
        mpz_class z(42);
        mpfr_class a(z);
        assertDoubleEqual(a.to_double(), 42.0);
    }
    {
        mpq_class q(1, 3);
        mpfr_class a(q);
        assertDoubleEqual(a.to_double(), 1.0 / 3.0);
    }

    std::cout << "testValueConstructorsMpfrClass passed." << std::endl;
}

void testMoveConstructorMpfrClass() {
    mpfr_class a(2.72, 128);
    double value = a.to_double();
    mpfr_class b(std::move(a));

    assertDoubleEqual(b.to_double(), value);
    assert(b.get_prec() == 128);

    std::cout << "testMoveConstructorMpfrClass passed." << std::endl;
}

void testMoveAssignmentOperatorMpfrClass() {
    mpfr_class a("2.71828182845904523536028747135266250", 256);
    mpfr_class b(prec_tag, 128);

    b = std::move(a);

    assertDoubleEqual(b.to_double(), 2.7182818284590451);
    assert(b.get_prec() == 256);

    std::cout << "testMoveAssignmentOperatorMpfrClass passed." << std::endl;
}

void testDefaultConstructorMpzClass() {
    mpz_class z;

    assert(mpz_cmp_si(z.get_mpz_t(), 0) == 0);

    std::cout << "testDefaultConstructorMpzClass passed." << std::endl;
}

void testCopyConstructorMpzClass() {
    mpz_class a("999999999999999999");
    mpz_class b(a);

    assert(mpz_cmp(b.get_mpz_t(), a.get_mpz_t()) == 0);

    std::cout << "testCopyConstructorMpzClass passed." << std::endl;
}

void testCopyAssignmentOperatorMpzClass() {
    mpz_class a("12345678901234567890");
    mpz_class b;

    b = a;

    assert(mpz_cmp(b.get_mpz_t(), a.get_mpz_t()) == 0);

    std::cout << "testCopyAssignmentOperatorMpzClass passed." << std::endl;
}

void testValueConstructorsMpzClass() {
    mpz_class z(123);

    assert(mpz_cmp_si(z.get_mpz_t(), 123) == 0);

    std::cout << "testValueConstructorsMpzClass passed." << std::endl;
}

void testMoveConstructorMpzClass() {
    mpz_class z(123);
    mpz_class moved(std::move(z));

    assert(mpz_cmp_si(moved.get_mpz_t(), 123) == 0);

    std::cout << "testMoveConstructorMpzClass passed." << std::endl;
}

void testMoveAssignmentOperatorMpzClass() {
    mpz_class a("12345678901234567890");
    mpz_class b;
    mpz_class expected("12345678901234567890");

    b = std::move(a);

    assert(mpz_cmp(b.get_mpz_t(), expected.get_mpz_t()) == 0);

    std::cout << "testMoveAssignmentOperatorMpzClass passed." << std::endl;
}

void testDefaultConstructorMpqClass() {
    mpq_class q;

    assert(mpq_sgn(q.get_mpq_t()) == 0);

    std::cout << "testDefaultConstructorMpqClass passed." << std::endl;
}

void testCopyConstructorMpqClass() {
    mpq_class a("7/11");
    mpq_class b(a);

    assert(mpq_equal(b.get_mpq_t(), a.get_mpq_t()) != 0);

    std::cout << "testCopyConstructorMpqClass passed." << std::endl;
}

void testCopyAssignmentOperatorMpqClass() {
    mpq_class a("355/113");
    mpq_class b;

    b = a;

    assert(mpq_equal(b.get_mpq_t(), a.get_mpq_t()) != 0);

    std::cout << "testCopyAssignmentOperatorMpqClass passed." << std::endl;
}

void testValueConstructorsMpqClass() {
    mpq_class q(3, 4);

    assert(mpq_cmp_ui(q.get_mpq_t(), 3, 4) == 0);

    std::cout << "testValueConstructorsMpqClass passed." << std::endl;
}

void testMoveConstructorMpqClass() {
    mpq_class q(3, 4);
    mpq_class moved(std::move(q));

    assert(mpq_cmp_ui(moved.get_mpq_t(), 3, 4) == 0);

    std::cout << "testMoveConstructorMpqClass passed." << std::endl;
}

void testMoveAssignmentOperatorMpqClass() {
    mpq_class a("355/113");
    mpq_class b;
    mpq_class expected("355/113");

    b = std::move(a);

    assert(mpq_equal(b.get_mpq_t(), expected.get_mpq_t()) != 0);

    std::cout << "testMoveAssignmentOperatorMpqClass passed." << std::endl;
}

int main() {
    testDefaultConstructorMpfrClass();
    testCopyConstructorMpfrClass();
    testCopyAssignmentOperatorMpfrClass();
    testValueConstructorsMpfrClass();
    testMoveConstructorMpfrClass();
    testMoveAssignmentOperatorMpfrClass();
    testDefaultConstructorMpzClass();
    testCopyConstructorMpzClass();
    testCopyAssignmentOperatorMpzClass();
    testValueConstructorsMpzClass();
    testMoveConstructorMpzClass();
    testMoveAssignmentOperatorMpzClass();
    testDefaultConstructorMpqClass();
    testCopyConstructorMpqClass();
    testCopyAssignmentOperatorMpqClass();
    testValueConstructorsMpqClass();
    testMoveConstructorMpqClass();
    testMoveAssignmentOperatorMpqClass();

    std::cout << "test_construction: ALL PASSED" << std::endl;
    return 0;
}
