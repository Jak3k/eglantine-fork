// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Vulkan {

template <typename Read>
std::optional<std::vector<char>> ReadPipelineCacheSnapshot(Read&& read) {
    constexpr size_t MaxSnapshotBytes = size_t{512} << 20;
    for (size_t attempt = 0; attempt < 3; ++attempt) {
        size_t size{};
        if (read(&size, nullptr) != VK_SUCCESS || size > MaxSnapshotBytes) {
            return std::nullopt;
        }
        std::vector<char> data(size);
        if (size == 0) {
            return data;
        }
        const VkResult result = read(&size, data.data());
        if (result == VK_SUCCESS && size <= data.size()) {
            data.resize(size);
            return data;
        }
        if (result != VK_INCOMPLETE) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

} // namespace Vulkan
