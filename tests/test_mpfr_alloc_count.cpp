#include <mpfrxx_mkII.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>

namespace {

std::atomic<int> alloc_count{0};

void* count_alloc(std::size_t n)
{
    ++alloc_count;
    return std::malloc(n);
}

void* count_realloc(void* p, std::size_t, std::size_t n)
{
    return std::realloc(p, n);
}

void count_free(void* p, std::size_t)
{
    std::free(p);
}

void require_alloc_count(int expected)
{
    const int actual = alloc_count.load();
    if (actual != expected) {
        std::fprintf(stderr, "expected %d allocations, got %d\n", expected, actual);
        std::abort();
    }
}

} // namespace

int main()
{
    mp_set_memory_functions(count_alloc, count_realloc, count_free);

    constexpr mpfr_prec_t precision = 256;
    const auto a = mpfrxx::mpfr_class::with_precision(precision, 1.5);
    const auto b = mpfrxx::mpfr_class::with_precision(precision, 2.5);
    const auto c = mpfrxx::mpfr_class::with_precision(precision, 3.5);
    const auto d = mpfrxx::mpfr_class::with_precision(precision, 4.5);
    auto dst = mpfrxx::mpfr_class::with_precision(precision);

    alloc_count = 0;
    dst = a + b;
    require_alloc_count(0);

    alloc_count = 0;
    dst = a + b + c;
    require_alloc_count(0);

    alloc_count = 0;
    dst = a + b + c + d;
    require_alloc_count(0);

    alloc_count = 0;
    dst = (a + b) * (c + d);
    require_alloc_count(1);

    return 0;
}
