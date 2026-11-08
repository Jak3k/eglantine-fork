# Zelda performance experiments

Changes on `oot-performance-lab` are experiments. Windows and Linux CI compile, test and package artifacts; automatic release publication is restricted to `eglantine-time`.

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
