# Eglantine-Time validation

Base: `Djihads80/Eglantine`, branch `nx2`, commit `ae9d3eb8d224e85b9fc4ec1df35f2825e13aef76`.

- Focused C++ regression suite: **67 test cases, 4,409 assertions passed** with GCC 14.2, Catch2 3.8.1 and fmt 11.2. This includes the new tests and existing standalone Ampere and preset tests. Backend-dependent Ampere depth-compare and numeric tests are included only in the full repository suite.
- Production syntax checks: allocator, hardware composer, Vulkan pipeline cache and Vulkan texture cache passed using the configured project compiler flags and locally extracted X11 SDK headers.
- Changed-line formatting and new C++ file formatting passed with clang-format 23.1.5. `git diff --check` passed.
- Repository ad-hoc C++ lint checks pass. The pre-existing missing header guard is fixed, and the lint script now declares the Bash interpreter required by its arrays.
- Modified Python helpers compile; release-notes shell script passes Bash syntax checking.
- All 13 changed XML/UI/plist/manifest files parse; all three build workflow YAML files parse.
- Patch applies cleanly to an archived clean base; reverse application also checks cleanly against the working tree.
- Full Linux core build and test run: **passed: 255 test cases, 7,210,175 assertions**. CTest passed in 9.14 seconds.
- Windows MSVC / Vulkan build: **not yet passing**. The first two GitHub Actions runs exposed signed/unsigned comparisons in existing core and shader tests. Those comparisons are corrected; the Windows rebuild still needs to pass compilation, CTest and runtime packaging. See [the Windows handoff](Eglantine-Time-handoff.md) for exact status and continuation steps.
- Latest shader-test corrections pass locally: **4 test cases, 50,888 assertions**. NVIDIA DLSS runtime and actual game performance/visual validation remain **not run**; no game/runtime capture is available in this Linux environment.

The Linux headless configuration uses Vulkan, bundled SDL/FFmpeg, tests enabled, and Qt/OpenGL/audio-device/web/libusb frontends disabled. CMake 4.4.4, Ninja 1.13.2 and glslang 16.6.0 are installed in workspace directories. The bundled Linux FFmpeg archive required explicit linkage to the available system VA-API, DRM and X11 libraries. Build and dependency caches are excluded from the source archive.

See [the fork guide](Eglantine-Time.md) for Windows build commands, settings, architectural findings, and limits. No 60 FPS performance result or confirmed bloom/lighting fix is claimed.

Codex assisted with implementation and validation. The repository owner explicitly requested publication to `Jak3k/eglantine-fork`.
