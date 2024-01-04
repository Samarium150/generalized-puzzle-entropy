module.exports = {
  extends: [
    "custom/next",
    "plugin:@next/next/core-web-vitals",
    "plugin:prettier/recommended",
    "plugin:tailwindcss/recommended",
  ],
  overrides: [
    {
      files: ["src/app/**/*.{ts,tsx}"],
    },
  ],
};
