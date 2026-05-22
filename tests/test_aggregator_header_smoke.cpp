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
#include <type_traits>

int main()
{
    unsetenv("GMPXX_DEFAULT_MPF_PRECISION_BITS");
    unsetenv("MPFRXX_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_DEFAULT_EMIN");
    unsetenv("MPFRXX_DEFAULT_EMAX");
    unsetenv("MPFRXX_DEFAULT_ROUNDING_MODE");

    static_assert(std::is_default_constructible_v<gmpxx::mpz_class>);
    static_assert(std::is_default_constructible_v<gmpxx::mpq_class>);
    static_assert(std::is_default_constructible_v<gmpxx::mpf_class>);
    static_assert(std::is_default_constructible_v<mpfrxx::mpfr_class>);
    static_assert(std::is_default_constructible_v<mpfrxx::mpc_class>);

    gmpxx::mpf_class mpf_value;
    if (mpf_value.precision() < 512) {
        std::abort();
    }

    mpfrxx::initialize_thread_defaults();

    mpfrxx::mpfr_class mpfr_value;
    if (mpfr_value.precision() != 512) {
        std::abort();
    }

    return 0;
}
