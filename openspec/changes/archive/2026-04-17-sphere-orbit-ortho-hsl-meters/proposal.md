## Why

The current **2D donut** view is a strong baseline but does not support **spatial inspection** of 16 channels as a single surface. Moving to a **3D sphere** with **orbit navigation** and **orthographic projection** gives a clear, diagram-like read while keeping a fixed world-up. Encoding level as **lightness** on **fixed vibrant hues per channel** avoids transparency artifacts in 3D and makes each channel instantly recognizable.

## What Changes

- Replace the **2D radial orb** visualization with a **3D sphere** tessellated into a **4×4 grid** of **surface patches** (meridian × parallel divisions → **16** patches, one per input channel).
- Add **classic orbit** camera interaction: **fixed +Y up**, drag to **yaw** around vertical and **pitch** with limits; **look-at** remains the **sphere center** (no drifting anchor).
- Use **orthographic** projection for the main 3D view (adjustable viewport scale in implementation).
- Map each channel’s smoothed level to **lightness** in **HSL** (or equivalent) in **[0, 1]**; **no alpha blending** for level (opaque fills). Assign a **distinct vibrant hue** per channel (fixed **H** and **S**, **L** from level).
- **BREAKING (visual)**: The **donut / annulus / opacity** visualization requirements are **removed**; the app’s primary meter view becomes the **3D sphere** per the new spec.

## Capabilities

### New Capabilities

- `sphere-3d-channel-display`: 3D sphere patch grid, orthographic camera, fixed-up orbit, HSL lightness mapping, per-channel hues.

### Modified Capabilities

- `radial-orb-channel-display`: **Superseded** — remove 2D donut and opacity-only requirements; migration to `sphere-3d-channel-display`.

## Impact

- **`src/`**: New or replaced view module (e.g. retire `RadialOrbView` 2D path); add camera + 3D draw path (`ofCamera` / `ofEasyCam` or equivalent), meshes or drawn quads on sphere, depth testing.
- **Audio / RMS**: Unchanged pipeline; same 16 smoothed levels fed into visualization.
- **UX**: Mouse-driven navigation; optional on-screen hint for controls.
