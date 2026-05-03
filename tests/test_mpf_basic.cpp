#include <gmpxx_mkII.h>

#include <cmath>
#include <cstdlib>

namespace {

void require_close(double actual, double expected)
{
    if (std::abs(actual - expected) > 1e-12) {
        std::abort();
    }
}

} // namespace

int main()
{
    auto a = gmpxx::mpf_class::with_precision(128, 1.5);
    auto b = gmpxx::mpf_class::with_precision(128, 2.0);
    auto c = gmpxx::mpf_class::with_precision(128, 3.0);

    gmpxx::mpf_class result = a + b * c;
    require_close(result.to_double(), 7.5);

    result = c / b - a;
    require_close(result.to_double(), 0.0);

    result = -a + +c + 2;
    require_close(result.to_double(), 3.5);

    result = 2.5 + b * 4;
    require_close(result.to_double(), 10.5);

    return 0;
}
