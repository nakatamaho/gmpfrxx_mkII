// ============================================================
// gmpfrxx_mkII.h — MPFR / GMP C++20 Expression-Template Wrapper
// Single-header, RAII, four arithmetic ops (+, -, *, /)
// Requires: MPFR >= 4.0, GMP, C++20
// SPDX-License-Identifier: BSD-2-Clause
// ============================================================
#pragma once

#include <mpfr.h>
#include <gmp.h>

#include <algorithm>
#include <concepts>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

// ============================================================
// 1. Config — forward declarations, tags, helpers
// ============================================================

class mpfr_class;
class mpz_class;
class mpq_class;

struct prec_tag_t { explicit prec_tag_t() = default; };
inline constexpr prec_tag_t prec_tag{};

namespace detail {

// GMP-allocated string helper (frees via mp_get_memory_functions)
struct GmpStr {
    char* s;
    explicit GmpStr(char* p) : s(p) {}
    ~GmpStr() {
        if (s) {
            void (*freefunc)(void*, size_t);
            mp_get_memory_functions(nullptr, nullptr, &freefunc);
            freefunc(s, std::strlen(s) + 1);
        }
    }
    GmpStr(const GmpStr&) = delete;
    GmpStr& operator=(const GmpStr&) = delete;
};

} // namespace detail

// ============================================================
// 2. Concepts — MpfrExpr, Scalar
// ============================================================

template <typename T>
concept MpfrExpr = requires(const T& t, mpfr_ptr p, mpfr_rnd_t r) {
    { t.eval(p, r) };
    { t.get_prec() } -> std::convertible_to<mpfr_prec_t>;
};

template <typename T>
concept Scalar = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

// ============================================================
// 3. Expression framework — ExprStorage, ScalarExpr, Ops,
//    BinaryExpr, TmpMpfr
// ============================================================

// --- 3a. ExprStorage policy ---
// Default: store by value (safe for temporaries like BinaryExpr).
// Leaf RAII classes: store by const& (caller must keep them alive).
template <typename T>
struct ExprStorage { using type = T; };

template <>
struct ExprStorage<mpfr_class> { using type = const mpfr_class&; };
template <>
struct ExprStorage<mpz_class> { using type = const mpz_class&; };
template <>
struct ExprStorage<mpq_class> { using type = const mpq_class&; };

// --- 3b. ScalarExpr ---
template <typename S>
    requires Scalar<S>
class ScalarExpr {
    S val_;

public:
    explicit ScalarExpr(S v) : val_(v) {}

    void eval(mpfr_ptr dst, mpfr_rnd_t rnd) const {
        if constexpr (std::is_same_v<S, long double>) {
            mpfr_set_ld(dst, val_, rnd);
        } else if constexpr (std::is_floating_point_v<S>) {
            mpfr_set_d(dst, static_cast<double>(val_), rnd);
        } else if constexpr (std::is_signed_v<S>) {
            mpfr_set_si(dst, static_cast<long>(val_), rnd);
        } else {
            mpfr_set_ui(dst, static_cast<unsigned long>(val_), rnd);
        }
    }

    mpfr_prec_t get_prec() const {
        if constexpr (std::is_same_v<S, float>)
            return 24;
        else if constexpr (std::is_same_v<S, double>)
            return 53;
        else if constexpr (std::is_same_v<S, long double>)
            return 64;
        else
            return 64; // integral
    }
};

// --- 3c. Operation tags ---
struct AddOp {
    static void apply(mpfr_ptr d, mpfr_srcptr a, mpfr_srcptr b, mpfr_rnd_t r) {
        mpfr_add(d, a, b, r);
    }
};
struct SubOp {
    static void apply(mpfr_ptr d, mpfr_srcptr a, mpfr_srcptr b, mpfr_rnd_t r) {
        mpfr_sub(d, a, b, r);
    }
};
struct MulOp {
    static void apply(mpfr_ptr d, mpfr_srcptr a, mpfr_srcptr b, mpfr_rnd_t r) {
        mpfr_mul(d, a, b, r);
    }
};
struct DivOp {
    static void apply(mpfr_ptr d, mpfr_srcptr a, mpfr_srcptr b, mpfr_rnd_t r) {
        mpfr_div(d, a, b, r);
    }
};

