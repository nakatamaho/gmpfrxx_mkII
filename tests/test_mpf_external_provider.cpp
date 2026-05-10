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
#include <thread>

int main()
{
    if (gmpxx_mkII_default_context_mode_v1() != GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER) {
        std::abort();
    }

    const void* token = gmpxx_mkII_default_context_provider_token_v1();
    if (token == nullptr || token != gmpxx_mkII_default_context_provider_token_v1()) {
        std::abort();
    }

    gmpxx::mpf_class initial;
    if (initial.precision() < 512) {
        std::abort();
    }

    gmpxx::set_default_mpf_precision_bits(768);
    gmpxx::mpf_class changed;
    if (gmpxx::default_mpf_precision_bits() != 768 || changed.precision() < 768) {
        std::abort();
    }

    {
        gmpxx::default_mpf_precision_guard guard(384);
        gmpxx::mpf_class guarded;
        if (gmpxx::default_mpf_precision_bits() != 384 || guarded.precision() < 384) {
            std::abort();
        }
    }
    if (gmpxx::default_mpf_precision_bits() != 768) {
        std::abort();
    }

    bool thread_ok = false;
    std::thread worker([&] {
        gmpxx::mpf_class thread_initial;
        thread_ok = gmpxx::default_mpf_precision_bits() == 512 && thread_initial.precision() >= 512;
        gmpxx::set_default_mpf_precision_bits(640);
        gmpxx::mpf_class thread_changed;
        thread_ok = thread_ok &&
                    gmpxx::default_mpf_precision_bits() == 640 &&
                    thread_changed.precision() >= 640;
    });
    worker.join();

    if (!thread_ok || gmpxx::default_mpf_precision_bits() != 768) {
        std::abort();
    }

    gmpxx::reload_default_mpf_precision_bits_from_environment();
    if (gmpxx::default_mpf_precision_bits() != 512) {
        std::abort();
    }

    return 0;
}
