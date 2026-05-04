#ifndef GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP
#define GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <mpc.h>
#include <mpfr.h>

namespace gmpfrxx_mkII {
namespace detail {

struct parsed_mpfr_environment {
    mpfr_prec_t precision;
    mpfr_exp_t emin;
    mpfr_exp_t emax;
    mpfr_rnd_t rounding;
};

inline mpfr_prec_t builtin_mpfr_default_precision() noexcept
{
    return 512;
}

inline mpfr_rnd_t builtin_mpfr_default_rounding() noexcept
{
    return MPFR_RNDN;
}

inline bool parse_mpfr_precision(const char* text, mpfr_prec_t& out) noexcept
{
    if (text == nullptr || *text == '\0' || *text == '-') {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const unsigned long long value = std::strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value == 0) {
        return false;
    }
    if (value > static_cast<unsigned long long>(std::numeric_limits<mpfr_prec_t>::max())) {
        return false;
    }
    if (value < static_cast<unsigned long long>(MPFR_PREC_MIN)) {
        return false;
    }
    out = static_cast<mpfr_prec_t>(value);
    return true;
}

inline bool parse_mpfr_exponent(const char* text, mpfr_exp_t& out) noexcept
{
    if (text == nullptr || *text == '\0') {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const long long value = std::strtoll(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return false;
    }
    if (value < static_cast<long long>(std::numeric_limits<mpfr_exp_t>::min()) ||
        value > static_cast<long long>(std::numeric_limits<mpfr_exp_t>::max())) {
        return false;
    }
    out = static_cast<mpfr_exp_t>(value);
    return true;
}

inline bool parse_mpfr_rounding(const char* text, mpfr_rnd_t& out) noexcept
{
    if (text == nullptr) {
        return false;
    }

    if (std::strcmp(text, "RNDN") == 0 || std::strcmp(text, "MPFR_RNDN") == 0) {
        out = MPFR_RNDN;
        return true;
    }
    if (std::strcmp(text, "RNDZ") == 0 || std::strcmp(text, "MPFR_RNDZ") == 0) {
        out = MPFR_RNDZ;
        return true;
    }
    if (std::strcmp(text, "RNDU") == 0 || std::strcmp(text, "MPFR_RNDU") == 0) {
        out = MPFR_RNDU;
        return true;
    }
    if (std::strcmp(text, "RNDD") == 0 || std::strcmp(text, "MPFR_RNDD") == 0) {
        out = MPFR_RNDD;
        return true;
    }
    if (std::strcmp(text, "RNDA") == 0 || std::strcmp(text, "MPFR_RNDA") == 0) {
        out = MPFR_RNDA;
        return true;
    }
    if (std::strcmp(text, "RNDF") == 0 || std::strcmp(text, "MPFR_RNDF") == 0) {
        out = MPFR_RNDF;
        return true;
    }

    return false;
}

inline parsed_mpfr_environment load_mpfr_environment() noexcept
{
    parsed_mpfr_environment result{
        builtin_mpfr_default_precision(),
        mpfr_get_emin(),
        mpfr_get_emax(),
        builtin_mpfr_default_rounding(),
    };

    mpfr_prec_t precision = result.precision;
    if (parse_mpfr_precision(std::getenv("MPFRXX_DEFAULT_PRECISION_BITS"), precision)) {
        result.precision = precision;
    }

    mpfr_exp_t emin = result.emin;
    mpfr_exp_t emax = result.emax;
    const bool has_emin = parse_mpfr_exponent(std::getenv("MPFRXX_EMIN"), emin);
    const bool has_emax = parse_mpfr_exponent(std::getenv("MPFRXX_EMAX"), emax);
    if (has_emin && has_emax && emin <= emax) {
        result.emin = emin;
        result.emax = emax;
    } else if (has_emin && !has_emax && emin <= result.emax) {
        result.emin = emin;
    } else if (!has_emin && has_emax && result.emin <= emax) {
        result.emax = emax;
    }

    mpfr_rnd_t rounding = result.rounding;
    if (parse_mpfr_rounding(std::getenv("MPFRXX_ROUNDING_MODE"), rounding)) {
        result.rounding = rounding;
    }

    return result;
}

class mpfr_exponent_range_guard {
public:
    mpfr_exponent_range_guard(mpfr_exp_t emin, mpfr_exp_t emax)
        : old_emin_(mpfr_get_emin()), old_emax_(mpfr_get_emax())
    {
        active_ = (old_emin_ != emin) || (old_emax_ != emax);
        if (active_) {
            mpfr_set_emin(emin);
            mpfr_set_emax(emax);
        }
    }

    mpfr_exponent_range_guard(const mpfr_exponent_range_guard&) = delete;
    mpfr_exponent_range_guard& operator=(const mpfr_exponent_range_guard&) = delete;

    ~mpfr_exponent_range_guard()
    {
        if (active_) {
            mpfr_set_emin(old_emin_);
            mpfr_set_emax(old_emax_);
        }
    }

private:
    mpfr_exp_t old_emin_;
    mpfr_exp_t old_emax_;
    bool active_{false};
};

} // namespace detail
} // namespace gmpfrxx_mkII

namespace mpfrxx {

struct mpfr_default_options {
    mpfr_prec_t precision_bits;
    mpfr_exp_t emin;
    mpfr_exp_t emax;
    mpfr_rnd_t rounding_mode;
};

inline mpfr_default_options& mutable_mpfr_default_options()
{
    static mpfr_default_options options = [] {
        const auto loaded = ::gmpfrxx_mkII::detail::load_mpfr_environment();
        return mpfr_default_options{
            loaded.precision,
            loaded.emin,
            loaded.emax,
            loaded.rounding,
        };
    }();
    return options;
}

inline void reload_mpfr_defaults_from_environment()
{
    const auto loaded = ::gmpfrxx_mkII::detail::load_mpfr_environment();
    mutable_mpfr_default_options() = mpfr_default_options{
        loaded.precision,
        loaded.emin,
        loaded.emax,
        loaded.rounding,
    };
}

inline mpfr_default_options default_options()
{
    return mutable_mpfr_default_options();
}

inline mpfr_prec_t default_precision_bits()
{
    return mutable_mpfr_default_options().precision_bits;
}

inline void set_default_precision_bits(mpfr_prec_t precision)
{
    if (precision >= MPFR_PREC_MIN) {
        mutable_mpfr_default_options().precision_bits = precision;
    }
}

inline mpfr_rnd_t default_rounding_mode()
{
    return mutable_mpfr_default_options().rounding_mode;
}

inline void set_default_rounding_mode(mpfr_rnd_t rounding)
{
    mutable_mpfr_default_options().rounding_mode = rounding;
}

inline mpfr_exp_t default_emin()
{
    return mutable_mpfr_default_options().emin;
}

inline mpfr_exp_t default_emax()
{
    return mutable_mpfr_default_options().emax;
}

inline void set_default_exponent_range(mpfr_exp_t emin, mpfr_exp_t emax)
{
    if (emin <= emax) {
        mutable_mpfr_default_options().emin = emin;
        mutable_mpfr_default_options().emax = emax;
    }
}

struct mpc_default_options {
    mpfr_prec_t real_precision_bits;
    mpfr_prec_t imag_precision_bits;
    mpfr_rnd_t real_rounding_mode;
    mpfr_rnd_t imag_rounding_mode;
};

inline mpc_default_options load_mpc_defaults_from_environment()
{
    const auto inherited = default_options();
    mpc_default_options result{
        inherited.precision_bits,
        inherited.precision_bits,
        inherited.rounding_mode,
        inherited.rounding_mode,
    };

    mpfr_prec_t precision = inherited.precision_bits;
    if (::gmpfrxx_mkII::detail::parse_mpfr_precision(
            std::getenv("MPFRXX_MPC_DEFAULT_PRECISION_BITS"), precision)) {
        result.real_precision_bits = precision;
        result.imag_precision_bits = precision;
    }

    mpfr_prec_t real_precision = result.real_precision_bits;
    if (::gmpfrxx_mkII::detail::parse_mpfr_precision(
            std::getenv("MPFRXX_MPC_REAL_PRECISION_BITS"), real_precision)) {
        result.real_precision_bits = real_precision;
    }

    mpfr_prec_t imag_precision = result.imag_precision_bits;
    if (::gmpfrxx_mkII::detail::parse_mpfr_precision(
            std::getenv("MPFRXX_MPC_IMAG_PRECISION_BITS"), imag_precision)) {
        result.imag_precision_bits = imag_precision;
    }

    mpfr_rnd_t rounding = inherited.rounding_mode;
    if (::gmpfrxx_mkII::detail::parse_mpfr_rounding(
            std::getenv("MPFRXX_MPC_ROUNDING_MODE"), rounding)) {
        result.real_rounding_mode = rounding;
        result.imag_rounding_mode = rounding;
    }

    mpfr_rnd_t real_rounding = result.real_rounding_mode;
    if (::gmpfrxx_mkII::detail::parse_mpfr_rounding(
            std::getenv("MPFRXX_MPC_REAL_ROUNDING_MODE"), real_rounding)) {
        result.real_rounding_mode = real_rounding;
    }

    mpfr_rnd_t imag_rounding = result.imag_rounding_mode;
    if (::gmpfrxx_mkII::detail::parse_mpfr_rounding(
            std::getenv("MPFRXX_MPC_IMAG_ROUNDING_MODE"), imag_rounding)) {
        result.imag_rounding_mode = imag_rounding;
    }

    return result;
}

inline mpc_default_options& mutable_mpc_default_options()
{
    static mpc_default_options options = load_mpc_defaults_from_environment();
    return options;
}

inline void reload_mpc_defaults_from_environment()
{
    mutable_mpc_default_options() = load_mpc_defaults_from_environment();
}

inline mpc_default_options default_mpc_options()
{
    return mutable_mpc_default_options();
}

inline mpfr_prec_t default_mpc_real_precision_bits()
{
    return mutable_mpc_default_options().real_precision_bits;
}

inline mpfr_prec_t default_mpc_imag_precision_bits()
{
    return mutable_mpc_default_options().imag_precision_bits;
}

inline mpfr_prec_t default_mpc_precision_bits()
{
    const auto options = mutable_mpc_default_options();
    return options.real_precision_bits == options.imag_precision_bits
        ? options.real_precision_bits
        : std::min(options.real_precision_bits, options.imag_precision_bits);
}

inline void set_default_mpc_precision_bits(mpfr_prec_t precision)
{
    if (precision >= MPFR_PREC_MIN) {
        mutable_mpc_default_options().real_precision_bits = precision;
        mutable_mpc_default_options().imag_precision_bits = precision;
    }
}

inline void set_default_mpc_precision_bits(mpfr_prec_t real_precision, mpfr_prec_t imag_precision)
{
    if (real_precision >= MPFR_PREC_MIN && imag_precision >= MPFR_PREC_MIN) {
        mutable_mpc_default_options().real_precision_bits = real_precision;
        mutable_mpc_default_options().imag_precision_bits = imag_precision;
    }
}

inline mpfr_rnd_t default_mpc_real_rounding_mode()
{
    return mutable_mpc_default_options().real_rounding_mode;
}

inline mpfr_rnd_t default_mpc_imag_rounding_mode()
{
    return mutable_mpc_default_options().imag_rounding_mode;
}

inline mpc_rnd_t default_mpc_rounding_mode()
{
    const auto options = mutable_mpc_default_options();
    return MPC_RND(options.real_rounding_mode, options.imag_rounding_mode);
}

inline void set_default_mpc_rounding_mode(mpfr_rnd_t rounding)
{
    mutable_mpc_default_options().real_rounding_mode = rounding;
    mutable_mpc_default_options().imag_rounding_mode = rounding;
}

inline void set_default_mpc_rounding_mode(mpfr_rnd_t real_rounding, mpfr_rnd_t imag_rounding)
{
    mutable_mpc_default_options().real_rounding_mode = real_rounding;
    mutable_mpc_default_options().imag_rounding_mode = imag_rounding;
}

} // namespace mpfrxx

#endif // GMPFRXX_MKII_DETAIL_ENVIRONMENT_HPP
