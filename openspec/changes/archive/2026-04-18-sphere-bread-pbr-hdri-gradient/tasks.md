## 1. Mesh and gradients

- [x] 1.1 Increase sphere tessellation (rings × segments) in `SphereMeterView` (or successor); keep **16** logical domains and document **channel → domain** mapping.
- [x] 1.2 Interpolate **adjacent channel levels** at domain boundaries (per-vertex or per-fragment weights) so regions **blend** in a **gradient-like** way; avoid flat single-color quads for the whole domain.

## 2. Shaders and PBR bread

- [x] 2.1 Add GLSL shaders (vertex/fragment) for **normal-mapped** shading with **roughness** (and optional **metallic/AO**) from `bin/data/bread/` or documented filenames.
- [x] 2.2 Load bread textures with **fallback** when files are missing; combine **base bread** look with **level-driven** modulation (constants documented in code).

## 3. HDRI environment

- [x] 3.1 Load an HDRI (or cubemap) from `bin/data/` for **IBL**; optional **sky** background; **fallback** lighting if load fails (log once).

## 4. Integration and cleanup

- [x] 4.1 Wire `ofApp` / draw path: depth, camera unchanged in behavior; remove or bypass obsolete **flat HSB-only** path once PBR path works.
- [x] 4.2 `make` succeeds; smoke-test orbit + audio-driven level response + visual sanity (bread detail + env lighting).
