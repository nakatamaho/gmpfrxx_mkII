#include <gmpxx_mkII.h>

#include <cstdlib>

namespace {

void require_mpz_si(const gmpxx::mpz_class& value, long expected)
{
    if (mpz_cmp_si(value.mpz_data(), expected) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    gmpxx::mpz_class a(7);
    gmpxx::mpz_class b(-3);

    gmpxx::mpz_class sum = a + b;
    require_mpz_si(sum, 4);

    sum = a - b;
    require_mpz_si(sum, 10);

    sum = a * b;
    require_mpz_si(sum, -21);

    gmpxx::mpz_class nested = (a + b) * (a - b);
    require_mpz_si(nested, 40);

    return 0;
}
