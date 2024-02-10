set -e
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
mkdir -p dist
cmake -S . -B dist \
  --fresh \
  --preset=clang
cmake --build dist --clean-first
node test/eval.mjs 2022-11-29.json
mv test/data/2022-11-29.txt dist/test/
cp test/plot.py requirements.txt dist/test/
python3 -m venv dist/test/venv
cd dist/test
chmod +x eval
./eval 2022-11-29.txt
./venv/bin/python3 -m pip install -r requirements.txt --upgrade pip
./venv/bin/python3 plot.py
