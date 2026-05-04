#include <gmpxx_mkII.h>

#include <iostream>

int main()
{
    gmpxx::mpf_class lhs("1.5", static_cast<mp_bitcnt_t>(128));
    gmpxx::mpf_class rhs("2.5", static_cast<mp_bitcnt_t>(128));

    gmpxx::mpf_class result = lhs + rhs;

    std::cout << "1.5 + 2.5 = " << result << '\n';
    return 0;
}
