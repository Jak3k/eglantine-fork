// SPDX-License-Identifier: GPL-3.0-or-later

#include <memory>
#include <catch2/catch_test_macros.hpp>
#include "common/settings.h"
#include "video_core/oot_network.h"

TEST_CASE("OoT network bypass is opt-in and title-specific", "[video_core][oot]") {
    using namespace VideoCommon::OotNetwork;
    using Settings::OotAntiAliasing;
    REQUIRE_FALSE(ShouldBypass(ProgramId, OotAntiAliasing::Game));
    REQUIRE(ShouldBypass(ProgramId, OotAntiAliasing::NoSharpening));
    REQUIRE(ShouldBypass(ProgramId, OotAntiAliasing::Game, true));
    for (const u64 other : {u64{0}, u64{0x040074A01BF12000}, u64{0x0400C3F00006E000}}) {
        REQUIRE_FALSE(ShouldBypass(other, OotAntiAliasing::NoSharpening));
        REQUIRE_FALSE(ShouldBypass(other, OotAntiAliasing::Game, true));
    }
    BypassState state;
    REQUIRE(state.tonemapped_image.load() == u64{0});
    REQUIRE_FALSE(state.output_replaced.load());
}

TEST_CASE("Lichii graphics choices preserve per-game overrides", "[settings][oot]") {
    auto settings = std::make_unique<Settings::Values>();
    REQUIRE(settings->oot_anti_aliasing.GetValue() == Settings::OotAntiAliasing::Game);
    REQUIRE_FALSE(settings->game_dlss_network.GetValue());
    settings->oot_anti_aliasing.SetGlobal(false);
    settings->oot_anti_aliasing.SetValue(Settings::OotAntiAliasing::NoSharpening);
    settings->game_dlss_network.SetGlobal(false);
    settings->game_dlss_network.SetValue(true);
    REQUIRE(settings->oot_anti_aliasing.GetValue() == Settings::OotAntiAliasing::NoSharpening);
    REQUIRE(settings->game_dlss_network.GetValue());
    settings->oot_anti_aliasing.SetGlobal(true);
    settings->game_dlss_network.SetGlobal(true);
    REQUIRE(settings->oot_anti_aliasing.GetValue() == Settings::OotAntiAliasing::Game);
    REQUIRE_FALSE(settings->game_dlss_network.GetValue());
    settings->oot_anti_aliasing.SetGlobal(false);
    settings->game_dlss_network.SetGlobal(false);
    REQUIRE(settings->oot_anti_aliasing.GetValue() == Settings::OotAntiAliasing::NoSharpening);
    REQUIRE(settings->game_dlss_network.GetValue());
}
