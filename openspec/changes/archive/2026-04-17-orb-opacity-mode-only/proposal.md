## Why

The radial orb currently supports three mappable display modes (A/B/C). In practice, **opacity on a fixed full-sector wedge (Mode C)** is the preferred visualization; maintaining three code paths and on-screen mode switching adds complexity without enough benefit for this app.

## What Changes

- **Remove** radial fill (Mode A) and annular-thickness (Mode B) drawing paths and any **mode-selection** UI or input (e.g. keys 1–3).
- **Keep** the existing **donut layout** (16 sectors, clockwise from 12 o’clock, center hole) and **Mode C** behavior: full annulus slice per channel with **alpha** driven by smoothed level.
- **Simplify** types and `draw()` to a single visualization path.

## Capabilities

### New Capabilities

- _(none — behavior is a narrowing of an existing capability)_

### Modified Capabilities

- `radial-orb-channel-display`: Restrict the orb to **opacity-only** mapping on fixed sector geometry; **remove** multi-mode requirements and mode-switching behavior.

## Impact

- **`src/ofApp.h` / `src/ofApp.cpp`**: drop `OrbDisplayMode` (or equivalent), mode branches in `draw()`, and `keyPressed` bindings for 1/2/3; retain sector geometry helpers and Mode C–style fill.
- **User-facing**: no mode labels or key hints; one consistent orb look.
- **Audio**: unchanged (RMS/metering and stereo monitor as today).
