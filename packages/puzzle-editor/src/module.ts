/// <reference types="emscripten" />
// noinspection JSUnresolvedReference

interface EditorModule extends EmscriptenModule {
  // noinspection SpellCheckingInspection
  cwrap: typeof cwrap;
}

declare const ModuleFactory: EmscriptenModuleFactory<EditorModule>;

export async function getModule(): Promise<EditorModule> {
  /* eslint-disable @typescript-eslint/no-explicit-any, @typescript-eslint/no-unsafe-member-access -- load WASM */
  if (typeof (window as any).EditorModule !== "undefined") {
    return ((window as any).EditorModule as typeof ModuleFactory)();
  }
  /* eslint-enable @typescript-eslint/no-explicit-any, @typescript-eslint/no-unsafe-member-access -- load WASM */
  return Promise.reject(new Error("WASM module not found"));
}
