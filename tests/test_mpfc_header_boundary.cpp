#include <gmpxx_mkII.h>

#include <cstdlib>

int main()
{
    gmpxx::mpfc_class value = gmpxx::mpfc_class::with_precision(128, 1.0, 2.0);
    if (value.real_precision() != 128 || value.imag_precision() != 128) {
        std::abort();
    }
    return 0;
}
