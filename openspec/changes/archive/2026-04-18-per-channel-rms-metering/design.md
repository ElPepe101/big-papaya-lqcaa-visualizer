## Context

- **Current behavior**: `ofApp` copies interleaved audio in `audioIn`, then in `update()` computes **per-channel peak** with abs max over frames and a **0.92** decay on `peakDisplay[]` for drawing 16 bars.
- **Goal**: Use **RMS** per channel per snapshot for a more **loudness-aligned** meter, plus **display smoothing** so bars feel stable and “broadcast-ish” without implementing full **LUFS** (EBU R 128) or K-weighting.

## Goals / Non-Goals

**Goals:**

- For each audio snapshot, compute **RMS** per channel: for channel `c`, over all frames `f` in the snapshot,  
  \(\text{RMS}_c = \sqrt{\frac{1}{N}\sum_f s_{f,c}^2}\) with \(s\) the interleaved sample values.
- Maintain computation on the **main thread** in `update()` from the mutex-protected copy (no extra work in `audioIn`).
- Apply **exponential smoothing** on the scalar sent to `draw()` (e.g. `display[c] = alpha * rms + (1-alpha) * display[c]` or separate attack/release).
- Normalize or map RMS to bar height (e.g. `ofMap` with clamp, optional `sqrt` for more motion at low levels — tuning in code).

**Non-Goals:**

- **ITU-R BS.1770** / **LUFS** integrated loudness, true-peak, or gating (future change if needed).
- Changing **audio I/O**, **BlackHole routing**, or **stereo monitor** mix.

## Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| RMS window | **One block** = one `audioIn` buffer (variable length = `getNumFrames()` in snapshot) | Matches current architecture; latency = one buffer; simple. |
| Smoothing | **EMA** on displayed value with coefficient ~0.85–0.95 for “release” (tune by ear) | Cheap; no separate attack/release unless needed. |
| Stereo monitor | **Unchanged** | This change is visualization/analysis only. |
| Constants | Named `kRmsSmoothing` (or attack/release) next to existing gains | Matches project style (`lqcaa::` namespace). |

**Alternatives considered:**

- **Sliding RMS over multiple buffers** — better time resolution, more state; defer.
- **dBFS display** — `20*log10(rms)` — can add later; linear + map is enough for v1.

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Very quiet channels → tiny RMS | Optional floor in map or sqrt curve in `draw()` only. |
| Heavier CPU than peak | RMS is still O(frames × 16); negligible vs graphics. |
| Smoothed bars “lag” transients | Acceptable for loudness-ish; document tuning constants. |

## Migration Plan

- Replace peak loop in `update()` with RMS + smoothing; remove or rename `peakDisplay` to `meterDisplay` if clearer.
- Rollback: revert commit or restore peak branch.

## Open Questions

- Whether to show **numeric RMS or dB** in debug text (optional follow-up).
