$ErrorActionPreference = "Stop"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
Set-Location -Path "lib\emsdk"
.\emsdk.ps1 install latest
.\emsdk.ps1 activate latest
Set-Location -Path "..\.."
if (Test-Path -Path "dist") {
  Remove-Item -Path "dist" -Recurse
}
New-Item -Path "dist" -ItemType Directory
cmake -B dist -S . `
  -G Ninja `
  -DCMAKE_C_COMPILER="lib\emsdk\upstream\emscripten\emcc" `
  -DCMAKE_CXX_COMPILER="lib\emsdk\upstream\emscripten\em++" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_TOOLCHAIN_FILE="lib\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake"
cmake --build dist --target editor
if (Test-Path -Path "dist\src\editor.*" -PathType Leaf) {
  Copy-Item -Path "dist\src\editor.*" -Destination "..\..\apps\web\public\js"
  Copy-Item -Path "src\utils.js" -Destination "..\..\apps\web\public\js"
} else {
  Exit 1
}
