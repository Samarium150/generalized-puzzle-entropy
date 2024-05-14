set -e
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
cmake --fresh --preset=emscripten
cmake --build --preset=src
if ls cmake-build-emscripten/src/editor.* >/dev/null 2>&1; then
  cp cmake-build-emscripten/src/editor.js ../../apps/web/public/js/
  cp cmake-build-emscripten/src/playground.js ../../apps/web/public/js/
  cp cmake-build-emscripten/src/*.wasm ../../apps/web/public/js/
else
  exit 1
fi
