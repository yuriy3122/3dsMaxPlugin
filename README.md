<p align="center">
  <img src="https://github.com/yuriy3122/Ryze-Studio/blob/main/logo.jpeg" />
</p>

# 3ds Max 2022 Scene Exporter

> A production‑oriented 3ds Max **SceneExport** plug‑in that exports node hierarchy, meshes, and materials into a compact custom format (`.rz`).

## ✨ Features

- **Hierarchy**: traverses the scene graph via `INode` (preserves parent/child relationships and transforms).
- **Geometry**: converts objects to `TriObject`, exports indexed triangle meshes.
- **Shading data**: vertex **normals**, **tangents**, and **bitangents** (where available / derivable).
- **UVs**: reads map channels for texture coordinates.
- **Materials**: supports **Multi/Sub‑Object** materials and resolves per‑face material IDs to leaf sub‑materials.
- **User‑defined properties**: reads the Max **User Properties** buffer and writes it alongside nodes/meshes.
- **Skin awareness**: detects `ISkin` so skinned nodes can be handled (export path present, retargeting optional).
- **Unicode‑safe I/O helpers**: text writer supports UTF‑8 / UTF‑16 encodings for any sidecar text data.
- **x64 build** for modern Max; MSVC toolset **v141**.

## 📁 Project layout

```
- maxExport.cpp  — plugin implementation (SceneExport, traversal, writing)
- maxExport.h    — class declaration and SceneExport overrides
- maxExport.rc   — resources (about box, strings)
- maxExport.def  — exports (LibDescription, etc.)
- maxExport.vcxproj(.filters/.user) — MSBuild project (x64, Debug/Hybrid/Release)
- resource.h     — resource IDs
- expDefines.h   — string/resource/ID defines
- maxExport.aps  — resource compiler data
```

## 🏗️ Build

1. **Prerequisites**
   - **Visual Studio 2017+** (toolset **v141** works; newer MSVC toolsets are typically fine).
   - **3ds Max 2022 SDK** for your target Max version (set *environment variables* as below).
   - Target architecture: **x64**.

2. **Environment**
   - Define `MaxSdkInc` to point at the SDK include directory (the project uses `$(MaxSdkInc)`).
   - If your SDK also needs lib paths, you can add a `MaxSdkLib` user macro and wire it in **Project → Properties → Linker → Additional Library Directories**.

3. **Configuration**
   - Build configs: **Debug, Hybrid, Release**.
   - Output: `maxexp.dle` (per `maxExport.def`).

4. **Compile**
   - Open `maxExport.sln` or `maxExport.vcxproj` in Visual Studio.
   - Select **x64** + **Release**, then **Build**.

## 📦 Install

1. Close 3ds Max.
2. Copy the built `maxexp.dle` to your 3ds Max *plugins* folder for the **matching Max version** (e.g. `C:\Program Files\Autodesk\3ds Max <YEAR>\Plugins`).
3. Start 3ds Max; confirm the plug‑in is loaded in **Customize → Plug‑in Manager** (look for *maxExport*).

## ▶️ Use

1. In 3ds Max: **File → Export… / Export Selected…**
2. Choose **rz** from the file type drop‑down (extension: `.rz`).
3. Pick options (if available), then export.
4. The exporter walks the scene, captures node transforms, meshes, materials, UVs, normals/tangents, and the **User Properties** buffer.

> The exporter writes **binary** data. If you need a text/JSON dump for quick inspection, see the *Developer Notes* below for a small diagnostic function suggestion.

## 🧭 Roadmap / nice‑to‑haves

- **Geometry compression**: integrate **LZ4** (frame API) or **Zstd** at the chunk level. Insert after vertex/index buffers are serialized.
- **Progress & cancel**: hook 3ds Max `Interface::ProgressStart/End/Update` for responsive long exports.
- **Smoothing & explicit normals**: ensure `MeshNormalSpec` is respected when present; fall back to computed normals otherwise.
- **Tangent basis**: validate MikkTSpace equivalence if a renderer requires a specific basis.
- **Instancing**: detect duplicate meshes via geometry hashing and emit instances to shrink file size.
- **Materials**: export `Texmap` graphs or bake a compact material description (IDs, parameters, map filenames).
- **Skin/anim**: if you need animation, extend to export `ISkin` weights and optional transform tracks.
- **MaxScript/MXS entrypoint**: expose a function like `maxExport.export fileName:<path> selected:<bool>` for batch pipelines.
- **CMake**: add a CMake toolchain for multi‑SDK, multi‑MSVC builds.
- **CI**: GitHub Actions matrix building against several 3ds Max SDKs (cache SDK paths via secrets).

## 🧩 Developer notes

- **Entry points** come from `maxExport.def` (`LibDescription`, etc.).
- The `SceneExport` subclass is **MaxExport**.
- Reported file extension: `.rz` (via `Ext(int)` override).
- Project defines: `_SECURE_SCL=0`, `_CRT_NON_CONFORMING_SWPRINTFS`.
- Target name: **maxexport** → `maxexp.dle` per `.def` file.

### Minimal diagnostic dumper (optional)

If you need a quick human‑readable dump for debugging, wrap your binary writer with a `#ifdef DEBUG_DUMP` and write a `.txt` next to the output containing hierarchy and mesh stats (vertex/face counts, material names, map channels, user‑props size). This is helpful in PR reviews.

## 📄 License

If this is an open‑source portfolio sample, consider adding a permissive license (MIT/BSD‑2‑Clause). *No license file was found in this snapshot.*
