import { defineShikiSetup } from "@slidev/types";
import type { ShikiSetup } from "@slidev/types";

// noinspection JSUnusedGlobalSymbols
export default defineShikiSetup((() => {
  // noinspection SpellCheckingInspection
  return {
    theme: {
      dark: "solarized-dark",
      light: "solarized-light",
    },
  };
}) as ShikiSetup);
