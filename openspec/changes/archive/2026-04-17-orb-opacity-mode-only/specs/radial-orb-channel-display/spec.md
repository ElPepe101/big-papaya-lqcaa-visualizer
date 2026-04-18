## REMOVED Requirements

### Requirement: Three display modes (A, B, C)

**Reason**: The product only needs one visualization: fixed full-sector geometry with level mapped to opacity (former Mode C).

**Migration**: Remove mode selection UI and non-opacity draw branches; implement a single opacity-driven fill per sector.

### Requirement: Mode switching without audio restart

**Reason**: Multiple display modes are removed; there is no mode to switch.

**Migration**: None.

## ADDED Requirements

### Requirement: Opacity-only mapping on fixed donut sectors

The system SHALL render each channel’s smoothed level as **opacity** (or equivalent alpha) on a **fixed** annulus sector: the **inner** and **outer** radii of the colored region SHALL NOT vary with level; only the **fill opacity** SHALL vary with level. Level SHALL be derived from the same smoothed per-channel RMS pipeline used elsewhere (including consistent mapping and shaping such as `ofMap` into a normalized range and `sqrt` for display, as implemented).

#### Scenario: Silence vs loud channel

- **WHEN** one channel’s smoothed level is near zero and another channel’s level is high
- **THEN** the quiet channel’s sector SHALL appear visually weaker (lower opacity) and the loud channel’s sector SHALL appear stronger (higher opacity), with both sectors covering the same annular wedge geometry

#### Scenario: No alternate geometry modes

- **WHEN** the application is running
- **THEN** the orb SHALL NOT offer radial-extent-only fills or inner-radius-thickness wedge modes as selectable behaviors
