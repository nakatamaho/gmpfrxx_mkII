#include <gmpxx_mkII.h>

#include <cstdlib>

namespace {

void require_num_den(const gmpxx::mpq_class& value, long numerator, unsigned long denominator)
{
    mpz_t num;
    mpz_t den;
    mpz_init(num);
    mpz_init(den);
    mpq_get_num(num, value.mpq_data());
    mpq_get_den(den, value.mpq_data());
    const bool ok = mpz_cmp_si(num, numerator) == 0 && mpz_cmp_ui(den, denominator) == 0;
    mpz_clear(den);
    mpz_clear(num);
    if (!ok) {
        std::abort();
    }
}

} // namespace

int main()
{
    gmpxx::mpq_class reduced(gmpxx::mpz_class(2), gmpxx::mpz_class(4));
    require_num_den(reduced, 1, 2);

    gmpxx::mpq_class sign(gmpxx::mpz_class(2), gmpxx::mpz_class(-4));
    require_num_den(sign, -1, 2);

    gmpxx::mpq_class result = reduced + reduced;
    require_num_den(result, 1, 1);

    return 0;
}
