## 1. Audio stream setup

- [x] 1.1 Add named constants for `kNumInputChannels` (16), `kNumOutputChannels` (2), and initial buffer size; document sample rate target (e.g. 48000) in a comment next to `ofSoundStream` setup.
- [x] 1.2 In `setup()`, enumerate or log available audio devices and identify BlackHole 16ch (substring match or index); set `ofSoundStream` settings for duplex input/output with 16 in / 2 out.
- [x] 1.3 Start the stream and verify in logs: actual sample rate, buffer size, and that 16 input channels are active (or fail loudly if not).

## 2. Thread-safe buffer handoff

- [x] 2.1 Implement `audioIn` (and `audioOut` if using separate callbacks per OF version) to **copy** incoming samples into a double buffer or ring buffer with channel stride handled correctly for the API.
- [x] 2.2 In `update()`, consume the latest snapshot and compute per-channel **peak** (or RMS) floats for channels 0–15; store in `std::array` or `std::vector` of atomics if needed for tear-free read in `draw`.
- [x] 2.3 Ensure no heavy work or allocation in audio callbacks.

## 3. Stereo monitor mix

- [x] 3.1 In `audioOut`, write stereo interleaved output using the design’s default mix (sum of all channels with gain/clamp) so the monitor reflects input energy safely.
- [x] 3.2 Add a master monitor gain constant (e.g. `0.2f`) to reduce risk of clipping when summing 16 channels; document in code.

## 4. Per-channel visualization

- [x] 4.1 In `draw()`, lay out **16** vertical or horizontal bars (or a 4×4 grid) labeled or ordered by channel index 1–16.
- [x] 4.2 Map each channel’s peak value to bar height/width with smoothing optional (e.g. decay) for readability.
- [x] 4.3 On-screen debug text: FPS optional; sample rate and buffer size once.

## 5. Verification

- [x] 5.1 Run with Ableton → BlackHole → OF: confirm all 16 meters move independently when tracks are routed to separate channels. _(manual — run app with Live routing)_
- [x] 5.2 Confirm stereo output is audible (or document toggle if monitoring is optional) and not clipping at nominal Ableton levels. _(manual — listen on headphones; adjust Live/BlackHole levels)_
