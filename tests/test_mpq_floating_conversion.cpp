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

#include <gmpfrxx_mkII.h>

#include <cstdlib>
#include <stdexcept>

namespace {

template <typename Function>
void require_domain_error(Function&& function)
{
    bool threw = false;
    try {
        function();
    } catch (const std::domain_error&) {
        threw = true;
    }
    if (!threw) {
        std::abort();
    }
}

gmpxx::mpq_class make_zero_denominator_mpq()
{
    gmpxx::mpq_class value;
    if (mpq_set_str(value.mpq_data(), "1/0", 10) != 0) {
        std::abort();
    }
    if (!value.has_zero_denominator()) {
        std::abort();
    }
    return value;
}

void test_mpf_rejects_zero_denominator_mpq()
{
    const gmpxx::mpq_class q = make_zero_denominator_mpq();

    require_domain_error([&] { (void)gmpxx::mpf_class(q); });
    require_domain_error([&] { (void)gmpxx::mpf_class(q, static_cast<mp_bitcnt_t>(128)); });

    gmpxx::mpf_class target(0.0, static_cast<mp_bitcnt_t>(128));
    require_domain_error([&] { target = q; });

    const gmpxx::mpf_class one(1.0, static_cast<mp_bitcnt_t>(128));
    require_domain_error([&] { (void)gmpxx::mpf_class(q + one); });
}

void test_mpfc_rejects_zero_denominator_mpq_expression()
{
    const gmpxx::mpq_class q = make_zero_denominator_mpq();
    const gmpxx::mpfc_class one(gmpxx::mpf_class(1.0, static_cast<mp_bitcnt_t>(128)));

    require_domain_error([&] { (void)gmpxx::mpfc_class(q + one); });
}

void test_mpfr_rejects_zero_denominator_mpq()
{
    const gmpxx::mpq_class q = make_zero_denominator_mpq();

    require_domain_error([&] { (void)mpfrxx::mpfr_class(q, static_cast<mpfr_prec_t>(128)); });

    mpfrxx::mpfr_class target(0.0, static_cast<mpfr_prec_t>(128));
    require_domain_error([&] { target = q; });

    const mpfrxx::mpfr_class one(1.0, static_cast<mpfr_prec_t>(128));
    require_domain_error([&] { (void)mpfrxx::mpfr_class(q + one); });
}

void test_mpc_rejects_zero_denominator_mpq()
{
    const gmpxx::mpq_class q = make_zero_denominator_mpq();

    require_domain_error([&] { (void)mpfrxx::mpc_class(q); });

    mpfrxx::mpc_class target;
    require_domain_error([&] { target = q; });

    const mpfrxx::mpc_class one(1.0);
    require_domain_error([&] { (void)mpfrxx::mpc_class(q + one); });
}

} // namespace

int main()
{
    test_mpf_rejects_zero_denominator_mpq();
    test_mpfc_rejects_zero_denominator_mpq_expression();
    test_mpfr_rejects_zero_denominator_mpq();
    test_mpc_rejects_zero_denominator_mpq();
    return 0;
}
