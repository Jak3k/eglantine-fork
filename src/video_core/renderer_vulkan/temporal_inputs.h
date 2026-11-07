// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <array>
#include <chrono>
#include <cmath>
#include <optional>
#include "common/common_types.h"

namespace Vulkan {

inline bool ValidTemporalInputs(std::array<u32, 2> render_size, std::array<f32, 2> jitter,
                                std::array<f32, 2> motion_scale) {
    for (size_t i = 0; i < 2; ++i) {
        if (render_size[i] == 0 || !std::isfinite(jitter[i]) || std::abs(jitter[i]) > 1.0f ||
            !std::isfinite(motion_scale[i])) {
            return false;
        }
    }
    return true;
}

inline bool ResetTemporalHistory(std::optional<std::chrono::steady_clock::time_point> last,
                                 std::chrono::steady_clock::time_point now) {
    return !last || now < *last || now - *last > std::chrono::milliseconds{250};
}

} // namespace Vulkan
