#include <gmpfrxx_mkII.h>

int main()
{
    gmpxx::mpfc_class a;
    mpfrxx::mpc_class b;
    auto c = a + b;
    (void)c;
    return 0;
}
