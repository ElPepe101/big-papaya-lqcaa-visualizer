## ADDED Requirements

### Requirement: Multiple translation units for the application

The application source SHALL be organized into **more than one** `.cpp` file under `src/` (excluding `main.cpp`), such that **device selection**, **per-channel level processing** (RMS / smoothing from the interleaved input snapshot), **orb visualization drawing**, and **`ofApp` lifecycle wiring** are not all implemented in a single monolithic application implementation file.

#### Scenario: Build includes new sources

- **WHEN** the project is built with the project `Makefile` and default OpenFrameworks project rules
- **THEN** all new `.cpp` files placed under `src/` that are part of this modularization SHALL be compiled into the application without requiring a non-default manual listing of every filename (unless the toolchain explicitly requires it for this repo, in which case the listing SHALL be updated accordingly)

### Requirement: Refactor preserves observable behavior

The modularization SHALL **not** intentionally change: BlackHole 16ch input selection behavior (including fallbacks as implemented), stereo summed monitor output with gain and clip flag behavior, per-channel RMS + EMA metering values used for drawing, or the 2D donut orb layout and opacity mapping described in existing specs.

#### Scenario: Parity check

- **WHEN** the refactored build is run with the same audio routing and window size as before modularization
- **THEN** the user-facing orb and debug text SHALL match the prior behavior within normal floating-point variance for meter values

### Requirement: Audio stream entry points remain on ofApp

`ofSoundStream` input and output handling SHALL remain implemented through **`ofApp`** (or the class registered as `ofSoundStream` listener), not by relocating listener interfaces to unrelated types in a way that would break the OpenFrameworks audio callback contract.

#### Scenario: Listener registration unchanged in spirit

- **WHEN** the application starts
- **THEN** the same logical audio device selection and `ofSoundStream` setup SHALL occur, with `audioIn` / `audioOut` still driven through the application object that subclasses `ofBaseApp` as in the pre-refactor design
