#include <mpfrxx_mkII.h>

#include <type_traits>

int main()
{
    mpfrxx::mpz_class z1(2);
    mpfrxx::mpz_class z2(3);
    mpfrxx::mpq_class q(mpfrxx::mpz_class(1), mpfrxx::mpz_class(5));
    auto r = mpfrxx::mpfr_class::with_precision(128, 1.25);

    auto zz = z1 + z2;
    auto zq = z1 + q;
    auto qq = q + q;
    auto zr = z1 + r;
    auto rq = r + q;

    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zz)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zq)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(qq)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(zr)>);
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<decltype(rq)>);

    static_assert(std::is_same_v<typename decltype(zz)::result_type, gmpxx::mpz_class>);
    static_assert(std::is_same_v<typename decltype(zq)::result_type, gmpxx::mpq_class>);
    static_assert(std::is_same_v<typename decltype(qq)::result_type, gmpxx::mpq_class>);
    static_assert(std::is_same_v<typename decltype(zr)::result_type, mpfrxx::mpfr_class>);
    static_assert(std::is_same_v<typename decltype(rq)::result_type, mpfrxx::mpfr_class>);

    static_assert(!std::is_same_v<decltype(zr), mpfrxx::mpfr_class>);

    return 0;
}
