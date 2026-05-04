#include <gmpxx_mkII.h>

#include <cmath>
#include <cstdlib>
#include <sstream>

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
    auto a = gmpxx::mpfc_class::with_precision(128, 1.0, 2.0);
    auto b = gmpxx::mpfc_class::with_precision(128, 3.0, 4.0);
    auto r = gmpxx::mpf_class::with_precision(128, 5.0);

    gmpxx::mpfc_class result = a + b;
    require_close(result.real().to_double(), 4.0);
    require_close(result.imag().to_double(), 6.0);

    result = a * b;
    require_close(result.real().to_double(), -5.0);
    require_close(result.imag().to_double(), 10.0);

    result = b / a;
    require_close(result.real().to_double(), 2.2);
    require_close(result.imag().to_double(), -0.4);

    result = r + a * 2;
    require_close(result.real().to_double(), 7.0);
    require_close(result.imag().to_double(), 4.0);

    result = -a;
    require_close(result.real().to_double(), -1.0);
    require_close(result.imag().to_double(), -2.0);

    std::ostringstream out;
    out << a;
    if (out.str().find(',') == std::string::npos) {
        std::abort();
    }

    return 0;
}
