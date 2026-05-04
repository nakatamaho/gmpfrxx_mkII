#ifndef GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
#define GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP

#include <gmpfrxx_mkII/detail/expr.hpp>
#include <gmpfrxx_mkII/detail/eval_context.hpp>
#include <gmpfrxx_mkII/detail/integer_conversion.hpp>
#include <gmpfrxx_mkII/detail/zq_impl.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <ios>
#include <istream>
#include <limits>
#include <locale>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

#include <mpfr.h>

namespace mpfrxx {

class mpfr_class {
public:
    mpfr_class() : mpfr_class(precision_tag{}, default_precision()) {}

    mpfr_class(const mpfr_class& other)
    {
        mpfr_init2(value_, other.precision());
        const auto context = gmpfrxx_mkII::detail::current_eval_context(other.precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, other.value_, context.rounding_mode);
    }

    mpfr_class(mpfr_class&& other) noexcept
    {
        mpfr_init2(value_, other.precision());
        mpfr_swap(value_, other.value_);
    }

    explicit mpfr_class(double value) : mpfr_class(value, default_precision()) {}

    mpfr_class(double value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_d(value_, value, context.rounding_mode);
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    explicit mpfr_class(T value) : mpfr_class(value, default_precision())
    {
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    mpfr_class(T value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        set_integral(value);
    }

    mpfr_class(bool) = delete;
    mpfr_class(bool, mpfr_prec_t) = delete;

    explicit mpfr_class(const char* text) : mpfr_class(text, default_precision(), 10) {}

    explicit mpfr_class(const std::string& text) : mpfr_class(text.c_str(), default_precision(), 10) {}

    mpfr_class(const char* text, mpfr_prec_t precision, int base = 10)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        if (mpfr_set_str(value_, text, base, context.rounding_mode) != 0) {
            mpfr_set_ui(value_, 0, context.rounding_mode);
        }
    }

    mpfr_class(const std::string& text, mpfr_prec_t precision, int base = 10)
        : mpfr_class(text.c_str(), precision, base)
    {
    }

    explicit mpfr_class(mpfr_srcptr value)
    {
        const mpfr_prec_t precision = mpfr_get_prec(value);
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, value, context.rounding_mode);
    }

    mpfr_class(mpfr_srcptr value, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, value, context.rounding_mode);
    }

