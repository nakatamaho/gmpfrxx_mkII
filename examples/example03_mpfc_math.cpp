#include <gmpxx_mkII.h>

#include <iostream>

int main()
{
    const auto z = gmpxx::mpfc_class::with_precision(160, 1.0, 1.0);

    const gmpxx::mpfc_class squared = z * z;
    const gmpxx::mpfc_class exponential = gmpxx::exp(z);
    const gmpxx::mpfc_class root = gmpxx::sqrt(z);

    std::cout << "z*z = " << squared << '\n';
    std::cout << "exp(z) = " << exponential << '\n';
    std::cout << "sqrt(z) = " << root << '\n';
    return 0;
}
