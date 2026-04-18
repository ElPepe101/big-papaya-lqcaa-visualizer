## Context

LQCAA computes **16** smoothed RMS values and currently draws **`RadialOrbView`** as a **2D opacity donut**. This change replaces that with a **3D sphere** and **orthographic orbit** camera.

## Goals / Non-Goals

**Goals:**

- **Sphere**: Unit or fixed-radius sphere centered at origin; **4 meridian steps × 4 parallel steps** → **16** quadrilateral patches on the surface (topology: globe-like grid excluding degenerate poles; use **uniform θ** in **[ε, π−ε]** and **uniform φ** in **[0, 2π)** or equivalent so each patch is a **curved quad** with two corners shared).
- **Channels**: One-to-one mapping **channel index 0…15** → patch index **(row, col)** with **documented order** (e.g. **latitude band from north to south**, **longitude west-to-east** within row).
- **Camera**: **Orthographic** projection; **target** = **origin** (sphere center); **distance** chosen so sphere fits viewport; **classic orbit** with **world +Y up**; mouse drag **yaw** (around **Y**) and **pitch** (elevation) with **clamp** (e.g. ±85°) to avoid flipping.
- **Color**: Per-channel **fixed hue** (spread hues for 16 channels, **distinct / vibrant**), **fixed saturation** (high); **lightness** = **f(level)** with **level ∈ [0,1]** from existing RMS mapping (e.g. same `sqrt(ofMap(...))` as today). **RGB output opaque** (alpha **255**); **no** level-driven alpha.

**Non-Goals:**

- PBR lighting, shadows, environment maps.
- Stereo VR or touch gestures.
- Changing **audio** device logic or RMS math (only visualization).

## Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Camera API | **`ofCamera`** + manual `begin()`/`end()` or **`ofEasyCam`** with ortho enabled | OF 0.12 supports both; verify ortho + target behavior. |
| Patch geometry | **Parametric sphere** → 4×4 **quads** as **triangle pairs** or **`ofMesh`** | Clear mapping; can refine later. |
| Hue table | **16** precomputed `ofColor` bases or **H** = `i * (360/16)` with jitter | “Vibrant” = high **S**; avoid identical adjacent hues where possible. |
| Level → L | **Linear** map **0…1** → **L** in **0.15…0.95** (or similar) to avoid pure black/white on sphere | Tunable constants in one place. |

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Ortho + orbit feel | User testing; adjust zoom / clip planes. |
| Pole quads | Use **epsilon** polar bounds so patches stay well-formed. |
| Z-fighting | Slight polygon offset or consistent draw order |

## Migration Plan

1. Implement 3D view behind same `meterDisplay` array.
2. Remove 2D `drawRadialOrb` from default draw path.
3. `make` and manual QA with Ableton → BlackHole.

## Open Questions

- Exact **patch ↔ channel** labeling on screen (optional numeric overlay) — **v2**.
