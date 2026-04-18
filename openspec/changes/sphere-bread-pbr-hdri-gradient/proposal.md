## Why

The current meter sphere uses a **coarse 4×4** patch mesh and **flat** HSB fills. That reads as **blocky** and **diagram-like** rather than a continuous instrument. Increasing tessellation, **blending channel values** across the surface, and adding **PBR** materials with an **HDRI** environment will make the visualization richer and closer to a “object in a lit scene” while still mapping **16 channels** of audio level.

## What Changes

- **Subdivide** the sphere geometry (many more triangles) so the silhouette and lighting read **smoothly**; keep **16 fixed logical channel regions** on the sphere (same mapping as today) but sample / interpolate levels so **adjacent regions blend** into a **gradient** rather than hard steps.
- **Drive appearance** with a **PBR-style** material using **bread-like** textures (albedo/detail normal/roughness/metallic or packed maps as available) so the surface suggests **crust, crumb, and softness** within the limits of real-time shading.
- Add an **HDRI** (equirectangular or prefiltered cubemap) for **image-based lighting** and optional **background** so lighting is consistent and appealing.
- **Audio path unchanged** (16 RMS → same consumption contract for visualization).

## Capabilities

### New Capabilities

- _(none — extends existing sphere visualization spec)_

### Modified Capabilities

- `sphere-3d-channel-display`: Replace coarse patch-only description with **high-resolution mesh**, **gradient interpolation** between channel domains, **PBR bread material**, and **HDRI** lighting; refine how **level** modulates material (not only flat HSB fills).

## Impact

- **`src/`** (`SphereMeterView`, shaders, possibly `ofApp` setup): programmable pipeline, texture loading, environment map.
- **`bin/data/`**: new texture and HDRI assets (or documented placeholders until art is finalized).
- **GPU / performance**: higher triangle count and PBR — may need caps or LOD later.
