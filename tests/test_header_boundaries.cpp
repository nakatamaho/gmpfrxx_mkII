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
#include <fstream>
#include <sstream>
#include <string>

#ifndef GMPFRXX_MKII_SOURCE_DIR
#define GMPFRXX_MKII_SOURCE_DIR "."
#endif

namespace {

std::string read_file(const char* path)
{
    std::ifstream in(path);
    if (!in) {
        std::abort();
    }
    std::ostringstream out;
    out << in.rdbuf();
    return out.str();
}

void require_absent(const std::string& text, const char* needle)
{
    if (text.find(needle) != std::string::npos) {
        std::abort();
    }
}

} // namespace

int main()
{
    const std::string source_dir = GMPFRXX_MKII_SOURCE_DIR;
    const std::string gmp_header_path = source_dir + "/include/gmpxx_mkII.h";
    const std::string mpfr_header_path = source_dir + "/include/mpfrxx_mkII.h";

    const std::string gmp_header = read_file(gmp_header_path.c_str());
    require_absent(gmp_header, "#include <mpfr.h>");
    require_absent(gmp_header, "#include <mpc.h>");
    require_absent(gmp_header, "mpfr_");
    require_absent(gmp_header, "mpc_");
    require_absent(gmp_header, "mpc_t");

    const std::string mpfr_header = read_file(mpfr_header_path.c_str());
    if (mpfr_header.find("#include <mpc.h>") == std::string::npos) {
        std::abort();
    }
    require_absent(mpfr_header, "#include <gmpxx_mkII.h>");
    require_absent(mpfr_header, "mpf_impl.hpp");
    require_absent(mpfr_header, "mpfc_impl.hpp");

    return 0;
}
