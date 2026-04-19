## ADDED Requirements

### Requirement: world-wall-noise-grid-display scope

The primary three-dimensional 16-channel meter SHALL be a world-fixed vertical wall mesh with a visible grid, vertex-shader-only displacement along the wall outward normal driven by smooth noise with per-channel phase, grayscale shading from smoothed levels, two selectable darkness-mapping dimensions (spatial and level-encoding modes per design), and opaque surfaces. The visualization SHALL not require bread textures, PBR maps, or HDRI for default operation. The audio input and monitoring contract SHALL remain unchanged.

#### Scenario: No bread or HDRI required for default run

- **WHEN** the application starts without `bread/` assets or HDRI files
- **THEN** the primary meter view SHALL still render the wall grid meter without fatal error

### Requirement: World-fixed wall grid

The system SHALL render a single connected tessellated wall in world space on a vertical plane (implementation SHALL document axes, e.g. XY at fixed Z). The mesh SHALL partition the wall into exactly 16 horizontal row bands, each band associated with one input channel index 0–15 in a documented order, and each band SHALL subdivide horizontally into at least 32 column steps (finer grid than one quad per channel).

#### Scenario: Row-to-channel mapping is stable

- **WHEN** operators inspect the wall grid across sessions
- **THEN** channel c SHALL always map to the same row band index per documented layout

### Requirement: Normal-direction noise displacement (VS-only)

All time-varying geometric offset from procedural noise for the wall meter SHALL be computed in the vertex shader only, along the wall outward normal. Each channel row SHALL use a distinct phase or equivalent parameter so adjacent rows do not move in lockstep. The fragment shader SHALL not implement parallax or height ray-march for this metering geometry.

#### Scenario: Fragment does not displace geometry for noise meter

- **WHEN** reviewing the wall meter shaders
- **THEN** position offsets for noise motion SHALL appear only in the vertex stage

### Requirement: Grayscale level visualization with dual darkness options

The system SHALL map each channel’s smoothed level to grayscale luminance on the corresponding row band without using alpha as level. The system SHALL expose at least two user-selectable options for how darkness is applied, covering both: (1) spatial variation within a row (per-cell or per-vertex versus uniform across the row) and (2) level encoding (linear RMS-based versus log/dB-scaled mapping consistent with existing meter scaling conventions). Exact UI binding is implementation-defined.

#### Scenario: User can switch darkness mapping modes at runtime

- **WHEN** the user invokes the documented mode control (e.g. key binding or UI)
- **THEN** the visual mapping SHALL change between the paired options without restarting the application

### Requirement: Opaque surfaces

Level SHALL not be represented by varying alpha on the wall meter surface; fills remain opaque.

#### Scenario: No alpha metering

- **WHEN** channels vary from silent to loud
- **THEN** the wall SHALL not rely on transparency to encode level

### Requirement: Perspective camera with fixed-up orbit

The main 3D view SHALL use perspective projection unless explicitly overridden in a future change. The user SHALL navigate with pointer drag using orbit-like behavior: world +Y up, no roll, stable look-at anchor documented in implementation.

#### Scenario: Orbit does not roll the camera

- **WHEN** the user drags to orbit
- **THEN** the camera up vector SHALL remain aligned with +Y (no roll)

### Requirement: Audio pipeline unchanged

The visualization SHALL consume the same 16 per-channel smoothed RMS-derived values from the existing metering path. This capability SHALL not alter audio capture, device selection, or stereo monitor mixing.

#### Scenario: Metering source unchanged

- **WHEN** audio is running and levels update
- **THEN** the values driving the wall meter SHALL match the same smoothing pipeline as before this change
