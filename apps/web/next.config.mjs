// noinspection SpellCheckingInspection

/**
 * @param {string} _
 * @param {{ defaultConfig: import('next').NextConfig }} options
 * @return {import('next').NextConfig}
 */
export default (_, { defaultConfig }) => {
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
    basePath: "/generalized-puzzle-entropy",
    output: "export",
    transpilePackages: ["@repo/puzzle-editor"],
  }
}
