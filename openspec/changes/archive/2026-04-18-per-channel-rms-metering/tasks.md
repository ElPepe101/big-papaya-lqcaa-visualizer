## 1. RMS computation

- [x] 1.1 In `update()`, replace per-channel peak (abs max over frames) with **RMS** per channel over all frames in `lastInputInterleaved` (sqrt of mean of squares per channel).
- [x] 1.2 Add named smoothing constant(s) (e.g. EMA coefficient `kMeterSmoothing` in `lqcaa` namespace) and apply smoothing to the values used for drawing.

## 2. Visualization

- [x] 2.1 Map smoothed RMS to bar height using `ofMap` (or equivalent) with sensible clamp; optionally apply a curve (e.g. `sqrt`) for low-level motion — document in a short comment.
- [x] 2.2 Update UI copy / debug line to state **RMS**-driven meters (not raw peak).

## 3. Verification

- [x] 3.1 `make` succeeds; run app with Live → BlackHole and confirm meters feel more “loudness-like” and less twitchy than peak-only. *(build verified; Live session = manual)*
