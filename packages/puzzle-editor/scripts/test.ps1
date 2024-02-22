$ErrorActionPreference = "Stop"
$data = "2022-11-29"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
New-Item -Path "dist\data" -ItemType Directory
cmake -S . -B dist `
  --fresh `
  --preset=clang
cmake --build dist --clean-first
node "test\eval.mjs" "$data.json"
Move-Item "test\data\$data.txt" -Destination "dist\test"
Copy-Item "test\eval.py" -Destination "dist\test"
Copy-Item "test\data\*.json" -Destination "dist\test\date"
Set-Location -Path "dist\test"
.\eval.exe "$data.txt"
..\..\.venv\Scripts\python -m pip install -r requirements.txt
..\..\.venv\Scripts\python eval.py
