# Lichii graphics controls in Eglantine-Time

Adapted from [tomk134/lichii-custom](https://github.com/tomk134/lichii-custom/tree/f5f6ced676d82ccff22340476947c5998d1eec23), under GPL-3.0-or-later.

Graphics settings now include **Use the game's own DLSS** and **Ocarina of Time anti-aliasing: Game / No sharpening**, globally and per game. Restart the game after changing them.

Game is the default. No sharpening bypasses OoT's spatial anti-aliasing network, replacing its output with the tonemapped image. This changes image quality; it can expose aliasing and banding. It is not an FPS unlock or a game-speed adjustment. Max Resolution remains an independent built-in add-on and may increase GPU work.

The Vulkan rasterizer recognizes upstream's three known tonemap shader hashes. After compute pipeline lookup, it checks the native program key on both cached and new dispatches. Only the known prepass/network/output passes are candidates. The first two are skipped only after an output copy has succeeded. Unknown images or failed copies restore the network on the next frame. Other titles and renderers are unaffected. Constants target OoT build E0F3CCFAF50065AE4EA01EB5AEB13B1A45DA3348; updates need revalidation.

The game-DLSS checkbox controls the existing native-network health checks and fallback. It defaults off, matching Lichii. NVIDIA DLSS, unsupported devices, resolution scaling and the existing EDEN_NX2_DLSS_REPLACEMENT environment override take precedence. This does not add DLSS support to titles whose networks are not supported.

Windows Fast and Linux Accurate optional presets remain unchanged. Compilation, unit tests and gameplay measurements for this revision must be reported separately; upstream performance numbers are not measurements of this fork.
