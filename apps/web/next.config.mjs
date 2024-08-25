// noinspection SpellCheckingInspection

/**
 * @param {string} _
 * @param {{ defaultConfig: import('next').NextConfig }} options
 * @return {import('next').NextConfig}
 */
export default (_, { defaultConfig }) => {
  /** @type {import('next').NextConfig} */
  const config = {
    ...defaultConfig,
    transpilePackages: ["@repo/puzzle-editor"],
  };
  return process.env.NODE_ENV === "production"
    ? {
        ...config,
        basePath: process.env.NEXT_PUBLIC_BASE_PATH,
        output: "export",
      }
    : {
        ...config,
        async headers() {
          return process.env.NODE_ENV === "production"
            ? []
            : [
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
                },
              ];
        },
      };
};
