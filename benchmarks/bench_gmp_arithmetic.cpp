#include <gmpxx_mkII.h>

#include <chrono>
#include <iostream>

int main()
{
    constexpr int iterations = 20000;
    auto a = gmpxx::mpf_class::with_precision(256, 1.25);
    auto b = gmpxx::mpf_class::with_precision(256, 2.5);
    auto c = gmpxx::mpf_class::with_precision(256, 3.75);
    auto out = gmpxx::mpf_class::with_precision(256);

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        out = a + b * c - a / b;
        a = out + 1;
    }
    const auto stop = std::chrono::steady_clock::now();

    const auto elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    std::cout << "bench_gmp_arithmetic " << iterations << " iterations "
              << elapsed << " us result=" << out << '\n';
    return 0;
}
