$ErrorActionPreference = "Stop"
$data = "2022-11-29"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
cmake --fresh --preset=clang
cmake --build --preset=test
node "test\eval.mjs" "$data.json"
New-Item -Path "cmake-build-clang\test\data" -ItemType Directory -ErrorAction Ignore
Move-Item "test\data\$data.txt" -Destination "cmake-build-clang\test" -Force
Copy-Item "test\eval.py" -Destination "cmake-build-clang\test" -Force
Copy-Item "test\data\*.json" -Destination "cmake-build-clang\test\data" -Force
Set-Location -Path "cmake-build-clang\test"
.\eval "$data.txt"
..\..\.venv\Scripts\python eval.py
