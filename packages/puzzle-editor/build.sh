set -e
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
cd lib/emsdk
./emsdk install latest
./emsdk activate latest
cd ../../
mkdir -p dist
cmake -S . -B dist \
  --fresh \
  --preset=emscripten
cmake --build dist --clean-first
if ls dist/src/editor.* >/dev/null 2>&1; then
  cp dist/src/*.js ../../apps/web/public/js/
  cp dist/src/*.wasm ../../apps/web/public/js/
else
  exit 1
fi
