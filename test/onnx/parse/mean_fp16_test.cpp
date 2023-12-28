/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <onnx_test.hpp>

TEST_CASE(mean_test)
{
    const std::size_t num_data = 3;
    migraphx::program p;
    auto* mm = p.get_main_module();
    migraphx::shape s{migraphx::shape::half_type, {1, 2, 3}};
    auto data0   = mm->add_parameter("0", s);
    auto data1   = mm->add_parameter("1", s);
    auto data2   = mm->add_parameter("2", s);
    auto div_lit = mm->add_literal(migraphx::literal{migraphx::shape{s.type()}, {num_data}});
    auto divisor =
        mm->add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", s.lens()}}), div_lit);
    auto mean = mm->add_instruction(migraphx::make_op("div"), data0, divisor);
    divisor =
        mm->add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", s.lens()}}), div_lit);
    data1 = mm->add_instruction(migraphx::make_op("div"), data1, divisor);
    mean  = mm->add_instruction(migraphx::make_op("add"), mean, data1);
    divisor =
        mm->add_instruction(migraphx::make_op("multibroadcast", {{"out_lens", s.lens()}}), div_lit);
    data2 = mm->add_instruction(migraphx::make_op("div"), data2, divisor);
    mean  = mm->add_instruction(migraphx::make_op("add"), mean, data2);

    auto prog = optimize_onnx("mean_fp16_test.onnx");

    EXPECT(p == prog);
}
