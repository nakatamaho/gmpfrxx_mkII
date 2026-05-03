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
    auto x = mpfrxx::mpfr_class::with_precision(128, 2.0);
    const auto y = mpfrxx::mpfr_class::with_precision(128, 3.0);

    x = x + y * x;
    require_close(x.to_double(), 8.0);

    x = -x + y;
    require_close(x.to_double(), -5.0);

    x = x / x;
    require_close(x.to_double(), 1.0);

    return 0;
}
