## 1. Camera and scene

- [x] 1.1 Add orthographic 3D camera (or configure `ofEasyCam`) with **look-at** at sphere center; enable **depth test**; clear color + depth each frame.
- [x] 1.2 Implement **fixed +Y up** orbit: pointer drag → **yaw** around **Y** and **clamped pitch**; keep target at origin.

## 2. Sphere geometry and colors

- [x] 2.1 Build **4×4** sphere surface patches (parametric quads or tri pairs) with stable **channel index → patch** mapping; document mapping in a short comment.
- [x] 2.2 Assign **16** fixed **vibrant hues** (high saturation); map smoothed level `0…1` → **lightness** (or **brightness** in HSB); **opaque** fills (no level-based alpha).

## 3. Integration

- [x] 3.1 Replace `RadialOrbView` 2D draw path in `ofApp::draw` with the 3D sphere view fed by `meterDisplay`; keep title / RMS debug lines as appropriate.
- [x] 3.2 Remove or isolate unused 2D orb code paths; verify no duplicate symbols.

## 4. Verification

- [x] 4.1 `make` succeeds; run app and confirm orbit anchor, ortho look, 16 distinct hues, lightness responds to audio.
