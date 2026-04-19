## Context

**LQCAA** currently implements the **primary** meter as **`PlaneMeterView`**: **XZ** **ground** **plane**, **`bread_plane`** **PBR**, **HDRI** **sky**, **Gaussian-blended** **16-channel** **weights**, **perspective** **orbit**. The product asks for a **starker** **foundation**: **no** **bread**, a **vertical** **wall** **fixed** in **world** **space**, **grid** **readable** in **3D**, **noise**-driven **motion** **along** **normals**, and **flexible** **grayscale** **mapping** for **levels**.

## Goals / Non-Goals

**Goals:**

- **World-fixed wall**: **Single** **quad** **mesh** (or **grid** of **quads**) on a **vertical** **plane** (implementation default: **XY** **plane** at **z = 0** or a **documented** **offset**, **normal** toward **+Z** or **−Z** toward **default** **camera** **hemisphere**).
- **Row ↔ channel**: **16** **horizontal** **row** **bands** (one **primary** **channel** **per** **band** in **row-major** **order** **top-to-bottom** or **bottom-to-top**, **documented**); **each** **band** has **many** **column** **subdivisions** (**M ≫ 1**) so the **grid** is **fine** **horizontally**.
- **VS-only** **displacement**: **Offset** **along** **geometric** **normal** **only** (no **FS** **parallax** **for** **metering** **geometry**). **Offset** = **amplitude** **×** **smooth** **noise** **function** **f(time, u, v, channelPhase_c)** with **distinct** **phase** **per** **channel** **c** (e.g. **offset** **in** **1D** **noise** **sampling** or **per-row** **constant** **added** to **time**).
- **Darkness** **modes** (**both** **in** **one** **change**): **user** **switchable** (key or **settings**) **at** **minimum**:
  - **Spatial** **variant**: **per-cell**/**per-vertex** **luma** **within** **a** **row** **band** **vs** **row-uniform** **luma** (**whole** **row** **shares** **one** **level** **visualization**).
  - **Encoding** **variant**: **linear** **RMS** **→** **luma** **vs** **log/dB**-style **mapping** **consistent** **with** **existing** **`rmsToLitColumnCount`** **philosophy** (reuse **constants** **where** **sensible**).
- **Opaque** **alpha**; **no** **level-as-transparency**.
- **Audio** **path** **unchanged** (**16** **smoothed** **values** **in**).

**Non-Goals:**

- **Bread** **textures**, **normal** **maps**, **roughness**, **displacement.png**, **HDRI** **as** **required** **for** **the** **meter** (optional **neutral** **background** **clear** **color** **only**).
- **Blended** **Gaussian** **overlap** **between** **channels** **across** **rows** **in** **v1** (**hard** **row** **ownership** **unless** **spec** **expands** **later**).

## Decisions

1. **Wall** **orientation**  
   - **Decision**: **Vertical** **wall** **in** **world** **space** (**XY**), **Z** **normal**; **camera** **orbits** **origin** **or** **wall** **center** **with** **+Y** **up**, **no** **roll**.  
   - **Rationale**: Matches **“world-fixed wall”** **and** **keeps** **existing** **orbit** **code** **pattern** **with** **tuned** **defaults** **to** **face** **the** **wall**.

2. **Noise** **implementation**  
   - **Decision**: **2D** **periodic** **smooth** **noise** **in** **VS** (e.g. **sin** **sum** **or** **simple** **hash**-based **value** **noise**) **parameterized** **by** **uv** **and** **time**, **plus** **per-channel** **phase** **uniform** **`uChannelPhase[16]`** **or** **`uTime` + `c * kPhaseStride`**.  
   - **Alternative** **rejected** **for** **v1**: **CPU** **noise** **texture** **upload** **each** **frame** (**heavier**).

3. **Finer** **grid**  
   - **Decision**: **Constants** **`kChannelRows = 16`**, **`kColsPerRow`** **≥** **32`** **(default** **64** **or** **128`**, **TBD** **by** **perf)**; **total** **quads** **=** **16** **×** **`kColsPerRow`**.

4. **Darkness** **“both** **options”**  
   - **Decision**: **Expose** **two** **booleans** **or** **enum**: **`DarknessSpatialMode`**: **PerCell** | **RowUniform**; **`LevelMappingMode`**: **LinearRms** | **DbMapped** (names **in** **code** **may** **differ**). **Default** **one** **pair** **for** **first** **launch**; **second** **pair** **reachable** **without** **recompile** (e.g. **`[`** **`]`** **or** **`d`** **key** **cycle**).

5. **PBR** **removal**  
   - **Decision**: **Delete** **hot-path** **loads** **of** **`bread/`** **and** **`env.hdr`** **in** **meter** **module**; **ofBackground** **solid** **or** **subtle** **gradient** **optional**.

## Risks / Trade-offs

- **[Risk]** **Z-fighting** **with** **wall** **at** **large** **distance** → **Mitigation**: **polygon** **offset** **or** **camera** **near** **plane** **tuning**.  
- **[Risk]** **Row** **band** **visibility** **vs** **noise** **amplitude** → **Mitigation**: **separate** **uniforms** **for** **motion** **gain** **and** **luma** **gain**.  
- **[Risk]** **Spec** **churn** **on** **`plane-3d-channel-display`** → **Mitigation**: **delta** **REMOVED** **with** **clear** **Migration** **links**.

## Migration Plan

1. **Implement** **wall** **path** **side-by-side** **briefly** **if** **needed**, **then** **swap** **`ofApp`** **draw**.  
2. **Remove** **`bread_plane`** **and** **unused** **assets** **from** **build** **after** **verification**.  
3. **Rollback**: **git** **revert** **to** **prior** **commit** **with** **plane** **meter**.

## Open Questions

- **Exact** **default** **`kColsPerRow`** **and** **wall** **width/height** **in** **world** **units**.  
- Whether **grid** **lines** **are** **drawn** **in** **FS** **(procedural)** **or** **wireframe** **overlay** **pass**.
