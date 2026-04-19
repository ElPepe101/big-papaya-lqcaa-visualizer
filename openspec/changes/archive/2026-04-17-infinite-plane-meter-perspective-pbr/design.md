## Context

**LQCAA** today renders **`drawSphereMeterPBR`** with a **dense square UV sphere**, **`bread_sphere`** shaders, **`ofCubeMap`** HDRI path, **orthographic** `orbitCam_`, and **16** `uLitEnd` scalars driving **band-limited** displacement and tint. The user wants a **product pivot**: same **audio + art** direction, but a **flat** primary surface, **perspective**, **infinite** feel, and a **blended** influence of all **16** channels over the **whole** visible plane.

## Goals / Non-Goals

**Goals:**

- **Plane meter** as the **only** primary 3D visualization (remove sphere from the hot path unless kept behind a compile-time or debug flag—default off).
- **Perspective** camera with **stable** feel: **+Y** up, **no roll**, drag to **orbit / inspect** around a **sensible anchor** (e.g. origin on the plane or a small offset above it).
- **Blended field**: each frame, map **16** RMS-derived values to a **scalar field** \(f(u,v)\) (and possibly **vector** tangential motion later) with **wide** smooth kernels so **every** vertex is influenced by **multiple** channels; avoid visible **4×4** borders.
- **High tessellation** on the plane (configurable **N×N** quads, **N** divisible by **16** for implementation symmetry) with **VS-only** offsets: **bread `displacement.png`** + **audio field** along **geometric normal** (plane **+Y** if using **XZ** ground). **Shipped default:** **`N = 1024`** (`PlaneMeterView.cpp` **`kUvGridRes`**), **half-extent 2800** world units, **fog** `exp(-uFogDensity * dist² * 1e-6)` with **`kFogDensity = 0.22`**.
- **PBR + HDRI**: reuse **`bin/data/bread/*`** and **`env.hdr`** pipeline patterns from the sphere; **fallbacks** if assets missing.
- **Infinite-looking**: combine **large** finite mesh extent, **distance fog** / **aerial perspective**, and **sky** so the plane **meets** the horizon **without** a hard “card” edge at normal viewing angles; optional **subtle** UV **repeat** on **albedo** only if art allows (document choice—avoid obvious tiling on **displacement** if it creates repeating bumps).

**Non-Goals:**

- **Compute shader** or **tessellation shader** **required** path (optional future); first milestone stays **GL 3.2**-friendly **vertex** displacement.
- **Multiplayer / network**; **audio engine** changes.
- **Parallax occlusion** or **FS ray-marched** height for **audio** (still **VS-only** for metering geometry).

## Decisions

1. **Plane orientation and UV**  
   - **Decision**: Use an **XZ** “ground” plane (**Y** is **normal** for **+** displacement), **UV** = **(x,z)** mapped to **[0,1]** on the **finite** mesh patch, **world** placement so the camera looks **downward** at a shallow angle (classic **stage**).  
   - **Alternatives**: **XY** “wall” plane (normal **+Z**)—better for frontal **UI** but weaker “infinite ground” read; rejected for this change’s **infinite** goal.

2. **Blended field from 16 channels**  
   - **Decision**: In VS (or shared **uniform** setup), build **per-channel weights** \(w_c(u,v)\) as **normalized** products of **smoothstep** **ramps** along **u** and **v** (soft **4×4** partition) **then** add a **small global bias** from **all** channels (e.g. mean or **RMS** of levels) so **quiet** channels still **nudge** the whole surface. **Blend** **displacement amplitude** \(A(u,v) = \sum_c w_c(u,v) \cdot \text{lit}_c\) with **smooth** \(w_c\).  
   - **Alternatives**: **GPU blur** of a **16-tap** **RT** each frame—nicer spreads, more code; **defer** unless first pass looks too **cellular**.

3. **Perspective vs ortho**  
   - **Decision**: **Perspective** `ofCamera` with **reasonable FOV** (e.g. **45–60°**) and **near/far** tuned to **mesh + fog**.  
   - **Alternatives**: **Hybrid** ortho base + perspective **post**—unnecessary complexity.

4. **Infinite illusion**  
   - **Decision**: **Fog** uniform(s) in fragment shader keyed on **distance** from camera (or **height**), **skybox** drawn first; plane **extent** large enough that **edges** sit outside typical **frustum** at default camera; if **edges** visible when orbiting wide, **fade** plane to **horizon color** at mesh **periphery** (alpha-to-coverage **not** used for **metering**—use **color** blend to **fog**, surface stays **opaque**).

5. **Code structure**  
   - **Decision**: New module **`PlaneMeterView`** (or rename namespace to **`MeterSurfaceView`** with plane impl) + **`bread_plane.vert/frag`**; **`ofApp`** calls **`setupPlaneMeterView`** / **`drawPlaneMeterPBR`**; remove **`drawSphereMeterSky`** coupling if sky stays shared—extract **`drawMeterEnvironment`** if useful.  
   - **Alternatives**: Parameterize one **shader** with **primitive** uniform—**harder** to read; keep **separate** shaders.

6. **Debug**  
   - **Decision**: Replace **UV unfold** panel with **optional** **overlay**: **grid**, **channel weight heatmap**, or **split** **albedo** debug—pick **one** minimal option in implementation (**tasks**).

## Risks / Trade-offs

- **[Risk]** **4096²** textures on a **large** plane **stretch** if UV spans whole mesh—**Mitigation**: keep **UV** on a **standardized** patch; **world scale** independent; document **meters per UV unit** in code.  
- **[Risk]** **Z-fighting** on very **flat** angles—**Mitigation**: **polygon offset** or **slight** camera **minimum height**.  
- **[Risk]** **Performance** with **high N**—**Mitigation**: same as sphere: **log N**, **cap** if needed, **default** **N ≥ 512** with path to **1024+** if GPU allows.  
- **[Risk]** **Spec sync** removes **sphere** requirements—**Mitigation**: **archive** this change only after **plane** spec is **complete** and **app** matches.

## Migration Plan

1. Land **plane** path behind feature-complete **draw** (single codepath swap in **`ofApp`**).  
2. Remove **sphere** **draw** calls and **unused** assets/shaders only when **plane** meets **tasks** checklist.  
3. **Rollback**: git revert; keep **archive** branch if needed.

## Open Questions

- Default **camera** **distance** and **plane half-extent** (world units) for **“infinite”** without **huge** **far** plane **Z-fighting**.  
- Whether **channel hue** identity remains **per-pixel tint** like the sphere or shifts to **rim light** / **emissive** **only** on the plane.  
- **macOS** **GL 3.2**: confirm **no** features beyond **150** shaders.
