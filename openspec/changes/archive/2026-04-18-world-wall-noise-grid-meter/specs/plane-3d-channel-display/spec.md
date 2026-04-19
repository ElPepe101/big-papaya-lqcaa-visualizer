## ADDED Requirements

### Requirement: Canonical primary meter reference

The primary three-dimensional 16-channel meter behavior SHALL be specified by `world-wall-noise-grid-display` after this migration completes.

#### Scenario: Implementers follow wall-grid spec

- **WHEN** implementing or testing the primary 3D meter
- **THEN** requirements SHALL be read from `world-wall-noise-grid-display`, not from historical bread ground-plane requirements removed below

## REMOVED Requirements

### Requirement: Tessellated plane meter primitive

**Reason:** Primary meter is no longer a textured ground plane; replaced by world-fixed wall grid.

**Migration:** `world-wall-noise-grid-display` Requirement: World-fixed wall grid.

### Requirement: Blended multi-channel displacement field

**Reason:** Row-channel bands replace Gaussian-blended field on the ground plane.

**Migration:** `world-wall-noise-grid-display` scope and World-fixed wall grid.

### Requirement: Infinite-looking staging

**Reason:** No longer required for the primary meter (no infinite horizon ground).

**Migration:** Optional background only; not normative in `world-wall-noise-grid-display`.

### Requirement: Perspective camera with fixed-up orbit

**Reason:** Normative camera behavior moves to the wall capability (same pattern, new spec).

**Migration:** `world-wall-noise-grid-display` Requirement: Perspective camera with fixed-up orbit.

### Requirement: Image-based lighting from an HDRI environment

**Reason:** HDRI not required for default wall meter.

**Migration:** None for default meter; optional future ambient pass.

### Requirement: Bread-like PBR surface

**Reason:** Bread visualizer removed from primary path.

**Migration:** `world-wall-noise-grid-display` scope (grayscale only).

### Requirement: Vertex-shader-only geometric displacement

**Reason:** Displacement from `displacement.png` and bread field removed; VS-only noise rule restated on wall spec.

**Migration:** `world-wall-noise-grid-display` Requirement: Normal-direction noise displacement (VS-only).

### Requirement: Level encoded without transparency

**Reason:** Hue/PBR encoding superseded by grayscale modes on wall spec.

**Migration:** `world-wall-noise-grid-display` Requirements: Grayscale level visualization with dual darkness options and Opaque surfaces.

### Requirement: Audio pipeline unchanged

**Reason:** Duplicate under retired capability; restated on wall spec.

**Migration:** `world-wall-noise-grid-display` Requirement: Audio pipeline unchanged.
