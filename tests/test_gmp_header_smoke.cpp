#include <gmpxx_mkII.h>

#include <type_traits>

int main()
{
    static_assert(std::is_default_constructible_v<gmpxx::mpz_class>);
    static_assert(std::is_default_constructible_v<gmpxx::mpq_class>);
    static_assert(std::is_default_constructible_v<gmpxx::mpf_class>);
    return 0;
}
