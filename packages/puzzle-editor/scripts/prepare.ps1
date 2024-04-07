$ErrorActionPreference = "Stop"
Set-Location -Path "$(git rev-parse --show-toplevel)\packages\puzzle-editor"
git submodule update -f --depth 1 --recommend-shallow
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
Set-Location -Path "lib\emsdk"
.\emsdk.ps1 install latest
.\emsdk.ps1 activate latest
