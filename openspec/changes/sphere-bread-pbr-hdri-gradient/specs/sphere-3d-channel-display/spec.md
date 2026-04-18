## ADDED Requirements

### Requirement: Image-based lighting from an HDRI environment

The system SHALL use a **high-dynamic-range** environment map (e.g. **equirectangular** `.hdr` / `.exr` or a **cubemap** derived from it) loaded from application data to provide **image-based lighting** for the meter sphere and **consistent** ambient/specular illumination. The implementation MAY draw the environment as a **background** (sky) or only use it for lighting; if loading fails, the system SHALL fall back to a **documented** non-HDR lighting path without crashing.

#### Scenario: Environment contributes to shading

- **WHEN** the 3D meter view is active and environment assets load successfully
- **THEN** the shaded bread-like sphere SHALL exhibit lighting that is **consistent** with the environment (e.g. highlights and bounce approximated via IBL), not only a single directional term

### Requirement: Bread-like PBR surface

The sphere SHALL be shaded with a **physically inspired** workflow using **texture maps** (at minimum **albedo** and **normal**, and **roughness** or equivalent packed maps) that evoke **bread**: **crusty / dimpled / soft** variation as art allows. **Per-channel level** SHALL modulate the material in a **defined** way (e.g. roughness, mix, or emissive gain) while preserving **opaque** surfaces (level SHALL NOT be expressed as **transparency** for metering).

#### Scenario: Textures drive appearance

- **WHEN** bread texture maps are present under the configured data paths
- **THEN** the sphere surface SHALL show **normal-mapped** detail and **roughness** variation characteristic of the maps, not a single flat color

#### Scenario: Missing texture fallback

- **WHEN** bread texture files are absent or invalid
- **THEN** the application SHALL still run and SHALL apply a **simple** fallback material (e.g. procedural tint) and MAY log a warning

## MODIFIED Requirements

### Requirement: Three-dimensional sphere with sixteen surface patches

The system SHALL render the 16 channel levels on a **single sphere** in **three dimensions**. The sphere SHALL use **sufficient tessellation** (many small faces) so the geometry appears **smooth** rather than faceted at the silhouette. The surface SHALL still be partitioned into **16** contiguous **logical regions** in **spherical coordinates** (meridian × azimuth bands) with a **fixed, stable mapping** from **channel index** 0–15 to those regions. **Adjacent regions** SHALL **blend** channel levels at shared boundaries so that shading and color **transition smoothly** (a **gradient-like** appearance) rather than only uniform color per coarse quad.

#### Scenario: Sixteen regions with smooth blending

- **WHEN** the application window is showing the 3D meter view
- **THEN** the user SHALL perceive **sixteen** channel-associated zones on the sphere with **smooth** transitions between neighboring zones and a **smooth** underlying mesh

### Requirement: Level encoded as lightness on fixed per-channel hues

For each channel **c**, the system SHALL retain a **distinct** **hue identity** (or equivalent art direction) per channel. The **smoothed level** SHALL modulate **perceived intensity** through **PBR parameters** and/or **HSB**-derived tints **in addition to** bread **albedo/normal/roughness** maps, such that **quiet** channels read **dimmer or rougher** and **loud** channels read **brighter or glossier** as defined in implementation. **Level SHALL NOT** be represented by **varying alpha** for metering; **fills remain opaque**.

#### Scenario: Quiet vs loud channel

- **WHEN** one channel’s smoothed level is near zero and another channel’s level is high
- **THEN** the corresponding regions SHALL remain **visually distinguishable** and SHALL reflect level through **shading** and **material** response, not transparency
