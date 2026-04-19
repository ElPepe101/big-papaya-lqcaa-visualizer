## Why

The **sphere** meter was the right stepping stone for **PBR**, **HDRI**, and **16-channel** blending, but the product direction is now a **flat**, **infinite-feeling** surface with **perspective** so depth and displacement read more naturally, while keeping the same **bread** look and **IBL**. A plane also gives a straightforward **(u,v)** domain for a **blended audio field** that deforms the **entire** visible sheet instead of wrapping meridian bands.

## What Changes

- **Replace** the primary **3D meter primitive** from a **tessellated sphere** to a **high-resolution tessellated plane** (or equivalent heightfield mesh) with **square** UV layout suitable for **bread** maps.
- **Perspective** projection for the main 3D view (**BREAKING** vs. current orthographic sphere orbit contract in `sphere-3d-channel-display`).
- **Navigation**: keep **pointer-drag orbit** semantics where practical (fixed world **+Y** up, no roll), but **anchor** and **clamps** target a **plane-centered** scene (not sphere poles).
- **Channel mapping**: drive displacement (and shading) from a **smooth blended field** over **(u,v)** derived from the **16** smoothed RMS values—**no** hard **4×4** cell walls; neighboring channel influence **overlaps** (implementation-defined kernels or weights, documented in design).
- **Infinite look**: horizon / extent SHALL feel **unbounded** via **staged** fog, **tiling**, **sky**, and/or **large finite extent** with **edge hiding** (exact technique in design—not a literal infinite simulation requirement).
- **Retain** **bread-like PBR** (albedo, normal, roughness, AO, displacement map), **HDRI** IBL + sky, **opaque** metering (no alpha levels), and **vertex-shader-only** geometric displacement for **audio + bread displacement** (no FS parallax/ray-march for those).
- **Audio pipeline unchanged** (same **16** smoothed values; no capture/device/monitor changes).

## Capabilities

### New Capabilities

- `plane-3d-channel-display`: **Primary** 3D meter as **tessellated plane**, **perspective** camera, **blended** **16-channel** displacement field over UV, **infinite-looking** staging, **bread PBR** + **HDRI**, **VS-only** displacement, **opaque** level read, metering **source** unchanged.

### Modified Capabilities

- `sphere-3d-channel-display`: **Supersede** sphere as the **primary** 3D meter; **remove** sphere-specific obligations (geometry, orthographic orbit-at-sphere-center, sphere UV band rules) that conflict with the **plane** capability. Shared ideas (PBR, HDRI, VS displacement discipline, hue/level encoding) **migrate** to `plane-3d-channel-display`.

## Impact

- **`src/`**: `ofApp` draw/camera path; replace or sideline **`SphereMeterView`** with a **plane** module (new files or renamed API); **shaders** (new `bread_plane` or generalized names).
- **`bin/data/`**: reuse **`bread/`** and **`env.hdr`** where possible; optional new assets for **horizon/fog** only if needed.
- **`openspec/specs/`**: new **`plane-3d-channel-display/spec.md`** at archive time; **`sphere-3d-channel-display`** trimmed or retired via delta.
- **User-facing**: **BREAKING**—sphere + ortho + UV unfold behavior **removed** or **replaced** by plane-appropriate debug (e.g. full-frame UV / grid overlay).
