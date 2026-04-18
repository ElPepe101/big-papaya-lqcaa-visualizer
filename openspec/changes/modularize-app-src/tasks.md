## 1. Headers and constants

- [ ] 1.1 Add a small shared header (e.g. `Constants.h` or equivalent) holding the existing `namespace lqcaa` constexpr/constants now in `ofApp.h`, and include it from modules + slim `ofApp.h` as needed.

## 2. Extract modules

- [ ] 2.1 Move `pickBlackHole16In` / `pickDefaultStereoOut` into `SoundDevicePickers.cpp` / `SoundDevicePickers.h` (or chosen names), same behavior and logging.
- [ ] 2.2 Move per-channel RMS + EMA update (from `lastInputInterleaved` → `meterDisplay`) into a dedicated `.cpp`/`.h` (e.g. `MeterBlocks.cpp`), callable from `ofApp::update`.
- [ ] 2.3 Move orb helpers (`channelVisualLevel`, sector angles, annulus polygon, rim/spokes draw) into `RadialOrbView.cpp` / `.h` (or equivalent), exposing a function like `drawRadialOrb(const std::array<float, kNumInputChannels> &)` or equivalent minimal API.
- [ ] 2.4 Optionally extract stereo monitor fill (`audioOut` inner loop) into a small helper in its own or shared `.cpp` to keep `ofApp::audioOut` readable.

## 3. ofApp wiring

- [ ] 3.1 Reduce `ofApp.cpp` to setup/draw/audio delegates, empty input stubs, and member state; ensure includes resolve and no duplicate symbols.

## 4. Verification

- [ ] 4.1 `make` Release succeeds; run app and confirm orb + RMS line + monitor behavior match pre-refactor.
