#include <mpfrxx_mkII.h>

#include <iostream>

int main()
{
    const auto real = mpfrxx::mpfr_class::with_precision(160, 1.25);
    const auto offset = mpfrxx::mpq_class("3/4");
    const auto complex = mpfrxx::mpc_class::with_precision(160, 1.0, 2.0);

    mpfrxx::mpfr_class real_result = real + offset + 2;
    mpfrxx::mpc_class complex_result = complex * complex + real_result;

    std::cout << "real: " << real_result.to_double() << '\n';
    std::cout << "complex: "
              << complex_result.real_to_double() << " + "
              << complex_result.imag_to_double() << "i\n";
    return 0;
}
