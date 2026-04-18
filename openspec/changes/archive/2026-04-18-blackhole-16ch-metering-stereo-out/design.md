## Context

- **App**: OpenFrameworks 0.12.x C++ project (`LQCAA`) built via `Makefile` with `OF_ROOT` pointing at the sibling OF release tree.
- **Source**: Ableton Live sends tracks/buses to **BlackHole 16ch**; the OF app must **capture** all 16 channels as input for metering and simultaneous on-screen visualization.
- **Monitoring**: The user also wants **stereo output** (e.g. headphones/speakers)—typically a **downmix** or **sum** of inputs for cueing, unless a different mix rule is chosen later.

## Goals / Non-Goals

**Goals:**

- Single **input** stream with **16 input channels** targeting BlackHole (user selects device in code or via a minimal device list).
- **Copy audio safely** from the audio callback; compute **peak or RMS** per channel on the **main thread** (`update`) from the latest buffer snapshot.
- **Draw all 16 channel meters at once** (simple rectangles/bars) to validate routing and levels.
- **Stereo output** opened alongside input; play a **defined** monitor mix (default: **equal-gain sum** of all 16 inputs to L/R, normalized/clamped to avoid blast—exact shaping in implementation).
- Align **sample rate** with Ableton/BlackHole (typically 44.1k or 48k); document as a **user-facing** setting constant or startup check.

**Non-Goals:**

- Per-channel **FFT** or advanced analysis (defer to a later change).
- Separate **multi-device** capture (only BlackHole-style **one device, N channels**).
- Polished UI, presets, or DAW-like mixer (iterate later).

## Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Input API | `ofSoundStream` | Standard OF path for duplex audio; one callback for input (and output if using full duplex). |
| Channel count | Fixed **16** for v1 | Matches BlackHole 16ch and proposal; make `kNumInputChannels` a named constant for future flexibility. |
| Metering metric | **Peak** first (optionally RMS with short window) | Cheaper than FFT; sufficient to prove signal and drive geometry later. |
| Where to analyze | **After** copy to lock-free / double buffer; work in **`update()`** | Avoids heavy work in `audioIn` / `audioOut` callbacks; reduces dropouts. |
| Stereo monitor mix | **Sum all channels → mono or dual mono to L/R** with **soft clamp** or **gain reduction** | Simplest behavior that uses all channels without a full pan matrix; document that **true stereo imaging** from Ableton is already in the mix if user routes stereo pairs to specific channels (future enhancement). |
| Device selection | **Hardcode BlackHole device name substring** first, or **print/list devices** once in `setup` | Beginners need a reliable path; optional: env var or `ofxGui` later. |
| Sample rate | **Match project default** (e.g. 48000) and **log** actual rate | Mismatch with Ableton causes pitch/slip issues; fail visibly in log if mismatch detected. |

**Alternatives considered:**

- **Multiple `ofSoundStream` instances** — rejected for v1 (unnecessary for one 16ch device).
- **FFT per channel** — deferred (complexity + CPU).
- **Separate process for audio** — rejected (overkill for first slice).

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| BlackHole not showing 16 inputs | Verify in Audio MIDI Setup; use OF device enumeration log; confirm 16ch variant installed. |
| Buffer size too small → glitches | Start with moderate buffer (e.g. 512); tune after stable. |
| Monitor sum **too loud** | Apply master gain + clip warning in debug overlay. |
| Thread safety bugs | Single writer audio thread → ring buffer; single reader `update`; use `std::atomic` or double-buffer pattern for floats. |
| Duplex open fails on some macOS configs | Fall back: input-only first, then add output; document in tasks. |

## Migration Plan

- Not applicable (greenfield app). Rollback = revert `src/` changes and restore empty `ofApp` behavior.

## Open Questions

- Exact **BlackHole device string** on the user’s machine (varies by version)—resolve by logging `ofSoundStream` device list in `setup`.
- Preferred **default sample rate** (48k vs 44.1k) to match the user’s Ableton session.
- Whether monitor output MUST be **audible at all times** or can be **silent** until a key toggles monitoring (safety).
