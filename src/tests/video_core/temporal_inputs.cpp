// SPDX-License-Identifier: GPL-3.0-or-later

#include <limits>
#include <catch2/catch_test_macros.hpp>
#include "video_core/renderer_vulkan/temporal_inputs.h"

TEST_CASE("Temporal reconstruction rejects invalid camera data", "[video_core][temporal]") {
    using Vulkan::ValidTemporalInputs;
    CHECK(ValidTemporalInputs({1280, 720}, {-0.5f, 0.25f}, {1.0f, 1.0f}));
    CHECK_FALSE(ValidTemporalInputs({0, 720}, {0, 0}, {1, 1}));
    CHECK_FALSE(ValidTemporalInputs({1280, 720}, {0, 2}, {1, 1}));
    CHECK_FALSE(
        ValidTemporalInputs({1280, 720}, {std::numeric_limits<f32>::quiet_NaN(), 0}, {1, 1}));
    CHECK_FALSE(
        ValidTemporalInputs({1280, 720}, {0, 0}, {1, std::numeric_limits<f32>::infinity()}));
}

TEST_CASE("Temporal reconstruction discards history after a pause", "[video_core][temporal]") {
    using Clock = std::chrono::steady_clock;
    using namespace std::chrono_literals;
    const Clock::time_point now{1s};
    CHECK(Vulkan::ResetTemporalHistory(std::nullopt, now));
    CHECK_FALSE(Vulkan::ResetTemporalHistory(now - 16ms, now));
    CHECK_FALSE(Vulkan::ResetTemporalHistory(now - 250ms, now));
    CHECK(Vulkan::ResetTemporalHistory(now - 251ms, now));
    CHECK(Vulkan::ResetTemporalHistory(now + 1ms, now));
}