// --- 3d. TmpMpfr — RAII temporary ---
class TmpMpfr {
    mpfr_t mp_;

public:
    explicit TmpMpfr(mpfr_prec_t prec) { mpfr_init2(mp_, prec); }
    ~TmpMpfr() { mpfr_clear(mp_); }
    TmpMpfr(const TmpMpfr&) = delete;
    TmpMpfr& operator=(const TmpMpfr&) = delete;

    mpfr_ptr   ptr()       { return mp_; }
    mpfr_srcptr ptr() const { return mp_; }
};

// --- 3e. BinaryExpr ---
template <typename Op, typename L, typename R>
class BinaryExpr {
    using LStore = typename ExprStorage<L>::type;
    using RStore = typename ExprStorage<R>::type;
    LStore lhs_;
    RStore rhs_;

public:
    BinaryExpr(const L& l, const R& r) : lhs_(l), rhs_(r) {}

    void eval(mpfr_ptr dst, mpfr_rnd_t rnd) const {
        mpfr_prec_t p = get_prec();
        TmpMpfr tl(p), tr(p);
        lhs_.eval(tl.ptr(), rnd);
        rhs_.eval(tr.ptr(), rnd);
        Op::apply(dst, tl.ptr(), tr.ptr(), rnd);
    }

    mpfr_prec_t get_prec() const {
        return std::max(lhs_.get_prec(), rhs_.get_prec());
    }
};

// ============================================================
// 4. RAII classes — mpz_class, mpq_class
// ============================================================

// --- 4a. mpz_class ---
class mpz_class {
    mpz_t mp_;

public:
    mpz_class()                          { mpz_init(mp_); }
    explicit mpz_class(long v)           { mpz_init_set_si(mp_, v); }
    explicit mpz_class(unsigned long v)  { mpz_init_set_ui(mp_, v); }
    explicit mpz_class(int v)            { mpz_init_set_si(mp_, v); }
    explicit mpz_class(const char* s, int base = 10) {
        mpz_init(mp_);
        if (mpz_set_str(mp_, s, base) != 0)
            throw std::invalid_argument("mpz_class: invalid string");
    }

    mpz_class(const mpz_class& o)       { mpz_init_set(mp_, o.mp_); }
    mpz_class(mpz_class&& o) noexcept   { mp_[0] = o.mp_[0]; mpz_init(o.mp_); }
    ~mpz_class()                         { mpz_clear(mp_); }

    mpz_class& operator=(const mpz_class& o) {
        if (this != &o) mpz_set(mp_, o.mp_);
        return *this;
    }
    mpz_class& operator=(mpz_class&& o) noexcept {
        if (this != &o) mpz_swap(mp_, o.mp_);
        return *this;
    }

    mpz_srcptr get_mpz_t() const { return mp_; }
    mpz_ptr    get_mpz_t()       { return mp_; }

    // MpfrExpr interface
    void eval(mpfr_ptr dst, mpfr_rnd_t rnd) const {
        mpfr_set_z(dst, mp_, rnd);
    }
    mpfr_prec_t get_prec() const {
        auto bits = static_cast<mpfr_prec_t>(mpz_sizeinbase(mp_, 2));
        return std::max(bits, mpfr_get_default_prec());
    }

    friend std::ostream& operator<<(std::ostream& os, const mpz_class& z) {
        detail::GmpStr gs(mpz_get_str(nullptr, 10, z.mp_));
        return os << gs.s;
    }
};

// --- 4b. mpq_class ---
class mpq_class {
    mpq_t mp_;

public:
    mpq_class() { mpq_init(mp_); }
    explicit mpq_class(long num, unsigned long den = 1) {
        mpq_init(mp_);
        mpq_set_si(mp_, num, den);
        mpq_canonicalize(mp_);
    }
    explicit mpq_class(const char* s, int base = 10) {
        mpq_init(mp_);
        if (mpq_set_str(mp_, s, base) != 0)
            throw std::invalid_argument("mpq_class: invalid string");
        mpq_canonicalize(mp_);
    }

    mpq_class(const mpq_class& o) { mpq_init(mp_); mpq_set(mp_, o.mp_); }
    mpq_class(mpq_class&& o) noexcept { mp_[0] = o.mp_[0]; mpq_init(o.mp_); }
    ~mpq_class() { mpq_clear(mp_); }

