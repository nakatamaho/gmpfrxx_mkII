#include <mpfrxx_mkII.h>

#include <chrono>
#include <iostream>

int main()
{
    constexpr int iterations = 10000;
    auto a = mpfrxx::mpfr_class::with_precision(192, 1.25);
    auto b = mpfrxx::mpfr_class::with_precision(192, 2.5);
    auto z = mpfrxx::mpc_class::with_precision(192, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(192, 3.0, 4.0);
    auto out = mpfrxx::mpc_class::with_precision(192);

    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        out = z * w + a - b;
        a = a + b / 3;
    }
    const auto stop = std::chrono::steady_clock::now();

    const auto elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    std::cout << "bench_mpfr_mpc_arithmetic " << iterations << " iterations "
              << elapsed << " us result=("
              << out.real_to_double() << "," << out.imag_to_double() << ")\n";
    return 0;
}
