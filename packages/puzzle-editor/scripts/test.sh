set -e
data="2022-11-29"
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
cmake --fresh --preset=clang
cmake --build --preset=test
node test/eval.mjs ${data}.json
mkdir -p cmake-build-clang/test/data
mv test/data/${data}.txt cmake-build-clang/test/
cp test/eval.py cmake-build-clang/test/
cp test/data/*.json cmake-build-clang/test/data
cd cmake-build-clang/test
chmod +x eval
./eval ${data}.txt
../../.venv/bin/python3 eval.py
