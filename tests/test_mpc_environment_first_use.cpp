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

#include "test_env.hpp"

#include <mpfrxx_mkII.h>
#include <mpcxx_mkII.h>

#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace {

void clear_environment()
{
    gmpfrxx_mkII_tests::unset_environment_variable("MPFRXX_DEFAULT_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPFRXX_DEFAULT_ROUNDING_MODE");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_REAL_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_IMAG_PRECISION_BITS");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_ROUNDING_MODE");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_REAL_ROUNDING_MODE");
    gmpfrxx_mkII_tests::unset_environment_variable("MPCXX_DEFAULT_IMAG_ROUNDING_MODE");
}

void require_mpc_defaults(mpfr_prec_t real_precision,
                          mpfr_prec_t imag_precision,
                          mpfr_rnd_t real_rounding,
                          mpfr_rnd_t imag_rounding,
                          mpfr_prec_t mpfr_precision,
                          mpfr_rnd_t mpfr_rounding)
{
    const auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != real_precision || defaults.imag_precision_bits != imag_precision) {
        std::abort();
    }
    if (defaults.real_rounding_mode != real_rounding || defaults.imag_rounding_mode != imag_rounding) {
        std::abort();
    }
    if (mpfrxx::default_precision_bits() != mpfr_precision ||
        mpfrxx::default_rounding_mode() != mpfr_rounding) {
        std::abort();
    }
}

void test_missing_mpc_env_shares_mpfr_default()
{
    clear_environment();
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_PRECISION_BITS", "176", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_ROUNDING_MODE", "RNDD", 1);

    const mpfrxx::mpc_class value;
    if (value.real_precision() != 176 || value.imag_precision() != 176) {
        std::abort();
    }
    require_mpc_defaults(176, 176, MPFR_RNDD, MPFR_RNDD, 176, MPFR_RNDD);
}

void test_mpc_env_overrides_first_use_default()
{
    clear_environment();
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_PRECISION_BITS", "176", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_ROUNDING_MODE", "RNDD", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_PRECISION_BITS", "224", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_ROUNDING_MODE", "RNDU", 1);

    const mpfrxx::mpc_class value;
    if (value.real_precision() != 224 || value.imag_precision() != 224) {
        std::abort();
    }
    require_mpc_defaults(224, 224, MPFR_RNDU, MPFR_RNDU, 176, MPFR_RNDD);
}

void test_component_mpc_env_overrides_first_use_default()
{
    clear_environment();
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_PRECISION_BITS", "176", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPFRXX_DEFAULT_ROUNDING_MODE", "RNDD", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_PRECISION_BITS", "256", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_ROUNDING_MODE", "RNDZ", 1);

    const mpfrxx::mpc_class value;
    if (value.real_precision() != 256 || value.imag_precision() != 176) {
        std::abort();
    }
    require_mpc_defaults(256, 176, MPFR_RNDZ, MPFR_RNDD, 176, MPFR_RNDD);
}

void test_component_mpc_env_installs_asymmetric_override_on_first_use()
{
    clear_environment();
    mpfrxx::set_default_precision_bits(160);
    mpfrxx::set_default_rounding_mode(MPFR_RNDN);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_PRECISION_BITS", "192", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_IMAG_PRECISION_BITS", "224", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_REAL_ROUNDING_MODE", "RNDU", 1);
    gmpfrxx_mkII_tests::set_environment_variable("MPCXX_DEFAULT_IMAG_ROUNDING_MODE", "RNDD", 1);

    const auto defaults = mpfrxx::default_mpc_options();
    if (defaults.real_precision_bits != 192 || defaults.imag_precision_bits != 224) {
        std::abort();
    }
    if (defaults.real_rounding_mode != MPFR_RNDU || defaults.imag_rounding_mode != MPFR_RNDD) {
        std::abort();
    }
    if (mpfrxx::default_precision_bits() != 160 || mpfrxx::default_rounding_mode() != MPFR_RNDN) {
        std::abort();
    }
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }

    if (std::strcmp(argv[1], "share_mpfr") == 0) {
        test_missing_mpc_env_shares_mpfr_default();
        return 0;
    }
    if (std::strcmp(argv[1], "mpc_env") == 0) {
        test_mpc_env_overrides_first_use_default();
        return 0;
    }
    if (std::strcmp(argv[1], "component_env") == 0) {
        test_component_mpc_env_overrides_first_use_default();
        return 0;
    }
    if (std::strcmp(argv[1], "asymmetric_override") == 0) {
        test_component_mpc_env_installs_asymmetric_override_on_first_use();
        return 0;
    }

    return 1;
}
