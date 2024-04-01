// noinspection JSUnresolvedReference

"use strict";
import {
  appendFileSync,
  closeSync,
  existsSync,
  readFileSync,
  rmSync,
  openSync,
} from "node:fs";
import { EOL } from "node:os";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import protobuf from "protobufjs";

const { loadProtoFile } = protobuf;

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const STORAGE = loadProtoFile(resolve(__dirname, "data/grid.proto")).build(
  "GridProto.Storage",
);

if (process.argv.length <= 2) {
  // eslint-disable-next-line no-console -- node script
  console.error("Usage: node eval.mjs <data_file>");
  process.exit(1);
}
if (!existsSync(resolve(__dirname, `data/${process.argv[2]}`))) {
  // eslint-disable-next-line no-console -- node script
  console.error("File not found");
  process.exit(1);
} else if (!process.argv[2].endsWith(".json")) {
  // eslint-disable-next-line no-console -- node script
  console.error("File should not have .json extension");
  process.exit(1);
}

/**
 * @param {?Storage} storage
 * @returns {boolean}
 */
function filter(storage) {
  if (!storage || storage.width !== 9) return false;
  if (
    storage.entity.find((value) => {
      return value.type === 9 || value.type === 10;
    }) ||
    storage.entity.filter((value) => {
      return value.type === 3;
    }).length > 1 ||
    storage.entity.filter((value) => {
      return value.type === 4;
    }).length > 1
  )
    return false;
  if (storage.symmetry === 1) return false;
  /** @type {Entity[]} */
  const expanded = [];
  storage.entity.forEach((e) => {
    if (e.count !== 0)
      for (let i = 0; i < e.count; ++i) {
        expanded.push({ ...e, count: 0 });
      }
    else expanded.push(e);
  });
  if (Math.floor(expanded.length / storage.width) !== 9) return false;
  storage.entity = expanded;
  return true;
}

/** @type {Data[]} */
let data;
try {
  data = JSON.parse(
    readFileSync(resolve(__dirname, `data/${process.argv[2]}`), "utf8"),
  );
} catch (e) {
  // eslint-disable-next-line no-console -- node script
  console.error(e);
  process.exit(1);
}

/** @type {Set.<string>} */
const blacklist = new Set();
try {
  readFileSync(resolve(__dirname, "data/blacklist.txt"), "utf8")
    .split(EOL)
    .forEach((line) => {
      if (line) blacklist.add(line);
    });
} catch (e) {
  // ignore
}

/** @type {Map.<string, string>} */
const errata = new Map();
try {
  JSON.parse(
    readFileSync(resolve(__dirname, "data/errata.json"), "utf8"),
  ).forEach((value) => {
    errata.set(value.id, value.contents);
  });
} catch (e) {
  // ignore
}

const dist = resolve(
  __dirname,
  `data/${process.argv[2].replace(".json", ".txt")}`,
);
rmSync(dist, { force: true });
const fd = openSync(dist, "a+");
/** @type {Map.<string, Set.<string>>} */
const recorded = new Map();
data.forEach((value) => {
  if (value.upvotes >= 40 || blacklist.has(value.id)) return;
  const encoded = errata.has(value.id)
    ? errata.get(value.id)
    : value.contents.replace("_0", "");
  if (recorded.has(encoded)) {
    recorded.get(encoded).add(value.id);
    return;
  }
  /** @type {?Storage} */
  let storage = null;
  try {
    storage = STORAGE.decode64(encoded);
  } catch (_) {
    // ignore
  }
  if (!filter(storage)) return;
  appendFileSync(
    fd,
    `${value.id}/${value.createUtc}/${value.upvotes}/${JSON.stringify(storage)}\n`,
    "utf8",
  );
  recorded.set(encoded, new Set([value.id]));
});
closeSync(fd);
