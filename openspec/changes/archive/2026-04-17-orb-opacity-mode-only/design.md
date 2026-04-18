## Context

The app already draws a 16-channel donut orb with optional modes A/B/C (`OrbDisplayMode`, keys **1** / **2** / **3**). This change collapses that to **Mode C only**: each sector is a fixed annulus slice; **loudness → opacity** (with existing level shaping: `ofMap` to `[0, kRmsMapMax]` and `sqrt` for display).

## Goals / Non-Goals

**Goals:**

- Single code path in `ofApp::draw()` for the orb (no `if` on mode enum).
- Remove mode state, key handlers, and on-screen mode/key hints tied to A/B/C.
- Preserve donut geometry, sector order (ch1 at top, clockwise), inner/outer rim cues, and **alpha blending** for opacity.

**Non-Goals:**

- Changing RMS computation, EMA, buffer handling, or stereo monitor math.
- New interaction (e.g. hover labels) unless already trivial.

## Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Level → alpha | Reuse current mapping: `level = sqrt(ofMap(meter, …))`, then `alpha = map(level, 0, 1, 15, 255)` (or equivalent clamp) | Matches prior Mode C and spec “opacity ∝ level”. |
| API surface | Delete `OrbDisplayMode` and `orbMode` | No dead enums or unused branches. |
| Dividers / rings | Keep faint outer/inner circles and sector spokes after fills | Unchanged from current draw order (fills first, then outlines). |

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Users relied on A/B for comparison | Acceptable product choice; opacity-only is the desired default. |
| Spec merge order with `radial-orb-16ch-modes` | Delta spec targets the same capability name; archive/order resolved at merge time. |

## Migration Plan

1. Implement code removal and single-path draw.
2. `make` and smoke-test orb + audio.
3. Archive this change after verification.

## Open Questions

- None for v1.
