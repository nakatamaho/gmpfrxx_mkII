#include <mpfrxx_mkII.h>

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
    const auto a = mpfrxx::mpfr_class::with_precision(128, 1.5);
    const auto b = mpfrxx::mpfr_class::with_precision(128, 2.0);
    const auto c = mpfrxx::mpfr_class::with_precision(128, 3.0);

    mpfrxx::mpfr_class result = a + b * c;
    require_close(result.to_double(), 7.5);

    result = c / b - a;
    require_close(result.to_double(), 0.0);

    result = -a + +c;
    require_close(result.to_double(), 1.5);

    return 0;
}
