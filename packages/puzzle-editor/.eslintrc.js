module.exports = {
  extends: ["custom/react-internal", "plugin:prettier/recommended"],
  overrides: [{
    files: ["src/**/*.{ts,tsx,js,jsx}"],
  }]
};
