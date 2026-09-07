# DirectX 11 Terrain Tool

Keyboard-driven terrain sculpting MVP based on DirectXProj_Ver2.

For a beginner-friendly explanation of the architecture, execution pipeline, sculpting pipeline, and recommended code-review order, read [Docs_TERRAIN_TOOL.md](Docs_TERRAIN_TOOL.md).

## Controls

- `1`, `2`, `3`: Raise/Lower, Flatten, Smooth
- `LMB`: sculpt, `Shift+LMB`: invert Raise/Lower
- `[`, `]`: brush radius, `-`, `=`: brush strength
- `RMB`: rotate, `MMB`: pan, mouse wheel: zoom, `Alt+LMB`: orbit
- `Ctrl+S`, `Ctrl+O`, `Ctrl+N`: save, load, new terrain
- `Ctrl+Z`, `Ctrl+Y`: undo, redo

Terrain data is stored at `Assets/Terrain/default.terrain.json` relative to the working directory.

Build `DirectXProj.sln` with Visual Studio 2022 using the Debug x64 configuration. Run with `TerrainTool/DirectXProj` as the working directory so shaders, textures, fonts, and terrain data resolve consistently.
