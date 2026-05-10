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

#include <gmpfrxx_mkII/detail/gmp_default_context.hpp>

namespace {

gmpxx_mkII_default_context_v1 make_initial_context() noexcept
{
    gmpxx_mkII_default_context_v1 context{};
    context.abi_version = gmpfrxx_mkII::detail::gmp_default_context_abi_version;
    context.struct_size = sizeof(context);
    context.mpf_precision_bits =
        static_cast<std::uint64_t>(gmpfrxx_mkII::detail::read_default_mpf_precision_from_environment_or_abort());
    return context;
}

const gmpxx_mkII_default_context_v1& initial_context() noexcept
{
    static const gmpxx_mkII_default_context_v1 context = make_initial_context();
    return context;
}

gmpxx_mkII_default_context_v1& thread_context() noexcept
{
    static thread_local gmpxx_mkII_default_context_v1 context = initial_context();
    return context;
}

int provider_token_storage = 0;

} // namespace

extern "C" GMPXX_MKII_API void gmpxx_mkII_get_current_default_context_v1(
    gmpxx_mkII_default_context_v1* out) noexcept
{
    if (out == nullptr) {
        return;
    }

    *out = thread_context();
}

extern "C" GMPXX_MKII_API void gmpxx_mkII_set_thread_default_context_v1(
    const gmpxx_mkII_default_context_v1* context) noexcept
{
    if (context == nullptr) {
        return;
    }

    gmpfrxx_mkII::detail::validate_default_context_or_abort(*context);
    thread_context() = *context;
}

extern "C" GMPXX_MKII_API void gmpxx_mkII_reset_thread_default_context_v1() noexcept
{
    thread_context() = initial_context();
}

extern "C" GMPXX_MKII_API const void* gmpxx_mkII_default_context_provider_token_v1() noexcept
{
    return &provider_token_storage;
}

extern "C" GMPXX_MKII_API int gmpxx_mkII_default_context_mode_v1() noexcept
{
    return GMPXX_MKII_DEFAULT_CONTEXT_EXTERNAL_PROVIDER;
}
