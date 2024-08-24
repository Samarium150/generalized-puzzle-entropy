# Generalized Puzzle Entropy

## Getting Started

### Prerequisites

Before cloning the repository, make sure the following environments are
installed and available in the system path of your machine:

- Local compiling:
  - CMake >= 3.24.0
  - LLVM >= 16.0.0
  - Ninja >= 1.10.0
- Serving the demo:
  - Node.js >= 18.17.0
  - pnpm >= 8.0.0
- Plotting the results:
  - Python >= 3.8.0

Project configurations for Jetbrains IDEs, like CLion and WebStorm,
are provided in the [`.idea`](/.idea) directory.
This repository and the [`packages/puzzle-editor`](/packages/puzzle-editor) subdirectory
can be opened as projects in these IDEs.

### Installation

Clone the repository and run `pnpm install` in the root directory.
It should take a while because of the size of the libraries it relies on.

### Serving the Demo

```shell
pnpm run dev
```

### Evaluation

```shell
cd packages/puzzle-editor
pnpm run test
```

## Useful Links

- [AntDesign](https://ant.design)
- [Emscripten](https://emscripten.org)
- [LLVM](https://llvm.org) 
- [Next.js](https://nextjs.org)
- [React](https://react.dev/)
- [Turborepo](https://turbo.build/repo)

## License

Copyright ©️ 2024 Samarium

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.txt).
