// @ts-check
// noinspection SpellCheckingInspection

/**
 * @param {string} phase
 * @param {{ defaultConfig: import('next').NextConfig }} options
 * @return {import('next').NextConfig}
 */
export default (phase, { defaultConfig }) => {
  return {
    ...defaultConfig,
    async headers() {
      return [
        {
          source: "/(.*)",
          headers: [
            {
              key: "Cross-Origin-Embedder-Policy",
              value: "require-corp",
            },
            {
              key: "Cross-Origin-Opener-Policy",
              value: "same-origin",
            },
          ],
        }];
    },
    transpilePackages: ["@repo/puzzle-editor"],
  }
}