    mpq_class& operator=(const mpq_class& o) {
        if (this != &o) mpq_set(mp_, o.mp_);
        return *this;
    }
    mpq_class& operator=(mpq_class&& o) noexcept {
        if (this != &o) mpq_swap(mp_, o.mp_);
        return *this;
    }

    mpq_srcptr get_mpq_t() const { return mp_; }
    mpq_ptr    get_mpq_t()       { return mp_; }

    // MpfrExpr interface
    void eval(mpfr_ptr dst, mpfr_rnd_t rnd) const {
        mpfr_set_q(dst, mp_, rnd);
    }
    mpfr_prec_t get_prec() const {
        return mpfr_get_default_prec();
    }

    friend std::ostream& operator<<(std::ostream& os, const mpq_class& q) {
        detail::GmpStr gs(mpq_get_str(nullptr, 10, q.mp_));
        return os << gs.s;
    }
};

// ============================================================
// 5. RAII class — mpfr_class
// ============================================================

class mpfr_class {
    mpfr_t mp_;

    // Helper: decide if E is a leaf RAII type (not an expression template)
    template <typename E>
    static constexpr bool is_leaf_v =
        std::is_same_v<std::decay_t<E>, mpfr_class> ||
        std::is_same_v<std::decay_t<E>, mpz_class>  ||
        std::is_same_v<std::decay_t<E>, mpq_class>;

public:
    // --- constructors ---

    // Default: default precision, value 0
    mpfr_class() {
        mpfr_init(mp_);
        mpfr_set_ui(mp_, 0, MPFR_RNDN);
    }

    // Precision-tagged (avoids ambiguity with integer ctors)
    mpfr_class(prec_tag_t, mpfr_prec_t prec) {
        mpfr_init2(mp_, prec);
        mpfr_set_ui(mp_, 0, MPFR_RNDN);
    }

    // From double
    explicit mpfr_class(double d, mpfr_prec_t prec = 0) {
        mpfr_init2(mp_, prec > 0 ? prec : mpfr_get_default_prec());
        mpfr_set_d(mp_, d, MPFR_RNDN);
    }

    // From long
    explicit mpfr_class(long v, mpfr_prec_t prec = 0) {
        mpfr_init2(mp_, prec > 0 ? prec : mpfr_get_default_prec());
        mpfr_set_si(mp_, v, MPFR_RNDN);
    }

    // From unsigned long
    explicit mpfr_class(unsigned long v, mpfr_prec_t prec = 0) {
        mpfr_init2(mp_, prec > 0 ? prec : mpfr_get_default_prec());
        mpfr_set_ui(mp_, v, MPFR_RNDN);
    }

    // From int (delegate to long)
    explicit mpfr_class(int v, mpfr_prec_t prec = 0)
        : mpfr_class(static_cast<long>(v), prec) {}

    // From C string
    explicit mpfr_class(const char* s, mpfr_prec_t prec = 0, int base = 10) {
        mpfr_init2(mp_, prec > 0 ? prec : mpfr_get_default_prec());
        if (mpfr_set_str(mp_, s, base, MPFR_RNDN) != 0)
            throw std::invalid_argument("mpfr_class: invalid string");
    }

    // From mpz_class
    explicit mpfr_class(const mpz_class& z, mpfr_prec_t prec = 0) {
        mpfr_init2(mp_, prec > 0 ? prec : mpfr_get_default_prec());
        mpfr_set_z(mp_, z.get_mpz_t(), MPFR_RNDN);
    }

    // From mpq_class
    explicit mpfr_class(const mpq_class& q, mpfr_prec_t prec = 0) {
        mpfr_init2(mp_, prec > 0 ? prec : mpfr_get_default_prec());
        mpfr_set_q(mp_, q.get_mpq_t(), MPFR_RNDN);
    }

    // Copy
    mpfr_class(const mpfr_class& o) {
        mpfr_init2(mp_, mpfr_get_prec(o.mp_));
        mpfr_set(mp_, o.mp_, MPFR_RNDN);
    }

    // Move
    mpfr_class(mpfr_class&& o) noexcept {
        mp_[0] = o.mp_[0];
        mpfr_init(o.mp_);
    }

