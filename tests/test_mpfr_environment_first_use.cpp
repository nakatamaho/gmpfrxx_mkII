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

#include <cstdlib>
#include <cstring>

namespace {

void configure_environment()
{
    setenv("MPFRXX_DEFAULT_PRECISION_BITS", "160", 1);
    setenv("MPFRXX_EMIN", "-64", 1);
    setenv("MPFRXX_EMAX", "64", 1);
    setenv("MPFRXX_ROUNDING_MODE", "RNDU", 1);
}

void clear_environment()
{
    unsetenv("MPFRXX_DEFAULT_PRECISION_BITS");
    unsetenv("MPFRXX_EMIN");
    unsetenv("MPFRXX_EMAX");
    unsetenv("MPFRXX_ROUNDING_MODE");
}

void require_environment_applied()
{
    const auto defaults = mpfrxx::default_options();
    if (defaults.precision_bits != 160) {
        std::abort();
    }
    if (defaults.emin != -64 || defaults.emax != 64) {
        std::abort();
    }
    if (defaults.rounding_mode != MPFR_RNDU) {
        std::abort();
    }
    if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDU) {
        std::abort();
    }
}

void test_with_precision_first_use()
{
    configure_environment();

    const auto rounded = mpfrxx::mpfr_class::with_precision(2, 1.25);
    if (rounded.to_double() != 1.5) {
        std::abort();
    }
    require_environment_applied();
}

void test_explicit_precision_constructor_first_use()
{
    configure_environment();

    const mpfrxx::mpfr_class rounded(1.25, 2);
    if (rounded.to_double() != 1.5) {
        std::abort();
    }
    require_environment_applied();
}

void test_current_eval_context_first_use()
{
    configure_environment();

    const auto context = gmpfrxx_mkII::detail::current_eval_context(53);
    if (context.precision_bits != 53 || context.rounding_mode != MPFR_RNDU) {
        std::abort();
    }
    require_environment_applied();
}

void test_initialize_thread_defaults_first_use()
{
    configure_environment();

    mpfrxx::initialize_thread_defaults();
    require_environment_applied();

    mpfr_set_default_prec(53);
    const auto defaults = mpfrxx::default_options();
    if (defaults.precision_bits != 53) {
        std::abort();
    }

    mpfrxx::mpfr_class value;
    if (value.precision() != 53) {
        std::abort();
    }
}

void test_raw_rounding_preserved_first_use()
{
    clear_environment();
    mpfr_set_default_rounding_mode(MPFR_RNDD);

    const auto context = gmpfrxx_mkII::detail::current_eval_context(53);
    if (context.rounding_mode != MPFR_RNDD) {
        std::abort();
    }
    if (gmpfrxx_mkII::detail::current_rounding_mode() != MPFR_RNDD) {
        std::abort();
    }
    if (mpfrxx::default_rounding_mode() != MPFR_RNDD) {
        std::abort();
    }
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }

    if (std::strcmp(argv[1], "with_precision") == 0) {
        test_with_precision_first_use();
        return 0;
    }
    if (std::strcmp(argv[1], "explicit_precision_constructor") == 0) {
        test_explicit_precision_constructor_first_use();
        return 0;
    }
    if (std::strcmp(argv[1], "current_eval_context") == 0) {
        test_current_eval_context_first_use();
        return 0;
    }
    if (std::strcmp(argv[1], "initialize_thread_defaults") == 0) {
        test_initialize_thread_defaults_first_use();
        return 0;
    }
    if (std::strcmp(argv[1], "raw_rounding_preserved") == 0) {
        test_raw_rounding_preserved_first_use();
        return 0;
    }

    return 1;
}
