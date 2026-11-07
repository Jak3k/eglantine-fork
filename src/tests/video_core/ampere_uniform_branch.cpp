// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "shader_recompiler/frontend/ampere/program_capture.h"

namespace {
using namespace Shader::Ampere;
constexpr Instruction Exit{{0x0000794d, 0, 0x03800000, 0}};

Instruction Branch(u32 condition, u32 guard = 7, s32 offset = 16) {
    const u64 relative = static_cast<u64>(static_cast<s64>(offset));
    return {{(condition << 24) | (guard << 12) | 0x547, (static_cast<u32>(relative) & ~3U) | 1U,
             0x0b800000U | static_cast<u32>((relative >> 32) & 0x3ffff), 0}};
}

struct BranchResult {
    size_t exit;
    u32 predicates;
};

// Execute the branch expansion's predicate moves and control flow, including taken restores.
BranchResult Run(const LoweredProgram& program, u32 predicates, u32 uniforms) {
    const auto address = [](size_t i) { return 0x50 + i * 8 + ((i + 1) / 3) * 8; };
    u32 saved{};
    size_t pc{};
    for (size_t step = 0; step < 32 && pc < program.instructions.size(); ++step) {
        const u64 word = program.instructions[pc];
        const u32 condition = static_cast<u32>((word >> 16) & 15);
        const u32 slot = condition & 7;
        const bool enabled = (slot == 7 || ((predicates >> slot) & 1)) != bool(condition & 8);
        if (!enabled) {
            ++pc;
            continue;
        }
        if ((word >> 48) == 0x38e8) {
            saved = predicates & static_cast<u32>((word >> 20) & 0x7f);
        } else if ((word >> 48) == 0x38f0) {
            const u32 source = static_cast<u32>((word >> 8) & 0xff);
            const u32 mask = static_cast<u32>((word >> 20) & 0x7f);
            predicates = (predicates & ~mask) | ((source == 239 ? uniforms : saved) & mask);
        } else if ((word >> 52) == 0xe24) {
            const s64 relative = static_cast<s64>(((word >> 20) & 0xffffff) << 40) >> 40;
            const s64 target = static_cast<s64>(address(pc)) + 8 + relative;
            // Local branches can land inside an expansion as well as at a native instruction.
            size_t next{};
            while (next < program.instructions.size() &&
                   static_cast<s64>(address(next)) != target) {
                ++next;
            }
            REQUIRE(next < program.instructions.size());
            pc = next;
            continue;
        } else if ((word >> 52) == 0xe30) {
            return {pc, predicates};
        } else {
            FAIL("Unexpected instruction in uniform branch expansion");
        }
        ++pc;
    }
    FAIL("Uniform branch expansion did not terminate");
    return {};
}
} // namespace

TEST_CASE("Ampere BRA.U preserves guest predicates on both paths", "[video_core][ampere]") {
    for (u32 uniform = 0; uniform < 7; ++uniform) {
        for (const u32 negate : {0U, 8U}) {
            for (const u32 guard : {7U, 0U, 8U, 6U, 14U}) {
                const auto branch = Branch(uniform | negate, guard);
                const auto program = LowerToMaxwell(std::array{branch, Exit, Exit});
                INFO(program.error);
                REQUIRE(static_cast<bool>(program));
                REQUIRE(program.source_instruction_offsets.size() == 3);
                for (const u32 predicates : {0U, 0x7fU, 0x15U, 0x2aU}) {
                    for (const u32 uniforms : {0U, 0x7fU, 0x15U, 0x2aU}) {
                        const bool guard_enabled =
                            ((guard & 7) == 7 || ((predicates >> (guard & 7)) & 1)) !=
                            bool(guard & 8);
                        const bool taken = guard_enabled &&
                                           (bool((uniforms >> (6 - uniform)) & 1) != bool(negate));
                        const auto result = Run(program, predicates, uniforms);
                        CHECK(result.exit == program.source_instruction_offsets[taken ? 2 : 1]);
                        CHECK(result.predicates == predicates);
                    }
                }
            }
        }
    }
}

TEST_CASE("Ampere captures forward uniform branches past an early EXIT", "[video_core][ampere]") {
    ProgramCaptureState capture;
    CHECK(capture.Observe(Branch(0), 0) == ProgramCaptureState::Result::Continue);
    CHECK(capture.Observe(Exit, 1) == ProgramCaptureState::Result::Continue);
    CHECK(capture.Observe(Exit, 2) == ProgramCaptureState::Result::Complete);
    ProgramCaptureState invalid;
    CHECK(invalid.Observe(Branch(0, 7, -32), 0) == ProgramCaptureState::Result::InvalidTarget);
    const auto backward =
        LowerToMaxwell(std::array{Instruction{{0x00007918, 0, 0, 0}}, Branch(0, 7, -32), Exit});
    REQUIRE(static_cast<bool>(backward));
}

TEST_CASE("Ampere BRA.U handles constant conditions and rejects invalid modes",
          "[video_core][ampere]") {
    for (const u32 condition : {7U, 15U}) {
        const auto lowered = LowerToMaxwell(std::array{Branch(condition), Exit, Exit});
        REQUIRE(static_cast<bool>(lowered));
        CHECK(std::count_if(lowered.instructions.begin(), lowered.instructions.end(), [](u64 word) {
                  return (word >> 52) == 0xe24;
              }) == (condition == 7 ? 1 : 0));
    }
    auto invalid = Branch(0);
    invalid.words[1] &= ~3U;
    CHECK_FALSE(LowerToMaxwell(std::array{invalid, Exit, Exit}));
    invalid = Branch(0);
    invalid.words[2] &= ~(1U << 27);
    CHECK_FALSE(LowerToMaxwell(std::array{invalid, Exit, Exit}));
}
