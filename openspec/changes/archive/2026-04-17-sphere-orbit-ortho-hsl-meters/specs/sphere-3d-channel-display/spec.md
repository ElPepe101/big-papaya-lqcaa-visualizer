## ADDED Requirements

### Requirement: Three-dimensional sphere with sixteen surface patches

The system SHALL render the 16 channel levels on a **single sphere** in **three dimensions**. The sphere surface SHALL be partitioned into **16** contiguous **surface patches** arranged as a **4×4** grid in **spherical coordinates**: **4** divisions along **meridian** (polar angle) and **4** along **azimuth** (parallel / longitude), such that each patch corresponds to a **channel index** 0–15 with a **fixed, documented mapping** (implementation-defined order, but MUST be stable across frames).

#### Scenario: Sixteen patches visible

- **WHEN** the application window is showing the 3D meter view
- **THEN** the user SHALL see sixteen distinct shaded surface regions on the sphere, each corresponding to one input channel

### Requirement: Orthographic camera with fixed-up classic orbit

The system SHALL use an **orthographic** projection for the main 3D view (allowing scale / viewport to be implemented as a camera parameter). The user SHALL **navigate** with a **pointer drag** using **classic orbit** behavior: **world vertical axis** (+Y) remains **up** (no roll); horizontal drag rotates the **view** around **that vertical axis** through the sphere center; vertical drag adjusts **elevation** (pitch) with **clamped** angles so the view does not flip past the poles. The **look-at target** SHALL remain the **sphere center** so the orbit does not **lose** the anchor point.

#### Scenario: orbit keeps center fixed

- **WHEN** the user drags to orbit the camera
- **THEN** the camera SHALL continue to face the sphere center as the orbit target and SHALL not translate the sphere center away from the anchor

### Requirement: Level encoded as lightness on fixed per-channel hues

For each channel **c**, the system SHALL assign a **fixed, vibrant** hue (and a **fixed saturation** in the **HSL** or **HSB** representation used by the implementation) that **distinguishes** channels from one another. The **smoothed level** used for visualization SHALL be mapped to a **lightness** (or **brightness** in HSB) in the **[0, 1]** range (or equivalent normalized range). **Level SHALL NOT** be represented by **varying alpha**; **fill SHALL be opaque** (full opacity).

#### Scenario: Quiet vs loud channel

- **WHEN** one channel’s smoothed level is near zero and another channel’s level is high
- **THEN** the quiet channel’s patch SHALL appear **darker** (lower lightness) and the loud channel’s patch SHALL appear **lighter** (higher lightness), while **hue identity** per channel remains recognizable

### Requirement: Audio pipeline unchanged

The visualization SHALL consume the same **16** per-channel smoothed RMS-derived values produced by the existing metering path; this change SHALL NOT alter **audio capture**, **device selection**, or **stereo monitor** mixing behavior.

#### Scenario: Metering source unchanged

- **WHEN** audio is running and levels update
- **THEN** the values driving patch lightness SHALL be derived from the same per-channel smoothing pipeline as before this change
