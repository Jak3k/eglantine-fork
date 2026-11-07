# Eglantine-Time

Experimental performance fork based on Eglantine `nx2` commit
`ae9d3eb8d224e85b9fc4ec1df35f2825e13aef76`. The target is the program identified upstream as
Ocarina of Time, `0400486024DEC000`.

## Isolation

The Qt binary is `eglantine-time.exe`; the SDL binary is `eglantine-time-cli.exe`.
Internal CMake targets remain `yuzu`, `yuzu-cmd`, and `tests` so existing dependency and IDE
integration continues to work. The CMake project and application identity are Eglantine-Time.

Windows data and configuration live in `%APPDATA%\eglantine-time`. Portable builds use
`user-time` beside the executable. Linux data, configuration and caches use the
`eglantine-time` subdirectory of their respective XDG roots. Shader caches, logs, saves and
NX2 catalogue defaults are separate from stock Eglantine. Explicit user-selected paths can
still share data. The default log filename is `eglantine_time_log.txt`. No automatic migration of stock data is performed by these changes.

The upstream update checker is disabled by default. Keep it disabled: this fork has no
release feed. Existing `EDEN_NX2_*` environment switches remain supported for diagnostics.

## Changes and remaining work

| Area | Implementation and limits |
| --- | --- |
| Native shaders | Adds Ampere opcode `0x547` (`BRA.U`) using the public Mesa NAK encoding. Uniform conditions and execution predicates remain distinct; guest predicates are restored on both branch paths. Program capture follows uniform forward targets. Existing `ULDC.U16` support is retained and gains indexed halfword-offset coverage. |
| Temporal reconstruction | Checks render dimensions and finite, subpixel jitter/motion parameters before reconstruction. The temporal fallback discards history after a gap exceeding 250 ms. Existing DLSS/FSR jitter and motion-vector conventions are retained. These guards do not prove that any specific bloom, smearing or geometry defect is fixed. |
| GPU threading | Retains the existing back-thread command-list flush and ordered flush-region deferral. Async `SubmitList` already enqueues without a completion wait. Semaphore, CPU-read and ring-capacity waits are dependencies and remain in place. |
| Image memory | Requires device-local memory on the initial budget-enforced allocation. Host fallback retains driver budgets, selects non-device-local host-visible memory, and checks exact image requirements against a cap of the smaller of 8 GiB and one quarter of physical RAM, counting existing VMA host blocks. Oversized extent checks cannot wrap. Sparse fallback remains; exhaustion can still produce an allocation error or missing resources. This is not a cap on total emulator memory. |
| Pipeline cache | Joins builders and pending writers before teardown serialization. Snapshot reads retry `VK_INCOMPLETE`, bound host allocation, and preserve the prior cache when a snapshot or file write fails. Cached background prebuilding remains enabled upstream. |
| Shader options | Asynchronous compilation defaults on. The new aggressive toggle skips every not-yet-built graphics pipeline, including one-shot draws. It defaults off because skipped texture or post-processing draws can leave visual defects. First-use translation and compilation are not eliminated. |
| Pacing | Defaults to the existing Vulkan 60 FPS pacing mode. The OoT-only presentation option normalizes display intervals 2–4 to one and preserves emulator speed-control extensions. Other program IDs retain their presentation intervals. This does not unlock simulation code capped at 30 FPS, generate frames or guarantee 60 FPS performance. |
| Controller input | The existing frontend defaults to a Pro Controller and uses Nintendo A/B/X/Y button semantics. Guest prompt images are game assets. HID command 300 remains `ActivateConsoleSixAxisSensor`; buttons and controller style use Npad, including commands 100–104 and shared-memory input. No fabricated command-300 button ABI is introduced. |

The new options are persisted through the existing settings registry and exposed in the Qt
graphics settings. The recommended OoT preset also enables 60 FPS pacing and the OoT display
interval option, using session overrides that restore global preferences afterwards.

## Windows MSVC / Vulkan build

Use Visual Studio 2022 x64 C++ tools, CMake 3.31 or newer, Ninja, and the Vulkan SDK with
`glslangValidator` on PATH. See [the upstream build guide](Build.md) for Qt and other
dependencies. Run these commands from an x64 Native Tools shell in the repository:

```powershell
cmake -S . -B build/time-msvc -G Ninja `
  -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENGL=OFF `
  -DBUILD_TESTING=ON -DYUZU_TESTS=ON -DYUZU_CMD=ON `
  -DYUZU_ROOM=OFF -DENABLE_UPDATE_CHECKER=OFF
cmake --build build/time-msvc --target yuzu yuzu-cmd tests --parallel
ctest --test-dir build/time-msvc --output-on-failure
```

The Windows CI workflow now builds and runs the core tests before packaging the renamed
executables. That workflow must run on a Windows runner to establish MSVC compatibility;
a Linux test result is not a substitute.

## Validation and visual triage

Regression tests cover uniform branch execution and predicate restoration, native capture,
indexed `ULDC.U16`, allocation overflow/headroom, concurrent cache growth, invalid temporal
inputs, stale-history reset, presentation intervals, and preset restoration. This patch adds no game shader dumps, game data, keys or NVIDIA binaries.

No in-game performance or visual result is claimed. Validate Kokiri Forest to Hyrule Field,
combat first-use shaders, dynamic resolution, menu transitions and low-memory scenes on the
actual game build and host. Follow `.claude/skills/nx2-shader-triage/SKILL.md`: compare logs and
shader captures against the exact executable, validate emitted SPIR-V, and bound unattended
runs. The built-in Burnout bloom workaround is unrelated and is not enabled for OoT.

These changes were generated with Codex assistance. Review and direct human testing are
required by the repository's AI contribution policy before any upstream submission. See the
[validation report](Eglantine-Time-validation.md) for the completed checks and remaining limits.
