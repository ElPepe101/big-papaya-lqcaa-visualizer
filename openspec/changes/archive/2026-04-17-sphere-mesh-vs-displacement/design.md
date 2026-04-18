## Context

The LQCAA bread sphere (`SphereMeterView`, `bread_sphere.vert`/`.frag`) uses a **square UV** mesh: **N×N** quads with **N % 16 == 0**, **16** latitude bands for channel logic, **vertex colors** carrying channel index and meter column parameter, and **uniforms** for per-channel `litEnd` and bread **`displacement.png`**. Displacement is applied along the **geometric sphere normal** in the **vertex shader**; the fragment shader handles PBR shading only.

Constraints: **OpenGL 3.2 Core** on macOS (no tessellation / geometry pipeline in scope), **oF** mesh upload from CPU.

## Goals / Non-Goals

**Goals:**

- Increase **usable mesh density** (raise default **N** or make **N** easy to tune) so **VS displacement** (bread + audio) reads **smoother** on silhouette and grazing angles.
- Keep **all displacement** (height from `displacement.png` and audio-driven offset) in **`bread_sphere.vert`** only — **no** FS parallax, **no** tessellation shader in this change.
- Preserve **square UV** (equal **du** and **dv** in **[0,1]²**) so **4096²** textures are not stretched in **u** vs **v**.
- Document **vertex/triangle budget** vs **N** so builders can pick a tier.

**Non-Goals:**

- **Parallax occlusion mapping** or **relief mapping** in the fragment shader.
- **GPU tessellation** or **Metal** port.
- **Whole-sphere RBF** blending of 16 channels (separate exploration / change).
- Changing **audio capture** or **meter smoothing** formulas beyond what the VS already consumes.

## Decisions

1. **Single knob `N` (`kUvGridRes`)**  
   **Rationale:** One constant drives **azimuth quads** and **latitude strips** (`kLatSub = N/16`); keeps UV square and code simple.  
   **Alternatives:** Separate `Nu`, `Nv` — rejected for this change (reintroduces elongation unless carefully coupled).

2. **Default `N` in a “high but sane” tier (e.g. 1536 or 2048)** after profiling, with **1024** as fallback constant if builds OOM  
   **Rationale:** User asked for **more mesh**; default must not brick low-RAM machines. Exact number chosen at apply time with a **one-line** constant and a **log line** for verts.  
   **Alternatives:** Runtime LOD slider — deferred.

3. **VS displacement formula unchanged in spirit**  
   Audio: `uDispAmp * pow(fill, …)` on radial normal; bread: `(texture(displacement).r - 0.5) * 2 * uDispTexScale`.  
   **Rationale:** Proven path; more verts improve **sampling** of the same height field between vertices.

4. **Optional `uDispTexScale` / `uDispAmp` retune** after mesh bump  
   **Rationale:** Finer mesh can use **slightly lower** per-vertex displacement amplitude to avoid **self-shadow** or **normal map** mismatch looking worse; tune empirically.

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| **O(N²) memory** for duplicated 4-vert quads | Document **N** vs **vert count** (`4N²`); cap **N**; consider **indexed mesh** in a **future** change if **N > 2048** is required |
| **Slower upload / draw** each frame | Mesh is static after `setup`; only uniforms animate — acceptable |
| **VS displacement vs normal map** mismatch at large amplitudes | Keep amplitudes moderate; note in spec that **geometric** normal is used in VS, **tangent** normal in FS (existing limitation) |
| **Unfold panel** performance with more grid lines | Optional: thin major lines only when **N > 512** |

## Migration Plan

1. Land constant / log changes behind this change.  
2. Run app, confirm **frame time** and **memory**; adjust **N** default in code or document recommended value for target hardware.  
3. No data migration; **shaders** remain path `bin/data/shaders/bread_sphere`.

## Open Questions

- Should **`N`** be exposed via **`ofXml`** / settings file without recompile? (Out of scope unless requested.)

**Resolved (apply):** Default **`N = 1536`** (divisible by 16, ~9.4M vertices with duplicated quads) as a step up from 1024 without jumping to 2048; **`uDispAmp` / `uDispTexScale`** nudged down slightly for finer tessellation.
