## 1. Remove bread / plane meter hot path

- [x] 1.1 Remove or bypass `PlaneMeterView`, `bread_plane` shaders, and HDRI loading from the default 3D meter path; keep build green.
- [x] 1.2 Clear `ofApp` draw/setup to stop referencing bread assets for the primary view (background may stay solid color).

## 2. World-fixed wall mesh

- [x] 2.1 Add a wall meter module (e.g. `WallGridMeterView`) building a vertical **16-row × M-column** grid in world space; document axis convention and row↔channel order.
- [x] 2.2 Log mesh parameters at setup: row count, column count, vertex/triangle counts.

## 3. Shaders: noise VS + grayscale FS

- [x] 3.1 Add `wall_grid.vert` / `wall_grid.frag` (or equivalent): VS displacement **along wall normal** from smooth noise **f(time, uv)** with **per-channel phase** uniform(s); FS grayscale from channel levels.
- [x] 3.2 Ensure FS does not apply parallax/height ray-march for metering geometry.

## 4. Dual darkness mapping options

- [x] 4.1 Implement **spatial** toggle: per-cell (or per-vertex) luma vs row-uniform luma within each channel band.
- [x] 4.2 Implement **encoding** toggle: linear RMS-based luma vs dB-scaled mapping aligned with existing meter helpers where practical.
- [x] 4.3 Wire runtime controls (e.g. keys) to cycle modes; document bindings in code comment or help string.

## 5. Camera and polish

- [x] 5.1 Tune `OrbitOrthoCamera` (or successor) defaults so the world-fixed wall reads well at startup (distance, clip planes, FOV).
- [x] 5.2 Optional: faint grid line visualization (wireframe overlay or FS line pattern) if spec readability needs it.

## 6. Verification

- [x] 6.1 `make` Release; run without `bread/` and without HDRI: wall meter still renders.
- [x] 6.2 Confirm 16ch input drives row bands; no alpha-based metering.
- [x] 6.3 `openspec validate world-wall-noise-grid-meter`; prepare `/opsx-archive` with spec sync for `world-wall-noise-grid-display` and `plane-3d-channel-display`.
