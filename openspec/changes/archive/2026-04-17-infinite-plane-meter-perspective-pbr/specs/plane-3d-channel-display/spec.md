## ADDED Requirements

### Requirement: plane-3d-channel-display scope

The **primary** three-dimensional meter view SHALL cover **16** input channels on a **tessellated plane** with **infinite-looking** horizon staging, **perspective** camera, **blended** channel weighting over **(u,v)**, **bread-like PBR**, **HDRI** IBL (with fallback), **opaque** surfaces, and **vertex-shader-only** geometric displacement from **audio** and **`displacement.png`**, consuming the same per-channel smoothed values as the existing metering path.

#### Scenario: Audio contract unchanged for visualization

- **WHEN** the application runs the **plane** meter
- **THEN** level values SHALL come from the **existing** **16-channel** smoothing path **without** altering **capture**, **device selection**, or **stereo monitor** behavior

### Requirement: Tessellated plane meter primitive

The system SHALL render the **16** channel levels on a **single** **tessellated plane** mesh in **three dimensions** (a **connected** heightfield over a **finite** domain in world space). The mesh SHALL use **sufficient subdivision** (**N×N** quads expressed as triangles, **N** a **positive integer** divisible by **16**) so silhouettes and **displacement** read **smoothly** at intended camera distances. **Texture coordinates** on the plane SHALL map **contiguously** across the mesh such that **bread** maps tile **without anisotropic stretch** relative to the authored **square** textures (implementation SHALL document the **UV ↔ world scale** convention).

#### Scenario: Startup reports mesh tier

- **WHEN** the plane meter finishes one-time mesh setup
- **THEN** the application SHALL emit a **log line** (or equivalent) that includes **N** and **approximate vertex count** (or triangle count)

### Requirement: Blended multi-channel displacement field

The system SHALL derive a **scalar influence field** over the plane parameterization from the **16** per-channel smoothed levels such that **each** interior point **(u,v)** receives **contributions** from **more than one** channel (smooth weights), **not** a hard **4×4** partition with **discontinuous** boundaries. **Adjacent** channel influences SHALL **overlap** so displacement and shading **transition smoothly** across the surface.

#### Scenario: No hard cell walls

- **WHEN** the meter is displaying multiple **active** channels at **similar** levels
- **THEN** the user SHALL **not** perceive **sharp rectangular** channel **cell** borders as the **dominant** structure; **gradients** and **blends** SHALL dominate at **default** tuning

### Requirement: Infinite-looking staging

The system SHALL present the plane as **visually open** and **horizon-bounded**: **sky** or **environment** fills the upper hemisphere, and the plane **meets** the **distant** view with **fog**, **color** **haze**, and/or **mesh extent** such that **typical** orbit positions do **not** reveal an obvious **finite card edge** as the primary artifact. **Exact** algorithm (fog formula, mesh size) is **implementation-defined** but SHALL be **documented** in **design** or code comments.

#### Scenario: Environment dominates far field

- **WHEN** the user orbits to a **normal** viewing angle at **default** camera settings
- **THEN** the **distant** plane region SHALL **read** as **atmospheric** continuation toward the **horizon**, **not** as a **sharp** **polygon** boundary against a **void**

### Requirement: Perspective camera with fixed-up orbit

The system SHALL use a **perspective** projection for the main **3D meter** view. The user SHALL **navigate** with **pointer drag** using **orbit-like** behavior: **world +Y** remains **up** (**no roll**); **horizontal** drag **yaws** the view around **+Y** through a **stable anchor**; **vertical** drag adjusts **pitch** with **clamped** angles to avoid **gimbal** **flip** past usable extremes. The **look-at target** SHALL remain a **documented** fixed point (e.g. **origin** or **plane center**).

#### Scenario: Anchor stability

- **WHEN** the user drags to orbit
- **THEN** the camera SHALL **remain** **anchored** relative to the **documented** target and SHALL **not** **drift** the scene **origin** unintentionally

### Requirement: Image-based lighting from an HDRI environment

The system SHALL use an **HDR** environment map (e.g. **equirectangular** `.hdr` / `.exr` or **cubemap** derived from it) from application data for **IBL** on the **plane** and **consistent** ambient/specular response. The implementation MAY draw the environment as **background** **sky**. If loading fails, the system SHALL fall back to a **documented** non-HDR path **without** crashing.

#### Scenario: Environment contributes to shading

- **WHEN** the **3D meter** view is active and environment assets load successfully
- **THEN** the shaded **bread-like** plane SHALL show **lighting** **consistent** with the **environment**, **not** only a **single** directional term

### Requirement: Bread-like PBR surface

The plane SHALL be shaded with a **physically inspired** workflow using **texture maps** (at minimum **albedo**, **normal**, **roughness**, and **AO** where available) consistent with the project’s **bread** art set. **Per-channel level** SHALL modulate **material** and/or **displacement** in a **defined** way while surfaces remain **opaque** (**level** SHALL NOT be expressed as **transparency** for metering).

#### Scenario: Textures drive appearance

- **WHEN** bread texture maps are present under configured paths
- **THEN** the surface SHALL show **normal-mapped** detail and **roughness** variation, **not** a **flat** untextured lambertian fill

### Requirement: Vertex-shader-only geometric displacement

All **geometric** offsets from **`displacement.png`** and from the **audio-driven blended field** SHALL be applied **only** in the **vertex shader** along the **surface normal**. The **fragment shader** SHALL NOT use **`displacement.png`** to **offset** **view** or **surface** position for **audio** metering (**no** **parallax** / **relief** **ray-march** for those effects).

#### Scenario: Fragment shader discipline

- **WHEN** inspecting the plane **fragment** shader for this feature set
- **THEN** there SHALL be **no** **displacement** texture sample whose purpose is **audio**-driven **geometry** in the **FS**

### Requirement: Level encoded without transparency

For each channel **c**, the system SHALL retain a **distinct** **hue** or **art** identity. The **smoothed level** SHALL modulate **perceived intensity** through **PBR** parameters and/or **tints** in addition to **bread** maps. **Level** SHALL NOT use **varying alpha** for metering.

#### Scenario: Quiet vs loud

- **WHEN** one channel is **quiet** and another is **loud**
- **THEN** regions **dominated** by each influence SHALL remain **distinguishable** via **shading** and **material** response, **not** via **transparency**

### Requirement: Audio pipeline unchanged

The visualization SHALL consume the same **16** per-channel **smoothed RMS-derived** values from the existing metering path. This capability SHALL NOT alter **audio capture**, **device selection**, or **stereo monitor** mixing.

#### Scenario: Metering source unchanged

- **WHEN** audio is running and levels update
- **THEN** the values driving the plane meter SHALL be derived from the **same** smoothing pipeline as before this capability
