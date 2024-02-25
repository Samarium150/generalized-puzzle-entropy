/** @type {import("eslint").Linter.Config} */
module.exports = {
  root: true,
  extends: ["@repo/eslint-config/react-internal.js"],
  parser: "@typescript-eslint/parser",
  parserOptions: {
    project: true,
  },
  env: {
    browser: true,
    node: true,
  },
  ignorePatterns: ["cmake-build-*", "lib"],
  rules: {
    "no-undef": "off",
    "no-unused-vars": "off",
    "turbo/no-undeclared-env-vars": "off",
  },
};
