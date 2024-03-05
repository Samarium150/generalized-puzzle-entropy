"use strict";
import { spawn } from "node:child_process";
import { existsSync } from "node:fs";
import { platform } from "node:os";
import { dirname } from "node:path";
import { fileURLToPath } from "node:url";

if (process.argv.length <= 2) {
  // eslint-disable-next-line no-console -- node script
  console.error("Usage: node run.mjs <task>");
  process.exit(1);
}

process.chdir(dirname(fileURLToPath(import.meta.url)));

function run(task) {
  let file;
  switch (platform()) {
    case "win32":
      file = `${task}.ps1`;
      if (existsSync(file))
        return spawn("powershell", [`.\\${task}.ps1`], {stdio: "inherit"});
      break;
    case "linux":
      file = `${task}.sh`;
      if (existsSync(file))
        return spawn("bash", [`${task}.sh`], {stdio: "inherit"});
      break;
    case "darwin":
      file = `${task}.sh`;
      if (existsSync(file))
        return spawn("zsh", [`${task}.sh`], {stdio: "inherit"});
      break;
    default:
      throw new Error("unsupported platform");
  }
  throw new Error("task file does not exist");
}

run(process.argv[2]).on("error", () => {
  process.exit(1);
});
