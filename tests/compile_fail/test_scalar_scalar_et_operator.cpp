#include <mpfrxx_mkII.h>

int main()
{
    auto expr = gmpfrxx_mkII::detail::operator+(1, 2);
    (void)expr;
    return 0;
}
