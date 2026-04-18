## Why

Raw **peak** metering (with decay) reacts to transients and often leaves bars looking quiet compared to how we *hear* level. **RMS** per channel better matches **perceived loudness** for steady content and makes the 16-channel visualizer feel more “alive” and mix-relevant without changing the audio path—only the analysis driving the meters.

## What Changes

- Replace per-channel **peak** analysis with **RMS** computed over each processed input snapshot (the buffer copied from `audioIn`), independently for channels 0–15.
- Apply **light display smoothing** (attack/release or exponential smoothing on the RMS values) so motion remains readable and “broadcast-ish” without requiring full LUFS/ITU-R compliance.
- Keep **simultaneous 16-bar** layout; mapping from normalized RMS to bar height may use a simple curve (implementation detail in design).

## Capabilities

### New Capabilities

<!-- None — behavior change is scoped to existing metering capability. -->

### Modified Capabilities

- `per-channel-metering-visualization`: Level metric SHALL be **RMS-based** (not optional peak) with stated smoothing for visualization; requirements updated in the delta spec.

## Impact

- **Code**: `ofApp::update()` (and any helpers) — RMS per channel from `lastInputInterleaved`; adjust or replace `peakDisplay` smoothing constants.
- **Specs**: Delta under `specs/per-channel-metering-visualization/spec.md` merging into `openspec/specs/per-channel-metering-visualization/spec.md` on archive.
- **Dependencies**: None beyond OF core; no new addons.
