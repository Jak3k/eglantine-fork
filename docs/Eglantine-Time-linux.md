# Building Eglantine-Time on Linux

The `eglantine-time` branch supports Linux and Windows from the same source. The GPU range-cache
change uses standard C++ and keeps its address-space and mapping-generation validation on both.

## Build recipe

The Linux package workflow targets **Ubuntu 24.04, x86-64, GCC and Qt 6.8.3**. Its build and
test results are published in GitHub Actions under **Linux x64 package**. A successful run
publishes the GUI, CLI, matching source ZIP and SHA-256 checksums as a prerelease.

Install build dependencies:

```sh
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
  autoconf cmake g++ gcc git glslang-tools libglu1-mesa-dev \
  libpulse-dev libtool libudev-dev libxkbcommon-x11-0 \
  mesa-common-dev nasm ninja-build qt6-base-private-dev \
  qt6-charts-dev qt6-tools-dev libavfilter-dev libavcodec-dev \
  libswscale-dev pkg-config zlib1g-dev libva-dev libvdpau-dev \
  spirv-tools spirv-headers libusb-1.0-0-dev \
  libboost-dev libboost-fiber-dev libboost-context-dev \
  libopus-dev libasound2-dev ccache libvulkan1 mesa-vulkan-drivers \
  libxcb-cursor0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
  libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-xinerama0 libxcb-xkb1
```

Install **Qt 6.8.3 for Linux desktop GCC**, including **Qt Charts**, using the Qt installer
or aqtinstall. The workflow uses `jurplel/install-qt-action` to install that exact version.
Set `QT_ROOT_DIR` to the installation containing `bin/qmake`, for example:

```sh
export QT_ROOT_DIR="$HOME/Qt/6.8.3/gcc_64"
git clone --branch eglantine-time https://github.com/Jak3k/eglantine-fork.git
cd eglantine-fork
cmake -S . -B build/linux -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DYUZU_TESTS=ON \
  -DYUZU_CMD=ON -DYUZU_ROOM=OFF -DYUZU_ROOM_STANDALONE=OFF \
  -DYUZU_USE_BUNDLED_QT=OFF -DCMAKE_PREFIX_PATH="$QT_ROOT_DIR" \
  -DYUZU_USE_BUNDLED_SDL3=ON -DVulkanHeaders_FORCE_BUNDLED=ON \
  -DVulkanUtilityLibraries_FORCE_BUNDLED=ON \
  -DENABLE_UPDATE_CHECKER=OFF -DUSE_DISCORD_PRESENCE=OFF \
  -DUSE_CCACHE=ON -DNIGHTLY_BUILD=OFF
cmake --build build/linux --target yuzu yuzu-cmd tests --parallel 2
ctest --test-dir build/linux --output-on-failure
```

Increase parallelism if sufficient RAM is available. Configuration downloads the dependency
versions recorded in `cpmfile.json`; it needs network access. Access to the private repository
requires your own GitHub authentication.

Run `build/linux/bin/eglantine-time` for the GUI, or
`build/linux/bin/eglantine-time-cli --help` for CLI options. Install a Vulkan driver for your GPU.
If Qt is outside the system library search path, set
`LD_LIBRARY_PATH="$QT_ROOT_DIR/lib:${LD_LIBRARY_PATH:-}"` when launching.

## Linux release package

Extract the tarball with `tar -xzf eglantine-time-linux-x64-<commit>.tar.gz`. It preserves the
executable permissions. This package contains dynamically linked executables, licenses and source
information; it needs Qt 6.8.3 and the system libraries above. It is **not a self-contained
AppImage**. Binaries built on Ubuntu 24.04 require a compatible system (glibc 2.39 or newer).
Build from source for another distribution or architecture.

CI runs the core/shader tests, CLI help, a bounded offscreen GUI startup and checks for missing
shared libraries before publication. These checks do not establish Linux game compatibility or
FPS gains. Windows gameplay measurements must not be presented as Linux measurements.

## Comparing the GPU range cache

`EDEN_NX2_RANGE_CACHE_VICTIMS=0` disables the new eight-entry victim cache for an A/B comparison;
the default enables it. `EDEN_NX2_RANGE_CACHE_STATS=1` logs cumulative direct hits, victim hits
and misses every 1,048,576 lookups. Counters are off by default. Compare the same saved scene,
settings and warmed shader cache, and measure FPS without the sampling profiler running.

AI assistance: Codex prepared the workflow, documentation and cache change at the repository
owner's request.
