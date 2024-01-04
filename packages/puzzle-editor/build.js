const platform = require("node:os").platform;
const spawn = require("node:child_process").spawn;

function build() {
  switch (platform()) {
    case "win32":
      return spawn("powershell", [".\\build.ps1"], { stdio: "inherit" });
    case "linux":
      return spawn("bash", ["./build.sh"], { stdio: "inherit" });
    case "darwin":
      return spawn("zsh", ["./build.sh"], { stdio: "inherit" });
    default:
      console.log("unsupported platform");
      process.exit(1);
  }
}

build().on("error", () => {
  process.exit(1);
});
