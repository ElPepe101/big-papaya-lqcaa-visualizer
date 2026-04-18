## Why

The app should ingest **16 discrete channels** from **BlackHole** (routed from Ableton) so each channel can be **metered and visualized at once**, while still providing **stereo monitoring** so the machine remains usable for hearing a downmix or cue. Today the codebase is an empty OF shell with no audio I/O or metering—this change defines the first vertical slice that proves routing, stability, and a path to geometric visuals driven by per-channel levels.

## What Changes

- Add **multi-channel audio input** (16 input channels) targeting the BlackHole virtual device via `ofSoundStream` (or equivalent OF audio setup on macOS).
- Implement **per-channel metering** (e.g. peak or RMS) updated in real time for **all 16 channels simultaneously**.
- Add **simultaneous visualization** of those levels (minimal UI: bars or grid—enough to validate signal path and layout before “final” geometry).
- Add **stereo audio output** (2 channels) for **monitoring** (e.g. downmix or selectable pair)—exact mix strategy is specified in design/specs, not left implicit.
- Document assumptions: **sample rate alignment** with Ableton/BlackHole, **buffer sizing**, and **thread-safety** between audio callback and draw.

## Capabilities

### New Capabilities

- `multi-channel-audio-capture`: Open and maintain a single input stream with **16 channels** from the selected device (BlackHole 16ch), with stable buffer delivery into the app.
- `per-channel-metering-visualization`: Compute lightweight level metrics per channel on the main thread (from copied buffers) and render **all 16** levels concurrently for debugging and artistic iteration.
- `stereo-output-monitoring`: Open **stereo output** alongside input; define how output relates to inputs (e.g. summed mix, selected channels, or silent placeholder until mix rules exist).

### Modified Capabilities

<!-- No existing specs in openspec/specs/ — none. -->

## Impact

- **Code**: `ofApp` and related `src/` files; possible small helpers for ring buffer / atomics; no breaking public API (app is greenfield).
- **System**: macOS **Core Audio** routing—Ableton → BlackHole; OF uses BlackHole as input; output device user-selectable or default (built-in/aggregate per design).
- **Dependencies**: OF **0.12.x** with existing `Makefile` / `OF_ROOT`; no new addons required for v1 unless design chooses an addon for FFT later (out of scope for metering-only v1).
- **Operational**: User must install/configure **BlackHole 16ch** and match **sample rate** across Ableton, BlackHole, and the app.
