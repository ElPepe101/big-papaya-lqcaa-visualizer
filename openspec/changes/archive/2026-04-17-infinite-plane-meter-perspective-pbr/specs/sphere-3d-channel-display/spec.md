## ADDED Requirements

### Requirement: Canonical 3D meter specification

The **`plane-3d-channel-display`** specification SHALL be the **authoritative** source for the **application’s** **primary** **three-dimensional** **16-channel** **bread** **meter** after this migration.

#### Scenario: Implementers follow plane spec

- **WHEN** implementing or testing the **primary** **3D meter**
- **THEN** **behaviors** SHALL be validated against **`plane-3d-channel-display`**, not historical **sphere** requirements **removed** by this change

## REMOVED Requirements

### Requirement: Image-based lighting from an HDRI environment

**Reason:** Primary **3D meter** is no longer the **sphere**; **HDRI** obligations for the **main meter** move to **`plane-3d-channel-display`**.

**Migration:** Implement and test **IBL** under `openspec/specs/plane-3d-channel-display/spec.md` (synced when this change archives).

### Requirement: Bread-like PBR surface

**Reason:** **Sphere**-specific wording; **bread PBR** for the **active** meter is specified on the **plane** capability.

**Migration:** See **`plane-3d-channel-display`** **Requirement: Bread-like PBR surface**.

### Requirement: Three-dimensional sphere with sixteen surface patches

**Reason:** The application **primary** meter **no longer** uses a **sphere** with **spherical-coordinate** patches.

**Migration:** Use **`plane-3d-channel-display`** **Requirement: Tessellated plane meter primitive** and **Requirement: Blended multi-channel displacement field**.

### Requirement: Square UV sphere mesh density

**Reason:** **Sphere** mesh requirement is **obsolete** for the **primary** meter.

**Migration:** **Plane** mesh density is covered by **`plane-3d-channel-display`** **Requirement: Tessellated plane meter primitive**.

### Requirement: Minimum default mesh tier

**Reason:** Was bound to **sphere** setup logging; superseded by **plane** startup logging requirement.

**Migration:** See **`plane-3d-channel-display`** **Requirement: Tessellated plane meter primitive** (**Scenario: Startup reports mesh tier**).

### Requirement: Vertex-shader-only displacement

**Reason:** **Sphere**-normal wording; the same **discipline** applies to the **plane** with **plane** normals.

**Migration:** See **`plane-3d-channel-display`** **Requirement: Vertex-shader-only geometric displacement**.

### Requirement: Orthographic camera with fixed-up classic orbit

**Reason:** **Primary** meter uses **perspective** per product direction; **orthographic** **orbit-at-sphere-center** is **no longer** required.

**Migration:** See **`plane-3d-channel-display`** **Requirement: Perspective camera with fixed-up orbit**.

### Requirement: Level encoded as lightness on fixed per-channel hues

**Reason:** **Sphere**-centric spec file **gutted** for **primary** meter; **level** encoding moves to **plane** capability.

**Migration:** See **`plane-3d-channel-display`** **Requirement: Level encoded without transparency**.

### Requirement: Audio pipeline unchanged

**Reason:** **Duplicate** maintenance on a **retired** capability; **audio** contract is **restated** on **`plane-3d-channel-display`**.

**Migration:** See **`plane-3d-channel-display`** **Requirement: Audio pipeline unchanged**.
