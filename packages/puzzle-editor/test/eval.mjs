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
if (!existsSync(resolve(__dirname, "data/", process.argv[2]))) {
  // eslint-disable-next-line no-console -- node script
  console.error("File not found");
  process.exit(1);
} else if (!process.argv[2].endsWith(".json")) {
  // eslint-disable-next-line no-console -- node script
  console.error("File should not have .json extension");
  process.exit(1);
}

/**
 * @param {Storage} storage
 * @returns boolean
 */
function filter(storage) {
  if (storage.width !== 9) return false;
  if (
    storage.entity.find((value) => {
      return value.type >= 8;
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
  /** @type Entity[] */
  const expanded = [];
  storage.entity.forEach((e) => {
    if (e.count !== 0) {
      for (let i = 0; i < e.count; i++) {
        expanded.push({ ...e, count: 0 });
      }
    } else {
      expanded.push(e);
    }
  });
  if (Math.floor(expanded.length / storage.width) !== 9) return false;
  storage.entity = expanded;
  return true;
}

/** @type Data[] */
let data;
try {
  data = JSON.parse(
    readFileSync(resolve(__dirname, "data/", process.argv[2]), "utf8"),
  );
} catch (e) {
  // eslint-disable-next-line no-console -- node script
  console.error(e);
  process.exit(1);
}
const dist = resolve(
  __dirname,
  "data/",
  process.argv[2].replace(".json", ".txt"),
);
if (existsSync(dist)) rmSync(dist, { force: true });
const fd = openSync(dist, "a+");
/** @type Set<string> */
const recorded = new Set();

for (let i = 0; i < data.length; ++i) {
  const encoded = data[i].contents.replace("_0", "");
  /** @type Storage */
  let storage;
  try {
    storage = STORAGE.decode64(encoded);
  } catch (_) {
    // ignore
  }
  if (!storage) continue;
  if (data[i].upvotes >= 40 || recorded.has(encoded) || !filter(storage))
    continue;
  recorded.add(data[i].id);
  appendFileSync(
    fd,
    `${data[i].id}/${JSON.stringify(storage)}/${data[i].upvotes}/${data[i].solves}\n`,
    "utf8",
  );
}
closeSync(fd);
