## Context

`SphereMeterView` draws a **4×4** logical grid on a sphere with **per-patch** HSB color. This change moves to **denser tessellation**, **interpolated** channel weights for **gradient** boundaries, **PBR** shading with **bread** textures, and **HDRI**-driven lighting.

## Goals / Non-Goals

**Goals:**

- **Mesh**: Parameteric sphere with **configurable** rings × segments (e.g. **≥** 32×64 or tuned for 60fps), still partitioned into **16** spherical **domains** (same row×col mapping as current code). **Vertices** inside each domain sample that channel’s level; **vertices on shared edges** blend **adjacent** channel levels (bilinear-style in spherical coordinates or equivalent) so **gradients** appear between regions.
- **Level distribution**: Map each channel’s normalized level across its domain so **variation** reads smoothly (e.g. radial or parametric ramp within patch — **implementation detail**, but **no** hard single color per coarse quad).
- **PBR**: Use **metallic-roughness**-style inputs (albedo + normal + roughness + AO/metallic as available) from **bread**-themed maps; fragment shader combines **base bread look** with **level-driven** modulation (e.g. roughness, emissive, or mix — **one clear scheme** in code comments).
- **HDRI**: Load **equirectangular** `.hdr` / `.exr` (or OF-supported env format) from `bin/data/`, build **cubemap** or use OF helpers for **IBL** (diffuse + specular approximation acceptable for v1); draw **sky** from same env or **tinted** clear when HDR background disabled for performance.

**Non-Goals:**

- Offline path tracing, **full** Disney BRDF, or **commercial** asset licensing in-repo (use **CC0** / **self-made** placeholders if needed).
- Changing **audio** device selection, **RMS** math, or **stereo** monitor.

## Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Renderer | **`ofShader`** + `ofMesh` with tangents (or normal mapping from MikkTSpace-style approx) | PBR needs normals; OF core supports shaders. |
| HDRI | **Equirect → cubemap** in setup or pre-baked cubemap `.dds` | Balance quality vs setup time; document fallback **gradient** sky if load fails. |
| Bread assets | **`bin/data/bread/`** — `albedo`, `normal`, `roughness` (`.png`/`.jpg`); optional `ao` | Keeps paths predictable; LQCAA ships **placeholders** if files missing (procedural noise + tan tint). |
| Level → shading | **Roughness** + slight **albedo** lift vs **emissive** | Reads “louder = glossier / hotter crust” without breaking PBR energy |

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Shader compile failures on older GL | Test on target macOS; `#version 150` or OF defaults. |
| Large HDRIs | Downscale or use **smaller** env for v1. |
| Frame time | Cap mesh; profile `update()` mesh rebuild vs shader uniforms only. |

## Migration Plan

1. Add assets + shaders; keep **ortho orbit** path.
2. Fallback: if HDRI missing, use **directional + ambient** and log once.
3. Remove or bypass **legacy flat HSB** fill path once PBR path validates.

## Open Questions

- Exact **CC0** HDRI filename to commit vs **user-supplied** only — **design**: ship **optional** placeholder note in README (user didn’t ask for README — keep in design/tasks only).
