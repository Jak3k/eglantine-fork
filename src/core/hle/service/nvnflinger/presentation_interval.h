// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/common_types.h"

namespace Service::Nvnflinger {

inline s32 PresentationInterval(s32 interval, bool force_60hz) {
    // Values outside 1..4 are emulator speed-control extensions, not display intervals.
    return force_60hz && interval > 1 && interval < 5 ? 1 : interval;
}

} // namespace Service::Nvnflinger
