## ADDED Requirements

### Requirement: Square UV sphere mesh density

The system SHALL tessellate the bread meter sphere with a **square grid** of **N×N** quadrilaterals (expressed as triangles in the mesh), where **N** is a **positive integer** divisible by **16** so that **16** channel latitude bands each contain an **equal** integer count of latitude subdivisions. Texture coordinates **u** and **v** on each quad corner SHALL span **contiguous** ranges in **[0, 1]** such that each quad occupies approximately a **1/N × 1/N** portion of UV space (no **independent** mismatch between **u** and **v** scale that would **elongate** square **4096×4096** bread maps).

#### Scenario: UV unwrap matches mesh columns and rows

- **WHEN** the UV unwrap debug view is enabled (if present in the application)
- **THEN** the overlay grid SHALL show **N** divisions along **u** and **N** divisions along **v** consistent with the mesh parameter **N**

### Requirement: Minimum default mesh tier

The implementation SHALL choose a **default** value of **N** that is **not less than 1024**, unless a **documented** platform or performance constraint forces a lower default; in that case the **documented** default and **rationale** SHALL appear in the change **tasks** or **design** notes.

#### Scenario: Startup reports mesh size

- **WHEN** the sphere meter view finishes one-time mesh setup
- **THEN** the application SHALL emit a **log line** (or equivalent) that includes **N** and **approximate vertex count** so operators can verify the active mesh tier

### Requirement: Vertex-shader-only displacement

All **radial** (or normal-aligned) **geometric** offsets driven by **`displacement.png`** and by **per-channel meter** values for the bread sphere SHALL be computed **only** in the **vertex shader**. The **fragment shader** SHALL NOT apply **parallax**, **relief**, or **height-based ray-march** displacement for those effects in the scope of this change.

#### Scenario: Fragment shader has no displacement sampling for audio

- **WHEN** inspecting the bread sphere **fragment** shader source for this feature set
- **THEN** there SHALL be **no** texture sample of **`displacement.png`** used to **offset** view or surface position for **audio** metering (audio-driven shape remains **VS-only**)

#### Scenario: Bread crust height remains VS-sampled

- **WHEN** the bread **displacement** map is loaded
- **THEN** the **vertex shader** SHALL sample that map (or a bound equivalent) to offset vertices along the **outward** normal, and the **fragment shader** MAY continue to use **normal**, **albedo**, **roughness**, and **AO** maps for shading without displacing geometry in the FS
