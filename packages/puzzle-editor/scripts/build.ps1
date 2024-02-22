$ErrorActionPreference = "Stop"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
New-Item -Path "dist" -ItemType Directory
cmake -S . -B dist `
  --fresh `
  --preset=emscripten
cmake --build dist --clean-first
if (Test-Path -Path "dist\src\editor.*" -PathType Leaf) {
  Copy-Item -Path "dist\src\*.js" -Destination "..\..\apps\web\public\js"
  Copy-Item -Path "dist\src\*.wasm" -Destination "..\..\apps\web\public\js"
} else {
  Exit 1
}
