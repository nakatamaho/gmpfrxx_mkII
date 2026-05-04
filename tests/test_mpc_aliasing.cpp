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
    auto z = mpfrxx::mpc_class::with_precision(128, 1.0, 2.0);
    auto w = mpfrxx::mpc_class::with_precision(128, 3.0, -1.0);

    z = z * w + z;
    require_close(z.real_to_double(), 6.0);
    require_close(z.imag_to_double(), 7.0);

    return 0;
}
