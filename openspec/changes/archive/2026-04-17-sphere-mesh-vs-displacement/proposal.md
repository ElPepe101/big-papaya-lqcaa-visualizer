## Why

The bread PBR sphere uses a **square UV grid** whose resolution is capped for CPU memory and upload cost; increasing **geometric density** improves **silhouette** and **vertex-displacement** fidelity for the crust map and audio-driven offset, without introducing **fragment-only** displacement in this change. The user wants **more mesh** while keeping displacement **strictly in the vertex shader** (portable on OpenGL 3.2 / macOS).

## What Changes

- **Raise or parameterize** the sphere tessellation (same square **N×N** UV topology, **N** divisible by 16) with documented **default**, **maximum**, and **memory / frame-time** guidance.
- **Reaffirm and document** that **surface offset** from `displacement.png` and from **per-channel meter** values is applied **only in the vertex shader** (no tessellation stage, no parallax / FS displacement in scope of this change).
- **Optional**: startup or debug log of **vertex / triangle counts** and **N** to verify mesh tier.
- **Unfold / debug UV** panel remains consistent with the same **N** (no behavior change beyond matching grid).

## Capabilities

### New Capabilities

- _(none)_

### Modified Capabilities

- `sphere-3d-channel-display`: Add requirements for **minimum mesh density tier**, **vertex-shader-only displacement** for bread and audio, and **operational limits** (configurable **N** or equivalent) so full-resolution textures can be sampled without requiring **one texel per CPU quad**.

## Impact

- **`src/SphereMeterView.cpp`**, **`bin/data/shaders/bread_sphere.vert`** (and possibly `.frag` only if uniforms need threading): mesh build constants, optional compile-time or documented constant for **N**.
- **Performance**: higher **N** increases vertex count **O(N²)**; may require lowering **N** on integrated GPUs or adding a future LOD change.
- **OpenSpec**: delta spec under this change for `sphere-3d-channel-display`.
