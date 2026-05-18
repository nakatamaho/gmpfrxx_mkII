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

#include <gmpfrxx_mkII/detail/expr.hpp>

#include <cstdlib>
#include <type_traits>

namespace {

struct move_tracking_leaf {
    using result_type = int;

    move_tracking_leaf() = default;

    move_tracking_leaf(const move_tracking_leaf&)
    {
        ++copy_count;
    }

    move_tracking_leaf(move_tracking_leaf&&) noexcept
    {
        ++move_count;
    }

    move_tracking_leaf& operator=(const move_tracking_leaf&) = delete;
    move_tracking_leaf& operator=(move_tracking_leaf&&) = delete;

    static void reset() noexcept
    {
        copy_count = 0;
        move_count = 0;
    }

    static int copy_count;
    static int move_count;
};

int move_tracking_leaf::copy_count = 0;
int move_tracking_leaf::move_count = 0;

void test_rvalue_unary_negation_folding_moves_child()
{
    using namespace gmpfrxx_mkII::detail;

    unary_expr<neg_op, move_tracking_leaf, int> source(move_tracking_leaf{});
    move_tracking_leaf::reset();

    auto folded = -std::move(source);
    static_assert(std::is_same_v<decltype(folded)::op_type, pos_op>);

    if (move_tracking_leaf::copy_count != 0 || move_tracking_leaf::move_count == 0) {
        std::abort();
    }
}

void test_rvalue_unary_positive_folding_moves_child()
{
    using namespace gmpfrxx_mkII::detail;

    unary_expr<pos_op, move_tracking_leaf, int> source(move_tracking_leaf{});
    move_tracking_leaf::reset();

    auto folded = -std::move(source);
    static_assert(std::is_same_v<decltype(folded)::op_type, neg_op>);

    if (move_tracking_leaf::copy_count != 0 || move_tracking_leaf::move_count == 0) {
        std::abort();
    }
}

} // namespace

int main()
{
    test_rvalue_unary_negation_folding_moves_child();
    test_rvalue_unary_positive_folding_moves_child();
    return 0;
}
