// noinspection SpellCheckingInspection

/** @type {import('next').NextConfig} */
const nextConfig = (module.exports = {
  reactStrictMode: true,
  transpilePackages: ["puzzle-editor"],
  headers: async () => {
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
      },
    ];
  },
});

module.exports = nextConfig;
