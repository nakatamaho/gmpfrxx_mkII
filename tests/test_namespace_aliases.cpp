#include <mpfrxx_mkII.h>

#include <type_traits>

int main()
{
    static_assert(std::is_same_v<mpfrxx::mpz_class, gmpxx::mpz_class>);
    static_assert(std::is_same_v<mpfrxx::mpq_class, gmpxx::mpq_class>);
    return 0;
}
