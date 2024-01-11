set -e
cd "$(git rev-parse --show-toplevel)/packages/puzzle-editor"
cd lib/emsdk
./emsdk install latest
./emsdk activate latest
cd ../../
if [ -d "dist" ]; then
  rm -rf dist
fi
mkdir -p dist
cmake -B dist -S . \
  -DCMAKE_C_COMPILER=lib/emsdk/upstream/emscripten/emcc \
  -DCMAKE_CXX_COMPILER=lib/emsdk/upstream/emscripten/em++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=lib/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
cmake --build dist --target editor
if ls dist/src/editor.* >/dev/null 2>&1; then
  cp dist/src/editor.* ../../apps/web/public/js/
  cp src/utils.js ../../apps/web/public/js/utils.js
else
  exit 1
fi