    // From expression template (not a leaf, not a scalar)
    template <MpfrExpr E>
        requires (!is_leaf_v<E>) && (!Scalar<std::decay_t<E>>)
    mpfr_class(const E& expr) {                         // NOLINT implicit
        mpfr_prec_t p = expr.get_prec();
        mpfr_init2(mp_, p);
        expr.eval(mp_, MPFR_RNDN);
    }

    // --- destructor ---
    ~mpfr_class() { mpfr_clear(mp_); }

    // --- assignment ---

    mpfr_class& operator=(const mpfr_class& o) {
        if (this != &o) mpfr_set(mp_, o.mp_, MPFR_RNDN);
        return *this;
    }

    mpfr_class& operator=(mpfr_class&& o) noexcept {
        if (this != &o) mpfr_swap(mp_, o.mp_);
        return *this;
    }

    // From MpfrExpr (uses *this precision)
    template <MpfrExpr E>
        requires (!std::is_same_v<std::decay_t<E>, mpfr_class>)
    mpfr_class& operator=(const E& expr) {
        expr.eval(mp_, MPFR_RNDN);
        return *this;
    }

    // From scalar
    template <Scalar S>
    mpfr_class& operator=(S val) {
        ScalarExpr<S>(val).eval(mp_, MPFR_RNDN);
        return *this;
    }

    // --- compound assignment (MpfrExpr) ---

    template <MpfrExpr E>
    mpfr_class& operator+=(const E& e) {
        TmpMpfr t(mpfr_get_prec(mp_));
        e.eval(t.ptr(), MPFR_RNDN);
        mpfr_add(mp_, mp_, t.ptr(), MPFR_RNDN);
        return *this;
    }
    template <MpfrExpr E>
    mpfr_class& operator-=(const E& e) {
        TmpMpfr t(mpfr_get_prec(mp_));
        e.eval(t.ptr(), MPFR_RNDN);
        mpfr_sub(mp_, mp_, t.ptr(), MPFR_RNDN);
        return *this;
    }
    template <MpfrExpr E>
    mpfr_class& operator*=(const E& e) {
        TmpMpfr t(mpfr_get_prec(mp_));
        e.eval(t.ptr(), MPFR_RNDN);
        mpfr_mul(mp_, mp_, t.ptr(), MPFR_RNDN);
        return *this;
    }
    template <MpfrExpr E>
    mpfr_class& operator/=(const E& e) {
        TmpMpfr t(mpfr_get_prec(mp_));
        e.eval(t.ptr(), MPFR_RNDN);
        mpfr_div(mp_, mp_, t.ptr(), MPFR_RNDN);
        return *this;
    }

    // --- compound assignment (Scalar, optimised) ---

    template <Scalar S>
    mpfr_class& operator+=(S v) {
        if constexpr (std::is_floating_point_v<S>)
            mpfr_add_d(mp_, mp_, static_cast<double>(v), MPFR_RNDN);
        else if constexpr (std::is_signed_v<S>)
            mpfr_add_si(mp_, mp_, static_cast<long>(v), MPFR_RNDN);
        else
            mpfr_add_ui(mp_, mp_, static_cast<unsigned long>(v), MPFR_RNDN);
        return *this;
    }
    template <Scalar S>
    mpfr_class& operator-=(S v) {
        if constexpr (std::is_floating_point_v<S>)
            mpfr_sub_d(mp_, mp_, static_cast<double>(v), MPFR_RNDN);
        else if constexpr (std::is_signed_v<S>)
            mpfr_sub_si(mp_, mp_, static_cast<long>(v), MPFR_RNDN);
        else
            mpfr_sub_ui(mp_, mp_, static_cast<unsigned long>(v), MPFR_RNDN);
        return *this;
    }
    template <Scalar S>
    mpfr_class& operator*=(S v) {
        if constexpr (std::is_floating_point_v<S>)
            mpfr_mul_d(mp_, mp_, static_cast<double>(v), MPFR_RNDN);
        else if constexpr (std::is_signed_v<S>)
            mpfr_mul_si(mp_, mp_, static_cast<long>(v), MPFR_RNDN);
        else
            mpfr_mul_ui(mp_, mp_, static_cast<unsigned long>(v), MPFR_RNDN);
        return *this;
    }
    template <Scalar S>
    mpfr_class& operator/=(S v) {
        if constexpr (std::is_floating_point_v<S>)
            mpfr_div_d(mp_, mp_, static_cast<double>(v), MPFR_RNDN);
        else if constexpr (std::is_signed_v<S>)
            mpfr_div_si(mp_, mp_, static_cast<long>(v), MPFR_RNDN);
        else
            mpfr_div_ui(mp_, mp_, static_cast<unsigned long>(v), MPFR_RNDN);
        return *this;
    }

