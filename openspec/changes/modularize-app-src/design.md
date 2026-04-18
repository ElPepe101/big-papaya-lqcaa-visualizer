## Context

LQCAA today uses `src/ofApp.cpp` for: BlackHole/default device helpers (anonymous namespace), orb sector geometry and drawing helpers, `setup` stream configuration, `update` RMS accumulation, `draw` orb + chrome, `audioIn`/`audioOut` I/O.

## Goals / Non-Goals

**Goals:**

- **Clear modules** with narrow responsibilities and stable names under `namespace lqcaa` where it fits existing constants.
- **`ofApp` remains thin**: owns `ofSoundStream`, mutex-protected buffers, `meterDisplay`, and delegates to free functions or small types.
- **`make` succeeds** without custom source list edits (all `.cpp` under `src/` compiled by the stock project makefile).

**Non-Goals:**

- Changing **constants** (channel count, gains, EMA blend) or **visual appearance** of the orb.
- Introducing addons, CMake, or a non-OF build system.
- Full unit-test harness (optional later).

## Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Module boundaries | **(1)** Sound device pickers<br>**(2)** Meter update from interleaved buffer<br>**(3)** Orb draw (geometry + frame layout)<br>**(4)** Optional: stereo monitor write helper | Matches natural cuts in current `ofApp.cpp`; minimizes churn. |
| API style | **Free functions** in `namespace lqcaa` + headers, or **one small struct** only if state appears | Matches current anonymous-namespace style; avoids over-OOP. |
| Constants | Keep **`lqcaa` constexpr block** in a **single header** (e.g. `Constants.h` or remain in a dedicated header included by all) | One source of truth; `ofApp.h` can include it or shrink. |
| File names | **PascalCase or OF-style** — e.g. `SoundDevicePickers.cpp`, `MeterBlocks.cpp`, `RadialOrbView.cpp` (exact names in tasks) | Consistent with OF samples; pick one convention in implementation. |

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| Circular includes | Forward-declare where possible; keep drawing headers from including `ofApp.h`. |
| Makefile does not compile new `.cpp` | After first split, run `make` and add to `PROJECT_SOURCES` only if required by this project’s OF version. |

## Migration Plan

1. Extract helpers bottom-up (pickers → meters → orb → slim `ofApp`).
2. `make` + run app: visual and audio parity with pre-refactor.

## Open Questions

- Whether to use a `src/lqcaa/` subfolder vs flat `src/*.cpp` — **either is fine**; flat is fewer path tweaks.
