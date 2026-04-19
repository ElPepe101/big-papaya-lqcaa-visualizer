## Why

The **bread PBR** ground **plane** (`plane-3d-channel-display`) served exploration of **HDRI**, **displacement**, and **blended** channel fields, but the desired direction is a **minimal**, **grid-forward** meter: a **world-fixed wall**, **row-aligned** channel bands, **procedural noise** motion along the **surface normal**, and **grayscale** level read—without **bread textures**, **PBR** maps, or **environment** as the core story.

## What Changes

- **Remove** the **bread** visualizer from the **primary** 3D path: **no** **`bread/`** texture set, **no** **HDRI/IBL** as **required** for the main meter (**BREAKING** for anyone relying on bread+env as the default look).
- **Replace** the **infinite ground plane** meter with a **vertical** **world-fixed** **wall** mesh in **3D**, with a **dense grid** (**rows × columns**, **rows** aligned to **16** **channel** **bands** and **columns** providing **finer** horizontal subdivision).
- **Vertex shader only** for **geometric** offset: each vertex (or row-consistent rule) moves **along** the **wall outward normal** by a **smooth noise** function of time and position, with a **distinct phase** (or seed) **per channel** so rows **don’t** move identically.
- **Fragment** (or vertex color) **grayscale**: **darkness** / **lightness** maps **smoothed per-channel levels**; **ship two** **user-selectable** **darkness modes** in one milestone (**e.g.** **per-cell** vs **row-uniform** luma, and/or **linear** vs **dB-scaled** mapping—exact pair documented in **design**).
- **Keep** the **16-channel** **smoothed RMS** consumption contract and **unchanged** **audio capture** / **device** / **stereo monitor** behavior.
- **Camera**: **world-fixed** wall implies the wall stays in **world space**; the user may still **orbit** to **inspect** (wall not billboard-locked to view).

## Capabilities

### New Capabilities

- `world-wall-noise-grid-display`: **World-fixed** **vertical** **wall** grid, **16** **row** **bands** **mapped** to **channels**, **finer** **column** tessellation, **VS-only** **normal-direction** **noise** **displacement** with **per-channel** **phase**, **grayscale** **level** **shading** with **two** **darkness** **mapping** **options**, **opaque** **surfaces**, **audio** **pipeline** **unchanged** for **I/O**.

### Modified Capabilities

- `plane-3d-channel-display`: **Supersede** the **bread**/**HDRI**/**ground-plane** **primary** meter; **remove** **requirements** that **mandate** that **visual** (migrate **normative** **behavior** to **`world-wall-noise-grid-display`**).

## Impact

- **`src/`**: Replace or **retire** **`PlaneMeterView`** / **`bread_plane`** shaders; new **wall** mesh + **noise** shaders (GLSL **150**), **`ofApp`** draw path, optional **debug** **toggles** for darkness modes.
- **`bin/data/`**: **`bread/`** and **`env.hdr`** **no longer** **required** for default run (may **remain** on disk **unused** until cleanup).
- **Specs**: New **`openspec/specs/world-wall-noise-grid-display/spec.md`** at **archive**; **`plane-3d-channel-display`** **trimmed** to **pointer** / **history** **pattern** (same **style** as **`sphere-3d-channel-display`** after prior **migration**).
