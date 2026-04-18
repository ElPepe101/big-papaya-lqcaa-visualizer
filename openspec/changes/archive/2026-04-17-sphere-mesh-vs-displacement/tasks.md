## 1. Mesh density

- [x] 1.1 Set `kUvGridRes` (square **N×N**) to the chosen default tier (**≥ 1024**, target **1536** or **2048** if memory allows); keep `static_assert(N % 16 == 0)`.
- [x] 1.2 Confirm `buildSphereMesh` still uses **square** UV corner **(u,v)** (`j/N`, `(j+1)/N`, `strip/N`, `(strip+1)/N`) unchanged in structure.
- [x] 1.3 Ensure setup log prints **N**, **quad count N²**, and **vertex count** (or `getNumVertices()`).

## 2. Vertex shader displacement only

- [x] 2.1 Audit `bread_sphere.frag`: confirm **no** `displacement` sampler and **no** parallax / height ray-march for geometry; document in a one-line comment if needed.
- [x] 2.2 Audit `bread_sphere.vert`: confirm **bread** height and **audio** meter offset remain the **only** geometric displacements before `gl_Position`.
- [x] 2.3 After mesh bump, visually tune **`uDispAmp`** / **`uDispTexScale`** (C++ constants) if silhouettes look too harsh or self-intersecting.

## 3. Debug and docs

- [x] 3.1 Verify UV unwrap overlay (`drawSphereMeterUnfoldBody`) still aligns with **N** (grid line count / major lines acceptable at high **N**).
- [x] 3.2 If unfold is too heavy at high **N**, subsample vertical/horizontal debug lines (document in code comment).

## 4. Verification

- [x] 4.1 `make` Release; run app; orbit sphere and confirm **no** GL errors and acceptable **FPS** on target hardware.
- [x] 4.2 If default **N** causes OOM or stutter, lower default and record the value in `design.md` **Open Questions** resolution.
