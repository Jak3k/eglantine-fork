# Zelda Dynamic FPS test

The performance-lab branch includes **Dynamic FPS by Dark (built-in)** for title 0400486024DEC000, main build E0F3CCFAF50065AE4EA01EB5AEB13B1A45DA3348. It can be disabled in the game's Add-Ons settings. This changes the simulation step below 60 FPS; it does not make the GPU render more frames.

Use CPU Clock **Normal** and **Sync Core Speed off**. This fork skips the patch and logs a warning with other clock settings, because they scale the timer the routine reads. Existing user settings are not changed automatically.

## Source and safeguards

Dark wrote the routine; Frizti integrated it with permission to bundle it (commit 765659af4ccb7201d1145c57f3b45ce84653cea0). The loader integration and regression tests are adapted from Lichii build-88 under GPL-3.0-or-later, with ctgl1987's delta-cap change from PR #584. Max Resolution is now included as a separate Add-On. The above-60-FPS unlock is not included.

- https://github.com/zortos293/Lichii/releases/tag/build-88
- https://github.com/zortos293/Lichii/pull/584

The hook at module offset 0x6E6CC4 requires the original instruction. The 120-byte routine at 0x3C46200 must fit zero-filled executable alignment padding. Its 16-byte timer state at 0x4E9FE00 must fit zero-filled BSS. Changes from user mods in either record or the timer state take precedence; validation completes before either record is written. Title, module, build, sizes and layout must match.

The routine caps elapsed time at 100 ms and smooths it. It retains the original step as a floor. Addressability checks cannot prove that the game never uses the timer-storage location later. Long gameplay, cinematics, audio synchronization and behavior below 10 FPS still need evaluation.

Production PatchManager tests cover the exact routine, enable/disable behavior, wrong title/module/build, truncation, original bytes, layout, timer state, user-mod precedence, and scaled-clock rejection. No game data is included in these synthetic tests.

Gameplay validation is pending; keep this a test build until matched scene and real-time movement comparisons pass.

Assisted by Codex (port, guards, tests and documentation).

## Max Resolution and texture upload experiment

Max Resolution is enabled by default for the same supported Zelda main build and can be disabled independently. It replaces the checked instruction at 0x805BD0, preserving user-mod precedence and bounds/build checks. It holds the game's internal resolution at full scale; this is an image-quality option and may increase GPU cost, not a promised FPS gain. Its application does not depend on the CPU clock.

The user's reference screenshot is from Lichii custom, showing both built-in Add-Ons and its 60 FPS mode. That label alone is not a measurement of simulation speed. Eglantine-Time retains its existing 30/60 frame-pacing choices.

Texture uploads also adopt tomk134/lichii-custom e8e50e7978d5f822f038af16901c4d95f8249915: contiguous GPU mappings are read directly by the texture deswizzler instead of copied to a temporary vector. Split or inaccessible mappings retain the copy path. EDEN_NX2_UNSWIZZLE_COPY=1 forces copying for comparisons. This targets texture-loading overhead across Zelda, Donkey Kong and Mario Kart; gameplay gains remain to be measured.
