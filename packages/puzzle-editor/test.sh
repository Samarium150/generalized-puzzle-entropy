set -e
data="2022-11-29"
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
mkdir -p dist
cmake -S . -B dist \
  --fresh \
  --preset=clang
cmake --build dist --clean-first
node test/eval.mjs ${data}.json
mv test/data/${data}.txt dist/test/
cp test/eval.py requirements.txt test/data/*.json dist/test/
python3 -m venv dist/test/venv
cd dist/test
chmod +x eval
./eval ${data}.txt
./venv/bin/python3 -m pip install -r requirements.txt --upgrade pip
./venv/bin/python3 eval.py
