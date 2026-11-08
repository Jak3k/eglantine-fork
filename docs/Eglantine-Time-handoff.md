# Continue Eglantine-Time on a Windows PC

## Goal and repository

Produce a tested Windows x64 / Vulkan ZIP containing `eglantine-time.exe`,
`eglantine-time-cli.exe`, and their runtime dependencies, then publish it to the
owner's fork, `https://github.com/Jak3k/eglantine-fork`, branch `eglantine-time`.

The repository contains the experimental OoT performance fork. Read
[the fork guide](Eglantine-Time.md) and [validation report](Eglantine-Time-validation.md)
for the implementation and its limits. Settings use `%APPDATA%\eglantine-time`;
portable mode uses `user-time` beside the executable.

## Current build status

- The full Linux core build passed: 255 test cases, 7,210,175 assertions.
- Windows build `42407168178` failed on an existing debug-memory test comparing
  `optional<u64>` with signed address literals. Those expected addresses now use `u64`.
- Windows build `42413432462` passed that correction, then found signed conditional
  expectations in three shader test files. This handoff includes unsigned literal
  fixes in `spirv_atomic_retry.cpp`, `spirv_fragment_shuffle.cpp`, and
  `ampere_depth_compare.cpp`. The affected Linux tests pass: 4 cases, 50,888 assertions.
- The Windows workflow now compiles test objects first, collects independent test
  compilation errors, and saves dependency/compiler caches after failure. Its cache
  identity is separate from MSVC's overwritten `PLATFORM` environment variable and
  reuses the saved `x64` cache.
- A working Windows executable has **not yet been verified**. Actual game visuals,
  NVIDIA DLSS runtime behavior, and 60 FPS performance remain untested.

Every source push to `eglantine-time` starts
[Windows EXE package](https://github.com/Jak3k/eglantine-fork/actions/workflows/windows-exe.yml).
Check its latest run; the two failed runs above describe earlier source revisions.

## Windows setup

Open this repository in Codex or the Codex extension in VS Code with a local
Windows execution environment. Install or locate Visual Studio 2022 C++ build
tools, the Windows SDK, CMake 3.31+, Ninja, Qt 6.8.3 for MSVC 2022 x64 with Qt Charts,
and the Vulkan SDK. Use an x64 Native Tools shell. The known CI configuration and
packaging commands are in `.github/workflows/build-windows.yml`; the basic build
commands are in [the fork guide](Eglantine-Time.md#windows-msvc--vulkan-build).

For installed Qt, configure `YUZU_USE_BUNDLED_QT=OFF` and point CMake at its actual
MSVC directory. Keep `ENABLE_OPENGL=OFF`, `BUILD_TESTING=ON`, `YUZU_TESTS=ON`,
`YUZU_CMD=ON`, `YUZU_ROOM=OFF`, `YUZU_ROOM_STANDALONE=OFF`,
`ENABLE_UPDATE_CHECKER=OFF`, and `USE_DISCORD_PRESENCE=OFF` to match CI.

## Finish the Windows package

1. Read the latest GitHub Actions result or build locally; fix actual compiler or
   test failures without disabling the checks. Run the core tests with CTest.
2. Run `windeployqt` against `eglantine-time.exe`, including compiler runtime files.
   Verify both x64 executables, Qt DLLs, and the Windows platform plugin are present.
3. Package the complete runtime directory as a ZIP, with license/source information
   and a SHA-256 checksum. Publish an experimental prerelease in the owner's fork,
   tied to the exact compiled source commit. The CI artifact alone expires after 7 days.
4. Update the validation report with actual Windows results. In-game rendering and
   FPS claims require a separate test on the real game and GPU.

Codex assisted with the implementation, build fixes, and this handoff. The owner
requested publication to this fork; no upstream pull request was opened.
