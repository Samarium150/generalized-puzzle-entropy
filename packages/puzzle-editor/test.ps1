$ErrorActionPreference = "Stop"
$data = "2023-08-15"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
New-Item -Path "dist" -ItemType Directory
cmake -S . -B dist `
  --fresh `
  --preset=clang
cmake --build dist --clean-first
node test/eval.mjs "$data.json"
Move-Item "test\data\$data.txt" -Destination "dist\test"
Copy-Item "test\plot.py" "requirements.txt" -Destination "dist\test"
python -m venv dist\test\venv
Set-Location -Path "dist\test"
.\eval.exe "$data.txt"
.\venv\Scripts\python -m pip install -r requirements.txt
.\venv\Scripts\python plot.py
