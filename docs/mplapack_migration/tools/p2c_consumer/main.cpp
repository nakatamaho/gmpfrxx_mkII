#include <gmpfrxx_mkII.h>

#include <sstream>
#include <type_traits>
#include <utility>

int main()
{
    using mpf_expr = decltype(
        std::declval<gmpxx::mpf_class&>() *
            std::declval<gmpxx::mpf_class&>() +
        std::declval<gmpxx::mpf_class&>());
    static_assert(gmpfrxx_mkII::detail::is_expression_node_v<mpf_expr>);

    const mpfrxx::mpfr_class mpfr_default;
    const gmpxx::mpf_class mpf_default;
    if (mpfr_default.precision() != 512 || mpf_default.precision() != 512) {
        return 1;
    }

    const mpfrxx::mpfr_class mpfr_value = 2;
    const mpfrxx::mpfr_class mpfr_result =
        mpfr_value * mpfr_value + sqrt(mpfr_value);
    const mpfrxx::mpc_class mpc_value(mpfr_value, -mpfr_value);
    const mpfrxx::mpc_class mpc_result = mpc_value * mpc_value + mpc_value;

    gmpxx::mpf_class low = 1;
    low.div_2exp(200);
    const gmpxx::mpf_class mpf_value = gmpxx::mpf_class(1) + low;
    const gmpxx::mpf_class mpf_result = mpf_value * mpf_value;
    const gmpxx::mpfc_class mpfc_value(mpf_value, -mpf_value);
    const gmpxx::mpfc_class mpfc_result = mpfc_value * mpfc_value + mpfc_value;

    std::ostringstream output;
    output << mpfr_result << mpc_result << gmpxx::const_pi() << mpfc_result;
    if (output.str().empty()) {
        return 2;
    }
    if (mpf_result - 1 <= low) {
        return 3;
    }
    return 0;
}
