# sphere-3d-channel-display Specification

## Purpose

Three-dimensional meter view: **16** input channels on a **single sphere** with **high tessellation**, **bread-like PBR** materials (albedo, normal, roughness, and related maps), **image-based lighting** from an HDRI (with fallback), **orthographic** camera with fixed-up orbit, and **opaque** surfaces. Per-channel level modulates the surface through **shading / displacement / material** as implemented, not through transparency.

## Requirements

### Requirement: Image-based lighting from an HDRI environment

The system SHALL use a **high-dynamic-range** environment map (e.g. **equirectangular** `.hdr` / `.exr` or a **cubemap** derived from it) loaded from application data to provide **image-based lighting** for the meter sphere and **consistent** ambient/specular illumination. The implementation MAY draw the environment as a **background** (sky) or only use it for lighting; if loading fails, the system SHALL fall back to a **documented** non-HDR lighting path without crashing.

#### Scenario: Environment contributes to shading

- **WHEN** the 3D meter view is active and environment assets load successfully
- **THEN** the shaded bread-like sphere SHALL exhibit lighting that is **consistent** with the environment (e.g. highlights and bounce approximated via IBL), not only a single directional term

### Requirement: Bread-like PBR surface

The sphere SHALL be shaded with a **physically inspired** workflow using **texture maps** (at minimum **albedo** and **normal**, and **roughness** or equivalent packed maps) that evoke **bread**: **crusty / dimpled / soft** variation as art allows. **Per-channel level** SHALL modulate the material in a **defined** way (e.g. roughness, mix, displacement, or emissive gain) while preserving **opaque** surfaces (level SHALL NOT be expressed as **transparency** for metering).

#### Scenario: Textures drive appearance

- **WHEN** bread texture maps are present under the configured data paths
- **THEN** the sphere surface SHALL show **normal-mapped** detail and **roughness** variation characteristic of the maps, not a single flat color

#### Scenario: Missing texture fallback

- **WHEN** bread texture files are absent or invalid
- **THEN** the application SHALL still run and SHALL apply a **simple** fallback material (e.g. procedural tint) and MAY log a warning

### Requirement: Three-dimensional sphere with sixteen surface patches

The system SHALL render the 16 channel levels on a **single sphere** in **three dimensions**. The sphere SHALL use **sufficient tessellation** (many small faces) so the geometry appears **smooth** rather than faceted at the silhouette. The surface SHALL still be partitioned into **16** contiguous **logical regions** in **spherical coordinates** (meridian × azimuth bands) with a **fixed, stable mapping** from **channel index** 0–15 to those regions. **Adjacent regions** SHALL **blend** channel levels at shared boundaries so that shading and color **transition smoothly** (a **gradient-like** appearance) rather than only uniform color per coarse quad.

#### Scenario: Sixteen regions with smooth blending

- **WHEN** the application window is showing the 3D meter view
- **THEN** the user SHALL perceive **sixteen** channel-associated zones on the sphere with **smooth** transitions between neighboring zones and a **smooth** underlying mesh

### Requirement: Square UV sphere mesh density

The system SHALL tessellate the bread meter sphere with a **square grid** of **N×N** quadrilaterals (expressed as triangles in the mesh), where **N** is a **positive integer** divisible by **16** so that **16** channel latitude bands each contain an **equal** integer count of latitude subdivisions. Texture coordinates **u** and **v** on each quad corner SHALL span **contiguous** ranges in **[0, 1]** such that each quad occupies approximately a **1/N × 1/N** portion of UV space (no **independent** mismatch between **u** and **v** scale that would **elongate** square **4096×4096** bread maps).

#### Scenario: UV unwrap matches mesh columns and rows

- **WHEN** the UV unwrap debug view is enabled (if present in the application)
- **THEN** the overlay grid SHALL show **N** divisions along **u** and **N** divisions along **v** consistent with the mesh parameter **N**

### Requirement: Minimum default mesh tier

The implementation SHALL choose a **default** value of **N** that is **not less than 1024**, unless a **documented** platform or performance constraint forces a lower default; in that case, the **documented** default and **rationale** SHALL appear in change **tasks** or **design** notes.

#### Scenario: Startup reports mesh size

- **WHEN** the sphere meter view finishes one-time mesh setup
- **THEN** the application SHALL emit a **log line** (or equivalent) that includes **N** and **approximate vertex count** so operators can verify the active mesh tier

### Requirement: Vertex-shader-only displacement

All **radial** (or normal-aligned) **geometric** offsets driven by **`displacement.png`** and by **per-channel meter** values for the bread sphere SHALL be computed **only** in the **vertex shader**. The **fragment shader** SHALL NOT apply **parallax**, **relief**, or **height-based ray-march** displacement for those effects in the scope of this capability.

#### Scenario: Fragment shader has no displacement sampling for audio

- **WHEN** inspecting the bread sphere **fragment** shader source for this feature set
- **THEN** there SHALL be **no** texture sample of **`displacement.png`** used to **offset** view or surface position for **audio** metering (audio-driven shape remains **VS-only**)

#### Scenario: Bread crust height remains VS-sampled

- **WHEN** the bread **displacement** map is loaded
- **THEN** the **vertex shader** SHALL sample that map (or a bound equivalent) to offset vertices along the **outward** normal, and the **fragment shader** MAY continue to use **normal**, **albedo**, **roughness**, and **AO** maps for shading without displacing geometry in the FS

### Requirement: Orthographic camera with fixed-up classic orbit

The system SHALL use an **orthographic** projection for the main 3D view (allowing scale / viewport to be implemented as a camera parameter). The user SHALL **navigate** with a **pointer drag** using **classic orbit** behavior: **world vertical axis** (+Y) remains **up** (no roll); horizontal drag rotates the **view** around **that vertical axis** through the sphere center; vertical drag adjusts **elevation** (pitch) with **clamped** angles so the view does not flip past the poles. The **look-at target** SHALL remain the **sphere center** so the orbit does not **lose** the anchor point.

#### Scenario: orbit keeps center fixed

- **WHEN** the user drags to orbit the camera
- **THEN** the camera SHALL continue to face the sphere center as the orbit target and SHALL not translate the sphere center away from the anchor

### Requirement: Level encoded as lightness on fixed per-channel hues

For each channel **c**, the system SHALL retain a **distinct** **hue identity** (or equivalent art direction) per channel. The **smoothed level** SHALL modulate **perceived intensity** through **PBR parameters** and/or **HSB**-derived tints **in addition to** bread **albedo/normal/roughness** maps, such that **quiet** channels read **dimmer or rougher** and **loud** channels read **brighter or glossier** as defined in implementation. **Level SHALL NOT** be represented by **varying alpha** for metering; **fills remain opaque**.

#### Scenario: Quiet vs loud channel

- **WHEN** one channel’s smoothed level is near zero and another channel’s level is high
- **THEN** the corresponding regions SHALL remain **visually distinguishable** and SHALL reflect level through **shading** and **material** response, not transparency

### Requirement: Audio pipeline unchanged

The visualization SHALL consume the same **16** per-channel smoothed RMS-derived values produced by the existing metering path; this change SHALL NOT alter **audio capture**, **device selection**, or **stereo monitor** mixing behavior.

#### Scenario: Metering source unchanged

- **WHEN** audio is running and levels update
- **THEN** the values driving patch lightness SHALL be derived from the same per-channel smoothing pipeline as before this change
