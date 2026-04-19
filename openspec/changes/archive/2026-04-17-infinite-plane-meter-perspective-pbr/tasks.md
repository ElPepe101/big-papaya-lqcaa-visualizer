## 1. Module and draw path

- [x] 1.1 Add **`PlaneMeterView`** (or equivalent) with **`setupPlaneMeterView`** / **`drawPlaneMeterPBR`** / **`drawPlaneMeterSky`** (or shared env draw) mirroring the **sphere** lifecycle.
- [x] 1.2 Wire **`ofApp`**: replace **`drawSphereMeterPBR`** / **`drawSphereMeterSky`** calls with **plane** equivalents; remove **sphere-only** UI strings (**UV unfold** text) or replace with **plane** debug copy.
- [x] 1.3 Switch **`orbitCam_`** (or camera helper) to **perspective** with **documented** **FOV**, **near/far**, and **anchor**; keep **+Y** up and **no roll**.

## 2. Mesh, UV, and logging

- [x] 2.1 Build **square** **N×N** quad grid plane (**N % 16 == 0**), **UV** consistent with **bread** **square** maps; choose **default N** (≥ **512**, push **1024+** if stable) and **document** in **`design.md`** resolution if changed from default.
- [x] 2.2 On setup, **log** **N**, **quad count**, **vertex count** (or triangle count).

## 3. Shaders (PBR + HDRI + VS displacement)

- [x] 3.1 Add **`bread_plane.vert`** / **`bread_plane.frag`** (or names per repo convention): **tangent-space** or **world** normals for **flat** geometry, **IBL** sampling consistent with existing **env** helpers, **fallback** path if **HDRI** missing.
- [x] 3.2 **VS**: sample **`displacement.png`** and apply **audio blended field** along **plane normal**; **FS**: **no** **`displacement.png`** used for **audio** geometry offsets (**no** parallax for metering).
- [x] 3.3 Port **per-channel hue / roughness / level** modulation behavior to the **plane** so **quiet vs loud** remains readable **without alpha metering**.

## 4. Blended field and infinite look

- [x] 4.1 Implement **smooth weight functions** \(w_c(u,v)\) so **multiple** channels affect each **(u,v)**; tune so **4×4** **walls** are **not** dominant at default gains.
- [x] 4.2 Add **fog / haze** (and/or **peripheral** color fade) plus **sky** draw order so the plane reads **open** toward the **horizon** at **default** camera; **document** chosen formula in **code comment** or **`design.md`**.
- [x] 4.3 Verify **large** plane **extent** and **camera limits** so **mesh edges** are **rarely** the first failure mode; adjust **extent** or **fog** if **edges** pop during **orbit**.

## 5. Debug and cleanup

- [x] 5.1 Replace or repurpose **`[u]`** debug: **grid** or **weight** overlay appropriate to **plane** (remove **sphere unfold** if obsolete).
- [x] 5.2 Remove **dead** **sphere** draw path, **unused** uniforms, and **duplicate** **texture** loads once **plane** path is **verified** (**Release** `make`, run app, **no GL errors**).

## 6. Verification

- [x] 6.1 **`make` Release**; run with **`bread/`** + **`env.hdr`** present: confirm **PBR**, **reflections**, and **displacement** read correctly under **perspective**.
- [x] 6.2 **16ch** input sanity: **channel isolation** still **discernible** under **blend**; **no** **alpha** level metering.
- [x] 6.3 After implementation, run **`openspec validate`** (or project equivalent) and prepare change for **`/opsx-archive`** with **spec sync** for **`plane-3d-channel-display`** and **`sphere-3d-channel-display`** deltas.
