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

#include <cstdlib>
#include <stdexcept>

namespace {

void require_num_den(const gmpxx::mpq_class& value, long numerator, unsigned long denominator)
{
    mpz_t num;
    mpz_t den;
    mpz_init(num);
    mpz_init(den);
    mpq_get_num(num, value.mpq_data());
    mpq_get_den(den, value.mpq_data());
    const bool ok = mpz_cmp_si(num, numerator) == 0 && mpz_cmp_ui(den, denominator) == 0;
    mpz_clear(den);
    mpz_clear(num);
    if (!ok) {
        std::abort();
    }
}

template <typename Function>
void require_invalid_argument(Function&& function)
{
    try {
        function();
    } catch (const std::invalid_argument&) {
        return;
    }
    std::abort();
}

template <typename Function>
void require_domain_error(Function&& function)
{
    try {
        function();
    } catch (const std::domain_error&) {
        return;
    }
    std::abort();
}

} // namespace

int main()
{
    gmpxx::mpq_class reduced(gmpxx::mpz_class(2), gmpxx::mpz_class(4));
    require_num_den(reduced, 1, 2);

    gmpxx::mpq_class sign(gmpxx::mpz_class(2), gmpxx::mpz_class(-4));
    require_num_den(sign, -1, 2);

    require_invalid_argument([] {
        (void)gmpxx::mpq_class(gmpxx::mpz_class(1), gmpxx::mpz_class(0));
    });

    require_invalid_argument([] {
        (void)gmpxx::mpq_class(1, 0);
    });

    gmpxx::mpq_class stream_like;
    mpq_set_str(stream_like.mpq_data(), "6/8", 10);
    require_num_den(stream_like, 6, 8);
    if (stream_like.has_zero_denominator()) {
        std::abort();
    }
    stream_like.canonicalize();
    require_num_den(stream_like, 3, 4);

    mpq_set_str(stream_like.mpq_data(), "1/0", 10);
    if (!stream_like.has_zero_denominator()) {
        std::abort();
    }
    require_domain_error([&] {
        stream_like.canonicalize();
    });

    mpq_t raw;
    mpq_init(raw);
    mpq_set_str(raw, "10/20", 10);
    gmpxx::mpq_canonicalize_checked(raw);
    if (gmpxx::mpq_has_zero_denominator(raw) ||
        mpz_cmp_ui(mpq_numref(raw), 1) != 0 ||
        mpz_cmp_ui(mpq_denref(raw), 2) != 0) {
        std::abort();
    }
    mpq_set_str(raw, "0/0", 10);
    if (!gmpxx::mpq_has_zero_denominator(raw)) {
        std::abort();
    }
    require_domain_error([&] {
        gmpxx::mpq_canonicalize_checked(raw);
    });
    mpq_clear(raw);

    gmpxx::mpq_class result = reduced + reduced;
    require_num_den(result, 1, 1);

    return 0;
}
