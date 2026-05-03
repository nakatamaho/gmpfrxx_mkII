#include <mpfrxx_mkII.h>

int main()
{
#if defined(__SIZEOF_INT128__)
    auto x = mpfrxx::mpfr_class::with_precision(128);
    __int128 wide = 1;
    auto expr = x + wide;
    (void)expr;
#else
    static_assert(false, "__int128 is required for this compile-fail test");
#endif
    return 0;
}
