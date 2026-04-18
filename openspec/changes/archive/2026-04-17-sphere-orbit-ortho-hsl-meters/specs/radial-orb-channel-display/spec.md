## REMOVED Requirements

### Requirement: Donut orb layout with fixed clockwise sectors

**Reason**: Replaced by the **3D sphere** meter view (`sphere-3d-channel-display`).

**Migration**: Remove 2D donut drawing; implement 16 patches on a sphere per the new capability.

### Requirement: Opacity-only mapping on fixed donut sectors

**Reason**: Level is now encoded as **HSL lightness** on **opaque** surfaces in 3D, not opacity on a 2D annulus.

**Migration**: Use per-channel fixed hue and lightness from level; remove alpha-based meter mapping for the primary view.
