#include <mpfrxx_mkII.h>

#include <cstdint>
#include <cstdlib>
#include <limits>

namespace {

void require_equal_to_integer(const mpfrxx::mpfr_class& value, const gmpxx::mpz_class& expected)
{
    if (mpfr_cmp_z(value.mpfr_data(), expected.mpz_data()) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    const auto zero = mpfrxx::mpfr_class::with_precision(128);

    const auto uint64_max = std::numeric_limits<std::uint64_t>::max();
    mpfrxx::mpfr_class unsigned_result = zero + uint64_max;
    require_equal_to_integer(unsigned_result, gmpxx::mpz_class(uint64_max));

    const auto int64_min = std::numeric_limits<std::int64_t>::min();
    mpfrxx::mpfr_class signed_result = zero + int64_min;
    require_equal_to_integer(signed_result, gmpxx::mpz_class(int64_min));

    const auto int64_max = std::numeric_limits<std::int64_t>::max();
    signed_result = int64_max + zero;
    require_equal_to_integer(signed_result, gmpxx::mpz_class(int64_max));

    return 0;
}
