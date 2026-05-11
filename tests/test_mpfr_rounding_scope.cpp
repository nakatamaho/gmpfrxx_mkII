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
 * OR SERVICES; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gmpfrxx_mkII/detail/eval_context.hpp>
#include <mpfrxx_mkII.h>

int main()
{
    const mpfr_rnd_t old_rounding = mpfrxx::default_rounding_mode();

    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDN) {
        return 1;
    }

    {
        mpfrxx::rounding_mode_scope outer(MPFR_RNDU);
        if (mpfrxx::default_rounding_mode() != MPFR_RNDU) {
            return 2;
        }
        if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDU) {
            return 3;
        }

        {
            mpfrxx::rounding_mode_scope inner(MPFR_RNDD);
            if (mpfrxx::default_rounding_mode() != MPFR_RNDD) {
                return 4;
            }
            if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDD) {
                return 5;
            }
        }

        if (mpfrxx::default_rounding_mode() != MPFR_RNDU) {
            return 6;
        }
        if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDU) {
            return 7;
        }
    }

    if (mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        return 8;
    }
    if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDN) {
        return 9;
    }

    mpfrxx::set_default_rounding_mode(old_rounding);
    return 0;
}
