## 1. Code simplification

- [x] 1.1 Remove `OrbDisplayMode` (and `orbMode`) from `ofApp`; delete Mode A and Mode B branches in `draw()`.
- [x] 1.2 Keep a single orb path: full annulus slice per sector, `ofSetColor` with alpha from normalized level (reuse existing `channelVisualLevel` / alpha mapping used for Mode C).

## 2. Input and UI

- [x] 2.1 Remove `keyPressed` handling for keys **1** / **2** / **3** (or strip to no-op if shared handler must remain).
- [x] 2.2 Remove on-screen text that names modes A/B/C or lists `[1]`/`[2]`/`[3]`; keep title and RMS/debug lines as appropriate.

## 3. Verification

- [x] 3.1 `make` succeeds; run the app and confirm 16 clockwise sectors, center hole, and opacity-only response to level.
