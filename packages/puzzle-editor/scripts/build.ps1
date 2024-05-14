$ErrorActionPreference = "Stop"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
cmake --fresh --preset=emscripten
cmake --build --preset=src
if (Test-Path -Path "cmake-build-emscripten\src\editor.*" -PathType Leaf) {
  Copy-Item -Path "cmake-build-emscripten\src\editor.js" -Destination "..\..\apps\web\public\js"
  Copy-Item -Path "cmake-build-emscripten\src\playground.js" -Destination "..\..\apps\web\public\js"
  Copy-Item -Path "cmake-build-emscripten\src\*.wasm" -Destination "..\..\apps\web\public\js"
} else {
  Exit 1
}
