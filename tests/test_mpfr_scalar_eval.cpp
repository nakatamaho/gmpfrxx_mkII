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
    auto x = mpfrxx::mpfr_class::with_precision(128, 10.0);

    mpfrxx::mpfr_class result = x + 5;
    require_close(result.to_double(), 15.0);

    result = 20u - x;
    require_close(result.to_double(), 10.0);

    result = x * 1.5f;
    require_close(result.to_double(), 15.0);

    result = 2.5 + x / 2;
    require_close(result.to_double(), 7.5);

    result = (x + 2) * (3 + x);
    require_close(result.to_double(), 156.0);

    return 0;
}
