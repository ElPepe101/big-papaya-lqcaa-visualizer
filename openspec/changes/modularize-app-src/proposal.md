## Why

`ofApp.cpp` has grown to bundle **device picking**, **per-channel RMS smoothing**, **stereo monitor output**, and **2D orb drawing** in one translation unit. Splitting responsibilities into separate files makes the project easier to navigate, test mentally, and extend (e.g. alternate visuals) without a single 300+ line file.

## What Changes

- Introduce **additional `.cpp` / `.h` files** under `src/` (and optionally a small subfolder or `namespace lqcaa` headers) for logical boundaries.
- **Move** implementation out of `ofApp.cpp` while keeping **`ofApp` as the OF entry point**: `ofSoundStream` callbacks and app lifecycle stay on `ofApp`.
- **No intentional change** to audio routing, RMS math, monitor mix, or orb appearance (refactor-only).

## Capabilities

### New Capabilities

- `app-source-modularity`: Normative expectations for how application source is split across modules while preserving observable behavior.

### Modified Capabilities

- _(none — audio and radial-orb behavior specs are unchanged at the product level)_

## Impact

- **`src/`**: new files; `ofApp.cpp` / `ofApp.h` shrink to wiring.
- **Build**: default OF `Makefile` / `compile.project.mk` should pick up new `.cpp` files under `src/` (verify with `make`).
- **Dependencies**: none beyond OpenFrameworks and the standard library.
