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

#include <cassert>

int main()
{
    gmpxx::mpz_class za("10");
    gmpxx::mpz_class zb("20");
    gmpxx::mpz_class zc("30");
    gmpxx::mpz_class zdst;

    zdst = za + zb;
    assert(zdst == 30);

    zdst = za + zb + zc;
    assert(zdst == 60);

    za += zb;
    assert(za == 30);

    za += zb + zc;
    assert(za == 80);

    gmpxx::mpq_class qa("1/3");
    gmpxx::mpq_class qb("2/5");
    gmpxx::mpq_class qdst;

    qdst = qa * qb;
    assert(qdst == gmpxx::mpq_class("2/15"));

    qdst += 1;
    assert(qdst == gmpxx::mpq_class("17/15"));

    qdst = qa + 2;
    assert(qdst == gmpxx::mpq_class("7/3"));

    return 0;
}
