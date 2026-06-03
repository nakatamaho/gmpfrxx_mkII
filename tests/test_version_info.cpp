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
#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>

int main()
{
    static_assert(std::is_same_v<decltype(gmpxx::git_commit_hash()), const char*>);
    static_assert(std::is_same_v<decltype(mpfrxx::git_commit_hash()), const char*>);
    static_assert(std::is_same_v<decltype(gmpxx::version()), const char*>);
    static_assert(std::is_same_v<decltype(mpfrxx::version()), const char*>);

    const char* gmp_version = gmpxx::version();
    const char* mpfrxx_version = mpfrxx::version();
    assert(gmp_version != nullptr);
    assert(mpfrxx_version != nullptr);
    assert(std::strcmp(gmp_version, "1.0.0") == 0);
    assert(std::strcmp(gmp_version, mpfrxx_version) == 0);
    assert(std::strcmp(gmp_version, GMPFRXX_MKII_VERSION) == 0);

    const char* gmp_hash = gmpxx::git_commit_hash();
    const char* mpfr_hash = mpfrxx::git_commit_hash();
    assert(gmp_hash != nullptr);
    assert(mpfr_hash != nullptr);
    assert(std::strlen(gmp_hash) > 0);
    assert(std::strcmp(gmp_hash, mpfr_hash) == 0);
    assert(std::strcmp(gmp_hash, GMPFRXX_MKII_GIT_COMMIT_HASH) == 0);

    std::ostringstream gmp_out;
    gmpxx::print_git_commit_hash(gmp_out);
    assert(gmp_out.str() == gmp_hash);

    std::ostringstream mpfr_out;
    mpfrxx::print_git_commit_hash(mpfr_out);
    assert(mpfr_out.str() == mpfr_hash);

    std::ostringstream gmp_version_out;
    gmpxx::print_version(gmp_version_out);
    assert(gmp_version_out.str() == gmp_version);

    std::ostringstream mpfr_version_out;
    mpfrxx::print_version(mpfr_version_out);
    assert(mpfr_version_out.str() == mpfrxx_version);

    return 0;
}
