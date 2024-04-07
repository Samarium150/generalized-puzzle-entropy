set -e
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
git submodule update -f --depth 1 --recommend-shallow
python3 -m venv .venv
./.venv/bin/python3 -m pip install -r requirements.txt
cd lib/emsdk
./emsdk install latest
./emsdk activate latest
