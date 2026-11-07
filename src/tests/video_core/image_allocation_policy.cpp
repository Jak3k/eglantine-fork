// SPDX-License-Identifier: GPL-3.0-or-later

#include <limits>
#include <catch2/catch_test_macros.hpp>
#include "video_core/vulkan_common/image_allocation_policy.h"

TEST_CASE("Large image extents cannot wrap past the allocation guard", "[video_core][memory]") {
    CHECK_FALSE(Vulkan::IsOversizedImage(1920, 1080, 1, 1));
    CHECK_FALSE(Vulkan::IsOversizedImage(16384, 16384, 1, 1));
    CHECK(Vulkan::IsOversizedImage(8192, 8192, 1, 68));
    CHECK(Vulkan::IsOversizedImage(65536, 65536, 65536, 65536));
    CHECK(Vulkan::IsOversizedImage(std::numeric_limits<u32>::max(), std::numeric_limits<u32>::max(),
                                   4, 4));
    CHECK(Vulkan::IsOversizedImage(0, 1080, 1, 1));
}

TEST_CASE("Host image fallback preserves RAM headroom", "[video_core][memory]") {
    constexpr u64 GiB = u64{1} << 30;
    CHECK(Vulkan::HostImageFallbackBudget(64 * GiB) == 8 * GiB);
    CHECK(Vulkan::HostImageFallbackBudget(8 * GiB) == 2 * GiB);
    CHECK(Vulkan::HostImageFallbackFits(GiB, GiB, 8 * GiB));
    CHECK_FALSE(Vulkan::HostImageFallbackFits(GiB, GiB + 1, 8 * GiB));
    CHECK_FALSE(Vulkan::HostImageFallbackFits(8 * GiB, 1, 64 * GiB));
    CHECK_FALSE(Vulkan::HostImageFallbackFits(0, 30 * GiB, 128 * GiB));
    CHECK_FALSE(Vulkan::HostImageFallbackFits(std::numeric_limits<u64>::max(), 1, 64 * GiB));
    CHECK_FALSE(Vulkan::HostImageFallbackFits(0, std::numeric_limits<u64>::max(), 64 * GiB));
    CHECK_FALSE(Vulkan::HostImageFallbackFits(0, 1, 0));
}
