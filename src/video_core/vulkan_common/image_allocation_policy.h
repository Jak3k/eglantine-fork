// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <array>
#include "common/common_types.h"

namespace Vulkan {

inline bool IsOversizedImage(u32 width, u32 height, u32 depth, u32 layers) {
    constexpr u64 MaxTexels = u64{1} << 28;
    u64 texels = 1;
    for (const u32 extent : std::array{width, height, depth, layers}) {
        if (extent == 0 || extent > MaxTexels / texels) {
            return true;
        }
        texels *= extent;
    }
    return false;
}

inline u64 HostImageFallbackBudget(u64 physical_memory) {
    return (std::min)(u64{8} << 30, physical_memory / 4);
}

inline bool HostImageFallbackFits(u64 used, u64 requested, u64 physical_memory) {
    const u64 budget = HostImageFallbackBudget(physical_memory);
    return requested != 0 && used <= budget && requested <= budget - used;
}

} // namespace Vulkan
