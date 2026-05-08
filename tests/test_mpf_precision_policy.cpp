/*
 * Copyright (c) 2026
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <gmpxx_mkII.h>

#include <algorithm>
#include <cstdlib>
#include <type_traits>
#include <utility>

namespace {

template <typename T, typename = void>
struct has_public_set_prec_raw : std::false_type {};

template <typename T>
struct has_public_set_prec_raw<
    T,
    decltype(void(std::declval<T&>().set_prec_raw(std::declval<mp_bitcnt_t>())))> : std::true_type {};

void require_mpf_equal(const gmpxx::mpf_class& got, mpf_srcptr expected)
{
    if (mpf_cmp(got.mpf_data(), expected) != 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    static_assert(std::is_same<decltype(std::declval<gmpxx::mpf_class&>().set_prec(
                                    std::declval<mp_bitcnt_t>())),
                               void>::value,
                  "");
    static_assert(!has_public_set_prec_raw<gmpxx::mpf_class>::value, "");

    unsetenv("MPFXX_DEFAULT_PREC_BITS");
    gmpxx::reload_default_mpf_precision_bits_from_environment();

    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    gmpxx::mpf_class builtin_default;
    if (builtin_default.precision() < 512) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "320", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 320) {
        std::abort();
    }
    gmpxx::mpf_class env_default;
    if (env_default.precision() < 320) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "0", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    setenv("MPFXX_DEFAULT_PREC_BITS", "not-a-number", 1);
    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    gmpxx::set_default_mpf_precision_bits(160);
    gmpxx::mpf_class default_value;
    if (default_value.precision() < 160) {
        std::abort();
    }

    gmpxx::set_default_mpf_precision_bits(0);
    if (gmpxx::default_mpf_precision_bits() != 160) {
        std::abort();
    }

    const auto low = gmpxx::mpf_class::with_precision(80, 1.0);
    const auto high = gmpxx::mpf_class::with_precision(192, 2.0);

    gmpxx::mpf_class materialized = low + high;
    if (materialized.precision() != std::max(low.precision(), high.precision())) {
        std::abort();
    }

    auto destination = gmpxx::mpf_class::with_precision(96);
    const auto destination_precision = destination.precision();
    destination = high + materialized;
    if (destination.precision() != destination_precision) {
        std::abort();
    }

    {
        gmpxx::mpf_class a("1.0", 64);
        gmpxx::mpf_class b("2.0", 1024);
        gmpxx::mpf_class r = a + b;
        if (r.get_prec() != std::max(a.get_prec(), b.get_prec())) {
            std::abort();
        }

        gmpxx::mpf_class assigned("0.0", 128);
        const mp_bitcnt_t old_prec = assigned.get_prec();
        assigned = a + b;
        mpf_t ref;
        mpf_init2(ref, old_prec);
        mpf_add(ref, a.mpf_data(), b.mpf_data());
        if (assigned.get_prec() != old_prec) {
            std::abort();
        }
        require_mpf_equal(assigned, ref);
        mpf_clear(ref);

        auto evaluated = (a + b).eval();
        if (evaluated.get_prec() != std::max(a.get_prec(), b.get_prec())) {
            std::abort();
        }
    }

    {
        gmpxx::mpf_class a("1.0", 64);
        gmpxx::mpf_class b("2.0", 1024);
        gmpxx::mpf_class r("1.0", 128);
        const mp_bitcnt_t old_prec = r.get_prec();

        mpf_t ref;
        mpf_init2(ref, old_prec);
        mpf_add(ref, a.mpf_data(), b.mpf_data());
        mpf_add(ref, r.mpf_data(), ref);

        r += a + b;

        if (r.get_prec() != old_prec) {
            std::abort();
        }
        require_mpf_equal(r, ref);
        mpf_clear(ref);
    }

    {
        gmpxx::mpf_class value("1.234567890123456789", 256);
        mpf_t ref;
        mpf_init2(ref, value.get_prec());
        mpf_set(ref, value.mpf_data());

        value.set_prec(64);
        mpf_set_prec(ref, 64);

        if (value.get_prec() != mpf_get_prec(ref)) {
            std::abort();
        }
        require_mpf_equal(value, ref);
        mpf_clear(ref);
    }

    return 0;
}
