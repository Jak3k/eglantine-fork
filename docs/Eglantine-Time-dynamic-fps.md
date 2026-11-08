# Zelda Dynamic FPS test

The performance-lab branch includes **Dynamic FPS by Dark (built-in)** for title 0400486024DEC000, main build E0F3CCFAF50065AE4EA01EB5AEB13B1A45DA3348. It can be disabled in the game's Add-Ons settings. This changes the simulation step below 60 FPS; it does not make the GPU render more frames.

Use CPU Clock **Normal** and **Sync Core Speed off**. This fork skips the patch and logs a warning with other clock settings, because they scale the timer the routine reads. Existing user settings are not changed automatically.

## Source and safeguards

Dark wrote the routine; Frizti integrated it with permission to bundle it (commit 765659af4ccb7201d1145c57f3b45ce84653cea0). The loader integration and regression tests are adapted from Lichii build-88 under GPL-3.0-or-later, with ctgl1987's delta-cap change from PR #584. This port includes neither Max Resolution nor the above-60-FPS unlock.

- https://github.com/zortos293/Lichii/releases/tag/build-88
- https://github.com/zortos293/Lichii/pull/584

The hook at module offset 0x6E6CC4 requires the original instruction. The 120-byte routine at 0x3C46200 must fit zero-filled executable alignment padding. Its 16-byte timer state at 0x4E9FE00 must fit zero-filled BSS. Changes from user mods in either record or the timer state take precedence; validation completes before either record is written. Title, module, build, sizes and layout must match.

The routine caps elapsed time at 100 ms and smooths it. It retains the original step as a floor. Addressability checks cannot prove that the game never uses the timer-storage location later. Long gameplay, cinematics, audio synchronization and behavior below 10 FPS still need evaluation.

Production PatchManager tests cover the exact routine, enable/disable behavior, wrong title/module/build, truncation, original bytes, layout, timer state, user-mod precedence, and scaled-clock rejection. No game data is included in these synthetic tests.

Gameplay validation is pending; keep this a test build until matched scene and real-time movement comparisons pass.

Assisted by Codex (port, guards, tests and documentation).
