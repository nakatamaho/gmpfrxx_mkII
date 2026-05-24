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
 */

#include "Rdot_microbench_common.hpp"

void _Rdot_microbench(int64_t n, mpf_t *dx, mpf_t *dy, mpf_t *) {
    unsigned long long sink = 0;
    for (int64_t i = 0; i < n; ++i) {
        const __mpf_struct &x = dx[i][0];
        const __mpf_struct &y = dy[i][0];
        sink += static_cast<unsigned long long>(x._mp_size);
        sink += static_cast<unsigned long long>(y._mp_size);
        sink += static_cast<unsigned long long>(x._mp_exp);
        sink += static_cast<unsigned long long>(y._mp_exp);
        sink ^= static_cast<unsigned long long>(x._mp_d[0]);
        sink += static_cast<unsigned long long>(y._mp_d[0]);
    }
    rdot_gmp_microbench::read_sink = sink;
}

int main(int argc, char **argv) {
    return rdot_gmp_microbench::run(argc, argv, "07_readonly", rdot_gmp_microbench::OutputMode::ReadSink, _Rdot_microbench);
}
