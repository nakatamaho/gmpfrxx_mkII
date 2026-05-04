#include <gmpfrxx_mkII.h>

int main()
{
    mpfrxx::mpfr_class a;
    gmpxx::mpfc_class b;
    auto c = a + b;
    (void)c;
    return 0;
}
