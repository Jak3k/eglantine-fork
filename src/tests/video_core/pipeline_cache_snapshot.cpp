// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <limits>
#include <catch2/catch_test_macros.hpp>
#include "video_core/renderer_vulkan/pipeline_cache_snapshot.h"

TEST_CASE("Pipeline cache snapshots retry concurrent growth", "[video_core][pipeline_cache]") {
    size_t calls{};
    const auto snapshot = Vulkan::ReadPipelineCacheSnapshot([&](size_t* size, void* data) {
        ++calls;
        if (!data) {
            *size = calls < 3 ? 4 : 8;
            return VK_SUCCESS;
        }
        if (calls == 2) {
            return VK_INCOMPLETE;
        }
        auto* bytes = static_cast<char*>(data);
        std::fill_n(bytes, *size, 'x');
        return VK_SUCCESS;
    });
    REQUIRE(snapshot);
    CHECK(snapshot->size() == 8);
    CHECK(snapshot->front() == 'x');
    CHECK(calls == 4);
}

TEST_CASE("Pipeline cache snapshots bound allocation and retry failures",
          "[video_core][pipeline_cache]") {
    CHECK_FALSE(
        Vulkan::ReadPipelineCacheSnapshot([](size_t*, void*) { return VK_ERROR_DEVICE_LOST; }));
    CHECK_FALSE(Vulkan::ReadPipelineCacheSnapshot([](size_t* size, void*) {
        *size = std::numeric_limits<size_t>::max();
        return VK_SUCCESS;
    }));
    size_t calls{};
    CHECK_FALSE(Vulkan::ReadPipelineCacheSnapshot([&](size_t* size, void* data) {
        ++calls;
        *size = 4;
        return data ? VK_INCOMPLETE : VK_SUCCESS;
    }));
    CHECK(calls == 6);
    const auto empty = Vulkan::ReadPipelineCacheSnapshot([](size_t* size, void*) {
        *size = 0;
        return VK_SUCCESS;
    });
    REQUIRE(empty);
    CHECK(empty->empty());
}
