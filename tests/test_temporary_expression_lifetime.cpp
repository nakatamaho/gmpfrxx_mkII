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

#include <cassert>

int main()
{
    {
        auto expr = gmpxx::mpz_class(10) + gmpxx::mpz_class(32);
        const gmpxx::mpz_class value(expr);
        assert(value == gmpxx::mpz_class(42));
    }

    {
        auto expr = gmpxx::mpq_class(1, 3) + gmpxx::mpq_class(1, 2);
        const gmpxx::mpq_class value(expr);
        assert(value == gmpxx::mpq_class(5, 6));
    }

    {
        auto expr = gmpxx::mpf_class(1.25) + gmpxx::mpf_class(2.5);
        const gmpxx::mpf_class value(expr);
        assert(value == gmpxx::mpf_class(3.75));
    }

    {
        auto expr = gmpxx::mpfc_class(gmpxx::mpf_class(1), gmpxx::mpf_class(2)) +
                    gmpxx::mpfc_class(gmpxx::mpf_class(3), gmpxx::mpf_class(4));
        const gmpxx::mpfc_class value(expr);
        assert(value.real() == gmpxx::mpf_class(4));
        assert(value.imag() == gmpxx::mpf_class(6));
    }

    {
        auto expr = mpfrxx::mpfr_class(1.25) + mpfrxx::mpfr_class(2.5);
        const mpfrxx::mpfr_class value(expr);
        assert(value == mpfrxx::mpfr_class(3.75));
    }

    {
        auto expr = mpfrxx::mpc_class(mpfrxx::mpfr_class(1), mpfrxx::mpfr_class(2)) +
                    mpfrxx::mpc_class(mpfrxx::mpfr_class(3), mpfrxx::mpfr_class(4));
        const mpfrxx::mpc_class value(expr);
        assert(value == mpfrxx::mpc_class(mpfrxx::mpfr_class(4), mpfrxx::mpfr_class(6)));
    }

    return 0;
}
