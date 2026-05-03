#include <gmpxx_mkII.h>

#include <cstdlib>

namespace {

void require_mpq_si(const gmpxx::mpq_class& value, long numerator, unsigned long denominator)
{
    if (mpq_cmp_si(value.mpq_data(), numerator, denominator) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    gmpxx::mpz_class one(1);
    gmpxx::mpz_class two(2);
    gmpxx::mpz_class three(3);

    gmpxx::mpq_class half(one, two);
    gmpxx::mpq_class third(one, three);

    gmpxx::mpq_class result = half + third;
    require_mpq_si(result, 5, 6);

    result = half - third;
    require_mpq_si(result, 1, 6);

    result = half * third;
    require_mpq_si(result, 1, 6);

    result = half / third;
    require_mpq_si(result, 3, 2);

    result = gmpxx::mpz_class(2) + half;
    require_mpq_si(result, 5, 2);

    return 0;
}
