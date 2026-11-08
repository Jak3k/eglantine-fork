// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>

#include "shader_recompiler/frontend/ir/ir_emitter.h"
#include "shader_recompiler/frontend/ir/program.h"
#include "shader_recompiler/ir_opt/passes.h"

TEST_CASE("SSA register copies preserve a local definition", "[video_core][ssa]") {
    using namespace Shader;
    ObjectPool<IR::Inst> pool;
    IR::Block block{pool};
    IR::IREmitter ir{block};
    ir.SetReg(IR::Reg::R0, ir.Imm32(42U));
    for (size_t i = 0; i < 1000; ++i) {
        ir.SetReg(IR::Reg::R1, ir.GetReg(IR::Reg::R0));
        ir.SetReg(IR::Reg::R0, ir.GetReg(IR::Reg::R1));
    }
    const auto result = ir.GetReg(IR::Reg::R0);
    IR::Program program;
    program.blocks = {&block};
    program.post_order_blocks = {&block};
    Optimization::SsaRewritePass(program);
    REQUIRE(result.Resolve().IsImmediate());
    REQUIRE(result.Resolve().U32() == 42U);
}

TEST_CASE("SSA retains both incoming values through a copied merge", "[video_core][ssa]") {
    using namespace Shader;
    ObjectPool<IR::Inst> pool;
    IR::Block entry{pool}, left{pool}, right{pool}, merge{pool};
    entry.AddBranch(&left);
    entry.AddBranch(&right);
    left.AddBranch(&merge);
    right.AddBranch(&merge);
    IR::IREmitter left_ir{left}, right_ir{right}, merge_ir{merge};
    left_ir.SetReg(IR::Reg::R0, left_ir.Imm32(11U));
    right_ir.SetReg(IR::Reg::R0, right_ir.Imm32(29U));
    merge_ir.SetReg(IR::Reg::R1, merge_ir.GetReg(IR::Reg::R0));
    const auto result = merge_ir.GetReg(IR::Reg::R1);
    IR::Program program;
    program.blocks = {&entry, &left, &right, &merge};
    program.post_order_blocks = {&merge, &right, &left, &entry};
    Optimization::SsaRewritePass(program);
    const auto resolved = result.Resolve();
    REQUIRE_FALSE(resolved.IsImmediate());
    const auto* phi = resolved.Inst();
    REQUIRE(phi->GetOpcode() == IR::Opcode::Phi);
    REQUIRE(phi->NumArgs() == size_t{2});
    std::array<u32, 2> incoming{};
    for (size_t i = 0; i < incoming.size(); ++i) {
        REQUIRE(phi->Arg(i).Resolve().IsImmediate());
        incoming[i] = phi->Arg(i).Resolve().U32();
    }
    std::ranges::sort(incoming);
    REQUIRE(incoming == std::array<u32, 2>{11U, 29U});
}
