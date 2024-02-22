set -e
data="2022-11-29"
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
mkdir -p dist/data
cmake -S . -B dist \
  --fresh \
  --preset=clang
cmake --build dist --clean-first
node test/eval.mjs ${data}.json
mv test/data/${data}.txt dist/test/
cp test/eval.py dist/test/
cp test/data/*.json dist/test/data
cd dist/test
chmod +x eval
./eval ${data}.txt
../../.venv/bin/python3 eval.py