    // --- MpfrExpr interface ---

    void eval(mpfr_ptr dst, mpfr_rnd_t rnd) const {
        mpfr_set(dst, mp_, rnd);
    }
    mpfr_prec_t get_prec() const { return mpfr_get_prec(mp_); }

    // --- accessors ---

    mpfr_srcptr get_mpfr_t() const { return mp_; }
    mpfr_ptr    get_mpfr_t()       { return mp_; }

    void set_prec(mpfr_prec_t prec) {
        mpfr_set_prec(mp_, prec);
        mpfr_set_ui(mp_, 0, MPFR_RNDN);
    }

    double to_double() const { return mpfr_get_d(mp_, MPFR_RNDN); }

    // --- I/O ---

    friend std::ostream& operator<<(std::ostream& os, const mpfr_class& x) {
        char* s = nullptr;
        mpfr_asprintf(&s, "%.20Rg", x.mp_);
        os << s;
        mpfr_free_str(s);
        return os;
    }
};

// ============================================================
// 6. Operators — +, -, *, / for all MpfrExpr × MpfrExpr and
//    MpfrExpr × Scalar combinations
// ============================================================

// --- 6a. MpfrExpr ⊕ MpfrExpr ---

template <MpfrExpr L, MpfrExpr R>
auto operator+(const L& l, const R& r) {
    return BinaryExpr<AddOp, L, R>(l, r);
}
template <MpfrExpr L, MpfrExpr R>
auto operator-(const L& l, const R& r) {
    return BinaryExpr<SubOp, L, R>(l, r);
}
template <MpfrExpr L, MpfrExpr R>
auto operator*(const L& l, const R& r) {
    return BinaryExpr<MulOp, L, R>(l, r);
}
template <MpfrExpr L, MpfrExpr R>
auto operator/(const L& l, const R& r) {
    return BinaryExpr<DivOp, L, R>(l, r);
}

// --- 6b. MpfrExpr ⊕ Scalar ---

template <MpfrExpr L, Scalar S>
auto operator+(const L& l, S s) {
    return BinaryExpr<AddOp, L, ScalarExpr<S>>(l, ScalarExpr<S>(s));
}
template <MpfrExpr L, Scalar S>
auto operator-(const L& l, S s) {
    return BinaryExpr<SubOp, L, ScalarExpr<S>>(l, ScalarExpr<S>(s));
}
template <MpfrExpr L, Scalar S>
auto operator*(const L& l, S s) {
    return BinaryExpr<MulOp, L, ScalarExpr<S>>(l, ScalarExpr<S>(s));
}
template <MpfrExpr L, Scalar S>
auto operator/(const L& l, S s) {
    return BinaryExpr<DivOp, L, ScalarExpr<S>>(l, ScalarExpr<S>(s));
}

// --- 6c. Scalar ⊕ MpfrExpr ---

template <Scalar S, MpfrExpr R>
auto operator+(S s, const R& r) {
    return BinaryExpr<AddOp, ScalarExpr<S>, R>(ScalarExpr<S>(s), r);
}
template <Scalar S, MpfrExpr R>
auto operator-(S s, const R& r) {
    return BinaryExpr<SubOp, ScalarExpr<S>, R>(ScalarExpr<S>(s), r);
}
template <Scalar S, MpfrExpr R>
auto operator*(S s, const R& r) {
    return BinaryExpr<MulOp, ScalarExpr<S>, R>(ScalarExpr<S>(s), r);
}
template <Scalar S, MpfrExpr R>
auto operator/(S s, const R& r) {
    return BinaryExpr<DivOp, ScalarExpr<S>, R>(ScalarExpr<S>(s), r);
}

// --- 6d. Unary minus (convenience) ---

template <MpfrExpr E>
auto operator-(const E& e) {
    return BinaryExpr<SubOp, ScalarExpr<int>, E>(ScalarExpr<int>(0), e);
}


