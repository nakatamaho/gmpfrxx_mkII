#include <gmpxx_mkII.h>

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

int main()
{
    static_assert(CHAR_BIT == 8, "Phase 0 assumes octet bytes for later import tests");
    static_assert(sizeof(std::uint64_t) >= 8);
    static_assert(std::numeric_limits<std::int64_t>::is_signed);
    static_assert(gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath ||
                  !gmpfrxx_mkII::detail::build_options::assume_fixed_precision_fastpath);
    return 0;
}
