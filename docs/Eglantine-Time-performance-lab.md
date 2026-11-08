# Zelda performance experiments

Changes on `oot-performance-lab` are experiments. Windows and Linux CI compile, test and package artifacts; the owner subsequently requested downloadable Windows and Linux test releases; both workflows publish clearly marked prereleases after CI checks.

## First candidate

Port the contiguous-prefix virtual range cache reuse and its three regression tests from Lichii PR #580 (original work by mstampfli in #478), GPL-3.0-or-later:
https://github.com/zortos293/Lichii/pull/580

A shorter binding at the same GPU address can reuse a previously walked single contiguous range. Split mappings still require an exact-size walk, and unmapping any part of the retained range invalidates it. The only production caller ignores single-segment lists. This targets page-walk overhead observed in the local GPU back-end profile; no FPS improvement is claimed yet.

Revert the earlier MaxContinuousRange victim-cache experiment in this branch. Six five-second samples in the same Zelda house scene averaged 39.85 FPS enabled versus 40.84 disabled. These single runs do not establish a benefit.

## Next comparisons

Use bounded isolated runs with copied saves/configuration and warmed shaders. Compare the same scene and settings, alternating baseline/candidate order. Keep screenshots and raw logs local. Do not upload game content, saves, keys or profiles. Assess visual correctness and simulation speed as well as FPS. Investigate Lichii's optional dedicated GPU CPU sets and guarded Zelda dynamic-FPS support separately. Mario Kart World is secondary.

The r/emulation discussion supplied context but no reproducible settings benchmark:
https://www.reddit.com/r/emulation/comments/1xofuoa/zelda_ocarina_of_time_remake_reportedly_dumped/

Assisted by Codex (research, port, tests and CI preparation); owner authorized work on this fork.

## lichii-custom follow-up

Reviewed https://github.com/tomk134/lichii-custom at f5f6ced676d82ccff22340476947c5998d1eec23 (GPL-3.0-or-later). Ported its SSA rewrite optimization from ac4191c6a909aa80885803c785e2980fb961b926, credited there to monarchy/Lichii-performance: resolve register-copy identities, avoid allocation for local definitions and reuse traversal storage. Existing shader regression tests exercise the resulting compiler.

The fork's 5e24f7577b11eeca8d33d3ec829b85166757afa1 deliberately removes Zelda's 60 FPS pacing. We retain pacing: a dynamic-step floor can otherwise accelerate simulation above 60 FPS. Also reject the built-in Dynamic FPS patch under scaled CPU clocks. See Eglantine-Time-dynamic-fps.md for guards and limitations. Fake occlusion and skipped rendering are not enabled by this port.

Local first CPU-set experiment: same house scene 36.63 FPS control, 36.70 pinned; no convincing benefit, so no pinning defaults added. Normal-clock control averaged 37.52 FPS. These are single-run six-sample means, not statistical proof.

Mario Kart World: reproduced failure to initialize the nested mkw_zero_depth_v1 cache on a fresh title directory. Use recursive directory creation so pipeline caching can initialize. First texture-budget comparison (23.16 vs 29.17 FPS) is confounded by this cache issue; a warmed control is required before adopting a larger budget.

No shader catalogue entries were added: no new native programs were identified, both local catalogue directories are absent and EDEN_NX2_CATALOG_DIR is unset. Python aliases have no installed interpreter.

Assisted by Codex (research, adaptation, tests and packaging); publication requested by repository owner.

## Platform profiles and follow-up checks

The owner's NVIDIA Windows machine uses Fast GPU mode; their AMD Linux machine performs better with Accurate. The optional Zelda and Mario Kart presets now select those platform modes. Zelda's accepted session preset additionally selects CPU Clock Normal and Sync Core Speed off for Dynamic FPS. Declining preserves existing configuration, and session changes restore after stopping the game. The dialog describes the actual selected mode.

Added direct SSA regressions for long register-copy chains and a branch merge that must retain two distinct incoming values.

A later Mario Kart control with the cache directory present averaged 29.71 FPS, versus 29.17 with a larger texture budget. No budget expansion is adopted; the first 23.16 FPS control lacked functioning disk caching and is not a clean memory-budget comparison. The directory initialization fix is retained.

The contiguous-range-only Zelda build averaged 36.80 FPS at normal CPU clock, versus the earlier 37.52 control. This single comparison does not establish a steady-FPS benefit; gameplay-speed validation of the Dynamic FPS build is separate.