    mpfr_class(const mpfr_class& other, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set(value_, other.value_, context.rounding_mode);
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
    mpfr_class(const Expr& expr);

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>,
        typename = void>
    mpfr_class(const Expr& expr, mpfr_prec_t precision);

    ~mpfr_class()
    {
        mpfr_clear(value_);
    }

    mpfr_class& operator=(const mpfr_class& other)
    {
        if (this != &other) {
            const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
            const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
            mpfr_set(value_, other.value_, context.rounding_mode);
        }
        return *this;
    }

    mpfr_class& operator=(mpfr_class&& other) noexcept
    {
        if (this != &other) {
            if (this->precision() == other.precision()) {
                mpfr_swap(value_, other.value_);
            } else {
                const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
                const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
                mpfr_set(value_, other.value_, context.rounding_mode);
            }
        }
        return *this;
    }

    mpfr_class& operator=(double value)
    {
        set(value);
        return *this;
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    mpfr_class& operator=(T value)
    {
        set_integral(value);
        return *this;
    }

    mpfr_class& operator=(bool) = delete;

    mpfr_class& operator=(const char* text)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        if (mpfr_set_str(value_, text, 10, context.rounding_mode) != 0) {
            mpfr_set_ui(value_, 0, context.rounding_mode);
        }
        return *this;
    }

    mpfr_class& operator=(const std::string& text)
    {
        return *this = text.c_str();
    }

    template <
        typename Expr,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                    std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
    mpfr_class& operator=(const Expr& expr);

    static mpfr_class with_precision(mpfr_prec_t precision, double value)
    {
        mpfr_class result = with_precision(precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_d(result.value_, value, context.rounding_mode);
        return result;
    }

    static mpfr_class with_precision(mpfr_prec_t precision)
    {
        return mpfr_class(precision_tag{}, precision);
    }

    mpfr_prec_t precision() const noexcept
    {
        return mpfr_get_prec(value_);
    }

    mpfr_prec_t get_prec() const noexcept
    {
        return precision();
    }

    double to_double() const
    {
        return mpfr_get_d(value_, default_rounding());
    }

    void set(double value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_d(value_, value, context.rounding_mode);
    }

    template <
        typename T,
        typename = std::enable_if_t<gmpfrxx_mkII::detail::is_supported_expression_integral_v<T>>>
    void set(T value)
    {
        set_integral(value);
    }

    void set(const char* text)
    {
        if (set_str(text) != 0) {
            throw std::invalid_argument("invalid mpfr_class decimal string");
        }
    }

    void set(const std::string& text)
    {
        set(text.c_str());
    }

    int set_str(const char* text, int base = 10)
    {
        if (text == nullptr) {
            return -1;
        }

        mpfr_t temp;
        mpfr_init2(temp, precision());
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        const int rc = mpfr_set_str(temp, text, base, context.rounding_mode);
        if (rc == 0) {
            mpfr_set(value_, temp, context.rounding_mode);
        }
        mpfr_clear(temp);
        return rc;
    }

    int set_str(const std::string& text, int base = 10)
    {
        return set_str(text.c_str(), base);
    }

    std::string get_str() const
    {
        return to_string();
    }

    std::string get_str(mpfr_exp_t& exponent, int base = 10, std::size_t n_digits = 0) const
    {
        char* raw = mpfr_get_str(nullptr, &exponent, base, n_digits, value_, default_rounding());
        if (raw == nullptr) {
            throw std::runtime_error("mpfr_get_str failed");
        }
        std::string result(raw);
        mpfr_free_str(raw);
        return result;
    }

    std::string to_string(std::size_t n_digits = 0) const
    {
        if (mpfr_nan_p(value_)) {
            return "nan";
        }
        if (mpfr_inf_p(value_)) {
            return mpfr_sgn(value_) < 0 ? "-inf" : "inf";
        }

        mpfr_exp_t exponent = 0;
        std::string digits = get_str(exponent, 10, n_digits);

        bool negative = false;
        if (!digits.empty() && digits.front() == '-') {
            negative = true;
            digits.erase(digits.begin());
        }
        if (n_digits == 0) {
            while (digits.size() > 1 && digits.back() == '0') {
                digits.pop_back();
            }
        }

        if (digits == "0") {
            return negative ? "-0" : "0";
        }

        std::string result;
        if (negative) {
            result.push_back('-');
        }

        if (exponent <= 0) {
            result += "0.";
            result.append(static_cast<std::size_t>(-exponent), '0');
            result += digits;
        } else if (static_cast<std::size_t>(exponent) >= digits.size()) {
            result += digits;
            result.append(static_cast<std::size_t>(exponent) - digits.size(), '0');
        } else {
            const auto split = static_cast<std::size_t>(exponent);
            result += digits.substr(0, split);
            result.push_back('.');
            result += digits.substr(split);
        }

        return result;
    }

    const mpfr_t& mpfr_data() const noexcept
    {
        return value_;
    }

    mpfr_t& mpfr_data() noexcept
    {
        return value_;
    }

    const mpfr_t& get_mpfr_t() const noexcept
    {
        return value_;
    }

    mpfr_t& get_mpfr_t() noexcept
    {
        return value_;
    }

    void swap(mpfr_class& other) noexcept
    {
        mpfr_swap(value_, other.value_);
    }

    static mpfr_prec_t default_precision() noexcept
    {
        return default_precision_bits();
    }

    static mpfr_rnd_t default_rounding() noexcept
    {
        return default_rounding_mode();
    }

private:
    struct precision_tag {};

    mpfr_class(precision_tag, mpfr_prec_t precision)
    {
        mpfr_init2(value_, precision);
        const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        mpfr_set_ui(value_, 0, context.rounding_mode);
    }

    template <typename T>
    void set_integral(T value)
    {
        const auto context = gmpfrxx_mkII::detail::current_eval_context(this->precision());
        const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
        const gmpxx::mpz_class integer(value);
        mpfr_set_z(value_, integer.mpz_data(), context.rounding_mode);
    }

    mpfr_t value_;
};

inline void swap(mpfr_class& lhs, mpfr_class& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace mpfrxx

namespace gmpfrxx_mkII {
namespace detail {

inline int mpfr_stream_input_base(const std::ios_base& stream) noexcept
{
    const auto basefield = stream.flags() & std::ios_base::basefield;
    if (basefield == std::ios_base::hex) {
        return 16;
    }
    if (basefield == std::ios_base::oct) {
        return 8;
    }
    return 10;
}

inline int mpfr_digit_value(char c) noexcept
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    return -1;
}

inline bool mpfr_is_digit_for_base(char c, int base) noexcept
{
    const int value = mpfr_digit_value(c);
    return value >= 0 && value < base;
}

inline bool mpfr_stream_peek_char(std::istream& in, char& c)
{
    const auto next = in.rdbuf()->sgetc();
    if (next == std::char_traits<char>::eof()) {
        in.setstate(std::ios_base::eofbit);
        return false;
    }
    c = static_cast<char>(next);
    return true;
}

inline void mpfr_stream_get_char(std::istream& in, std::string& token)
{
    const auto next = in.rdbuf()->sbumpc();
    if (next == std::char_traits<char>::eof()) {
        in.setstate(std::ios_base::eofbit | std::ios_base::failbit);
        return;
    }
    token.push_back(static_cast<char>(next));
}

inline void mpfr_read_optional_sign(std::istream& in, std::string& token)
{
    char c = '\0';
    if (mpfr_stream_peek_char(in, c) && (c == '+' || c == '-')) {
        mpfr_stream_get_char(in, token);
    }
}

inline bool mpfr_read_digits_for_base(std::istream& in, std::string& token, int base)
{
    bool saw_digit = false;
    char c = '\0';
    while (mpfr_stream_peek_char(in, c) && mpfr_is_digit_for_base(c, base)) {
        mpfr_stream_get_char(in, token);
        saw_digit = true;
    }
    return saw_digit;
}

inline char mpfr_stream_decimal_point(const std::ios_base& stream)
{
    return std::use_facet<std::numpunct<char>>(stream.getloc()).decimal_point();
}

inline bool mpfr_read_float_token(std::istream& in, std::string& token, int base)
{
    token.clear();
    mpfr_read_optional_sign(in, token);
    const char decimal_point = mpfr_stream_decimal_point(in);

    bool saw_mantissa_digit = false;
    saw_mantissa_digit |= mpfr_read_digits_for_base(in, token, base);

    char c = '\0';
    if (mpfr_stream_peek_char(in, c) && c == decimal_point) {
        in.rdbuf()->sbumpc();
        token.push_back('.');
        saw_mantissa_digit |= mpfr_read_digits_for_base(in, token, base);
    }

    if (!saw_mantissa_digit) {
        return false;
    }

    const char exponent_marker = base == 10 ? 'e' : '@';
    if (!mpfr_stream_peek_char(in, c) ||
        (c != exponent_marker && !(base == 10 && c == 'E'))) {
        return true;
    }

    mpfr_stream_get_char(in, token);
    mpfr_read_optional_sign(in, token);

    bool saw_exponent_digit = false;
    while (mpfr_stream_peek_char(in, c) && std::isdigit(static_cast<unsigned char>(c))) {
        mpfr_stream_get_char(in, token);
        saw_exponent_digit = true;
    }
    return saw_exponent_digit;
}

inline std::string mpfr_strip_leading_plus(std::string token)
{
    if (!token.empty() && token.front() == '+') {
        token.erase(token.begin());
    }
    return token;
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

inline std::ostream& operator<<(std::ostream& out, const mpfr_class& value)
{
    try {
        char conversion = 'g';
        const auto floatfield = out.flags() & std::ios_base::floatfield;
        if (floatfield == std::ios_base::fixed) {
            conversion = 'f';
        } else if (floatfield == std::ios_base::scientific) {
            conversion = 'e';
        }
        if (out.flags() & std::ios_base::uppercase) {
            conversion = static_cast<char>(conversion - 'a' + 'A');
        }

        std::string format = (out.flags() & std::ios_base::showpoint) ? "%#.*R" : "%.*R";
        format.push_back(conversion);

        std::streamsize stream_precision = out.precision();
        if (stream_precision < 0) {
            stream_precision = 6;
        }
        if (stream_precision > static_cast<std::streamsize>(std::numeric_limits<int>::max())) {
            stream_precision = std::numeric_limits<int>::max();
        }

        char* raw = nullptr;
        const int count = mpfr_asprintf(
            &raw, format.c_str(), static_cast<int>(stream_precision), value.mpfr_data());
        if (count < 0 || raw == nullptr) {
            out.setstate(std::ios_base::badbit);
            return out;
        }

        std::string text(raw);
        mpfr_free_str(raw);
        if ((out.flags() & std::ios_base::showpos) && mpfr_sgn(value.mpfr_data()) >= 0) {
            text.insert(0, "+");
        }

        const std::streamsize width = out.width();
        if (static_cast<std::streamsize>(text.size()) < width) {
            const auto fill_count = static_cast<std::size_t>(width - static_cast<std::streamsize>(text.size()));
            if (out.flags() & std::ios_base::left) {
                text.append(fill_count, out.fill());
            } else if (out.flags() & std::ios_base::internal) {
                const std::size_t pos = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1 : 0;
                text.insert(pos, fill_count, out.fill());
            } else {
                text.insert(0, fill_count, out.fill());
            }
        }

        out << text;
        out.width(0);
    } catch (...) {
        out.setstate(std::ios_base::badbit);
    }
    return out;
}

template <
    typename Expr,
    typename = std::enable_if_t<gmpfrxx_mkII::detail::is_expression_node_v<std::decay_t<Expr>> &&
                                std::is_same_v<typename std::decay_t<Expr>::result_type, mpfr_class>>>
inline std::ostream& operator<<(std::ostream& out, const Expr& expr)
{
    return out << mpfr_class(expr);
}

inline std::istream& operator>>(std::istream& in, mpfr_class& value)
{
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    const int base = gmpfrxx_mkII::detail::mpfr_stream_input_base(in);
    std::string token;
    const bool parsed_token = gmpfrxx_mkII::detail::mpfr_read_float_token(in, token, base);
    const std::string parse_token = gmpfrxx_mkII::detail::mpfr_strip_leading_plus(std::move(token));

    mpfr_class tmp = mpfr_class::with_precision(value.precision());
    if (parsed_token && tmp.set_str(parse_token, base) == 0) {
        value.swap(tmp);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

} // namespace mpfrxx

namespace gmpfrxx_mkII {
namespace detail {

template <typename T, typename = void>
struct is_mpfr_expression_operand : std::false_type {};

template <typename T>
struct is_supported_mpfr_scalar
    : std::bool_constant<is_supported_expression_integral_v<T> ||
                         std::is_same_v<std::remove_cv_t<T>, float> ||
                         std::is_same_v<std::remove_cv_t<T>, double>> {};

template <typename T>
inline constexpr bool is_supported_mpfr_scalar_v =
    is_supported_mpfr_scalar<std::remove_cv_t<T>>::value;

template <typename T, typename = void>
struct normalized_mpfr_scalar;

template <typename T>
struct normalized_mpfr_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                  std::is_signed_v<std::remove_cv_t<T>>>> {
    using type = std::int64_t;
};

template <typename T>
struct normalized_mpfr_scalar<T, std::enable_if_t<is_supported_expression_integral_v<T> &&
                                                  std::is_unsigned_v<std::remove_cv_t<T>>>> {
    using type = std::uint64_t;
};

template <>
struct normalized_mpfr_scalar<float> {
    using type = double;
};

template <>
struct normalized_mpfr_scalar<double> {
    using type = double;
};

template <typename T>
using normalized_mpfr_scalar_t = typename normalized_mpfr_scalar<std::remove_cv_t<T>>::type;

template <>
struct is_mpfr_expression_operand<mpfrxx::mpfr_class> : std::true_type {};

template <>
struct is_mpfr_expression_operand<gmpxx::mpz_class> : std::true_type {};

template <>
struct is_mpfr_expression_operand<gmpxx::mpq_class> : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> && std::is_same_v<typename T::result_type, mpfrxx::mpfr_class>>>
    : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<
    T,
    std::enable_if_t<is_expression_node_v<T> &&
                     (std::is_same_v<typename T::result_type, gmpxx::mpz_class> ||
                      std::is_same_v<typename T::result_type, gmpxx::mpq_class>)>>
    : std::true_type {};

template <typename T>
struct is_mpfr_expression_operand<T, std::enable_if_t<is_supported_mpfr_scalar_v<T>>> : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_expression_operand_v =
    is_mpfr_expression_operand<std::decay_t<T>>::value;

template <typename T, typename = void>
struct is_mpfr_object_or_node : std::false_type {};

template <typename T>
struct is_mpfr_object_or_node<
    T,
    std::enable_if_t<std::is_same_v<std::decay_t<T>, mpfrxx::mpfr_class>>> : std::true_type {};

template <typename T>
struct is_mpfr_object_or_node<
    T,
    std::enable_if_t<is_expression_node_v<std::decay_t<T>> &&
                     std::is_same_v<typename std::decay_t<T>::result_type, mpfrxx::mpfr_class>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_mpfr_object_or_node_v = is_mpfr_object_or_node<T>::value;

inline object_leaf<mpfrxx::mpfr_class> make_mpfr_operand(const mpfrxx::mpfr_class& value)
{
    return object_leaf<mpfrxx::mpfr_class>(value);
}

inline object_leaf<gmpxx::mpz_class> make_mpfr_operand(const gmpxx::mpz_class& value)
{
    return object_leaf<gmpxx::mpz_class>(value);
}

inline object_leaf<gmpxx::mpq_class> make_mpfr_operand(const gmpxx::mpq_class& value)
{
    return object_leaf<gmpxx::mpq_class>(value);
}

template <typename Expr, typename = std::enable_if_t<is_expression_node_v<std::decay_t<Expr>>>>
std::decay_t<Expr> make_mpfr_operand(Expr&& expr)
{
    return std::forward<Expr>(expr);
}

template <typename Scalar, typename = std::enable_if_t<is_supported_mpfr_scalar_v<Scalar>>>
auto make_mpfr_operand(Scalar value)
{
    using storage_type = normalized_mpfr_scalar_t<Scalar>;
    return scalar_leaf<storage_type, mpfrxx::mpfr_class>(static_cast<storage_type>(value));
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return expr.get().precision();
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<gmpxx::mpz_class>&)
{
    return 0;
}

inline mpfr_prec_t mpfr_expression_precision(const object_leaf<gmpxx::mpq_class>&)
{
    return 0;
}

template <typename T, typename Result>
mpfr_prec_t mpfr_expression_precision(const scalar_leaf<T, Result>&)
{
    return mpfrxx::mpfr_class::default_precision();
}

template <typename Op, typename Expr, typename Result>
mpfr_prec_t mpfr_expression_precision(const unary_expr<Op, Expr, Result>& expr)
{
    return mpfr_expression_precision(expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
mpfr_prec_t mpfr_expression_precision(const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return std::max(mpfr_expression_precision(expr.lhs()), mpfr_expression_precision(expr.rhs()));
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<mpfrxx::mpfr_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set(dest, expr.get().mpfr_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<gmpxx::mpz_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set_z(dest, expr.get().mpz_data(), rnd);
}

inline void mpfr_evaluate(
    mpfr_t dest,
    const object_leaf<gmpxx::mpq_class>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    mpfr_set_q(dest, expr.get().mpq_data(), rnd);
}

template <typename T, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const scalar_leaf<T, Result>& expr,
    mpfr_prec_t,
    mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<T, double>) {
        mpfr_set_d(dest, expr.value(), rnd);
    } else if constexpr (std::is_same_v<T, std::int64_t> ||
                         std::is_same_v<T, std::uint64_t>) {
        const gmpxx::mpz_class integer(expr.value());
        mpfr_set_z(dest, integer.mpz_data(), rnd);
    } else {
        static_assert(std::is_same_v<T, double>, "unsupported MPFR scalar leaf");
    }
}

inline bool mpfr_expression_references(
    const mpfr_t target,
    const object_leaf<mpfrxx::mpfr_class>& expr)
{
    return static_cast<const void*>(&target[0]) ==
           static_cast<const void*>(&expr.get().mpfr_data()[0]);
}

inline bool mpfr_expression_references(const mpfr_t, const object_leaf<gmpxx::mpz_class>&)
{
    return false;
}

inline bool mpfr_expression_references(const mpfr_t, const object_leaf<gmpxx::mpq_class>&)
{
    return false;
}

template <typename T, typename Result>
bool mpfr_expression_references(const mpfr_t, const scalar_leaf<T, Result>&)
{
    return false;
}

template <typename Op, typename Expr, typename Result>
bool mpfr_expression_references(
    const mpfr_t target,
    const unary_expr<Op, Expr, Result>& expr)
{
    return mpfr_expression_references(target, expr.expr());
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
bool mpfr_expression_references(
    const mpfr_t target,
    const binary_expr<Op, Lhs, Rhs, Result>& expr)
{
    return mpfr_expression_references(target, expr.lhs()) ||
           mpfr_expression_references(target, expr.rhs());
}

template <typename Op>
void mpfr_apply_binary(mpfr_t dest, const mpfr_t lhs, const mpfr_t rhs, mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<Op, add_op>) {
        mpfr_add(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, sub_op>) {
        mpfr_sub(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, mul_op>) {
        mpfr_mul(dest, lhs, rhs, rnd);
    } else if constexpr (std::is_same_v<Op, div_op>) {
        mpfr_div(dest, lhs, rhs, rnd);
    } else {
        static_assert(std::is_same_v<Op, add_op>, "unsupported MPFR expression operation");
    }
}

template <typename Expr, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const unary_expr<pos_op, Expr, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    mpfr_evaluate(dest, expr.expr(), eval_precision, rnd);
}

template <typename Expr, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const unary_expr<neg_op, Expr, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    mpfr_evaluate(dest, expr.expr(), eval_precision, rnd);
    mpfr_neg(dest, dest, rnd);
}

template <typename Expr>
void mpfr_evaluate_to_temporary(
    mpfr_t temp,
    const Expr& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    mpfr_init2(temp, eval_precision);
    mpfr_evaluate(temp, expr, eval_precision, rnd);
}

template <typename Op, typename Lhs, typename Rhs, typename Result>
void mpfr_evaluate(
    mpfr_t dest,
    const binary_expr<Op, Lhs, Rhs, Result>& expr,
    mpfr_prec_t eval_precision,
    mpfr_rnd_t rnd)
{
    if constexpr (std::is_same_v<Result, gmpxx::mpz_class>) {
        mpz_t exact;
        mpz_init(exact);
        mpz_evaluate(exact, expr);
        mpfr_set_z(dest, exact, rnd);
        mpz_clear(exact);
        return;
    } else if constexpr (std::is_same_v<Result, gmpxx::mpq_class>) {
        mpq_t exact;
        mpq_init(exact);
        mpq_evaluate(exact, expr);
        mpfr_set_q(dest, exact, rnd);
        mpq_clear(exact);
        return;
    }

    if (mpfr_expression_references(dest, expr)) {
        mpfr_t lhs;
        mpfr_t rhs;
        mpfr_evaluate_to_temporary(lhs, expr.lhs(), eval_precision, rnd);
        mpfr_evaluate_to_temporary(rhs, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, lhs, rhs, rnd);
        mpfr_clear(rhs);
        mpfr_clear(lhs);
        return;
    }

    if constexpr (std::is_same_v<Lhs, object_leaf<mpfrxx::mpfr_class>> &&
                  std::is_same_v<Rhs, object_leaf<mpfrxx::mpfr_class>>) {
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), expr.rhs().get().mpfr_data(), rnd);
    } else if constexpr (std::is_same_v<Rhs, object_leaf<mpfrxx::mpfr_class>>) {
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, dest, expr.rhs().get().mpfr_data(), rnd);
    } else if constexpr (std::is_same_v<Lhs, object_leaf<mpfrxx::mpfr_class>> &&
                         (std::is_same_v<Op, add_op> || std::is_same_v<Op, mul_op>)) {
        mpfr_evaluate(dest, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, expr.lhs().get().mpfr_data(), dest, rnd);
    } else {
        mpfr_t rhs;
        mpfr_evaluate(dest, expr.lhs(), eval_precision, rnd);
        mpfr_evaluate_to_temporary(rhs, expr.rhs(), eval_precision, rnd);
        mpfr_apply_binary<Op>(dest, dest, rhs, rnd);
        mpfr_clear(rhs);
    }
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
                                    int> = 0>
auto operator+(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<add_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
                                    int> = 0>
auto operator-(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<sub_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
                                    int> = 0>
auto operator*(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<mul_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Lhs, typename Rhs, std::enable_if_t<
                                    is_mpfr_expression_operand_v<Lhs> &&
                                        is_mpfr_expression_operand_v<Rhs> &&
                                        (is_mpfr_object_or_node_v<Lhs> ||
                                         is_mpfr_object_or_node_v<Rhs>),
                                    int> = 0>
auto operator/(Lhs&& lhs, Rhs&& rhs)
{
    auto left = make_mpfr_operand(std::forward<Lhs>(lhs));
    auto right = make_mpfr_operand(std::forward<Rhs>(rhs));
    return binary_expr<div_op, decltype(left), decltype(right), mpfrxx::mpfr_class>(
        std::move(left), std::move(right));
}

template <typename Expr, std::enable_if_t<is_mpfr_expression_operand_v<Expr>, int> = 0>
auto operator+(Expr&& expr)
{
    auto operand = make_mpfr_operand(std::forward<Expr>(expr));
    return unary_expr<pos_op, decltype(operand), mpfrxx::mpfr_class>(std::move(operand));
}

template <typename Expr, std::enable_if_t<is_mpfr_expression_operand_v<Expr>, int> = 0>
auto operator-(Expr&& expr)
{
    auto operand = make_mpfr_operand(std::forward<Expr>(expr));
    return unary_expr<neg_op, decltype(operand), mpfrxx::mpfr_class>(std::move(operand));
}

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

template <typename Expr, typename>
mpfr_class::mpfr_class(const Expr& expr)
{
    const mpfr_prec_t precision = gmpfrxx_mkII::detail::mpfr_expression_precision(expr);
    mpfr_init2(value_, precision);
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
}

template <typename Expr, typename, typename>
mpfr_class::mpfr_class(const Expr& expr, mpfr_prec_t precision)
{
    mpfr_init2(value_, precision);
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
}

template <typename Expr, typename>
mpfr_class& mpfr_class::operator=(const Expr& expr)
{
    const mpfr_prec_t precision = this->precision();
    const auto context = gmpfrxx_mkII::detail::current_eval_context(precision);
    const gmpfrxx_mkII::detail::mpfr_exponent_range_guard range_guard(context.emin, context.emax);
    gmpfrxx_mkII::detail::mpfr_evaluate(value_, expr, precision, context.rounding_mode);
    return *this;
}

using ::gmpfrxx_mkII::detail::operator+;
using ::gmpfrxx_mkII::detail::operator-;
using ::gmpfrxx_mkII::detail::operator*;
using ::gmpfrxx_mkII::detail::operator/;

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_MPFR_IMPL_HPP
