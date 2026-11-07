// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "core/hle/service/nvnflinger/presentation_interval.h"

TEST_CASE("60 Hz presentation changes only display intervals", "[core][presentation]") {
    for (s32 interval = -4; interval <= 100; ++interval) {
        CHECK(Service::Nvnflinger::PresentationInterval(interval, false) == interval);
        CHECK(Service::Nvnflinger::PresentationInterval(interval, true) ==
              (interval > 1 && interval < 5 ? 1 : interval));
    }
}
