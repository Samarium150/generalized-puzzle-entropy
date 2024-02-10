// noinspection JSUnresolvedReference

"use strict";
/* eslint-disable -- external utilities */
const BASE_URL = "https://windmill.thefifthmatt.com/build/";

const STORAGE =
  dcodeIO.ProtoBuf.loadProtoFile("proto/grid.proto").build("GridProto.Storage");

function _export() {
  const message = document.getElementById("message");
  if (message) {
    const text = message.innerHTML;
    try {
      message.innerHTML = `${BASE_URL}${STORAGE.decodeJSON(text)
        .encode64()
        .replace(/\//g, "_")
        .replace(/\+/g, "-")}_0`;
    } catch (e) {
      message.innerHTML = "";
      console.error(e);
      window.alert("cannot encode message");
    }
  }
}

async function _load() {
  const text = await navigator.clipboard
    .readText()
    .then((content) => {
      return content
        .replace(BASE_URL, "")
        .replace("_0", "")
        .replace(/-/g, "+")
        .replace(/_/g, "/");
    })
    .catch((err) => {
      console.error(err);
      window.alert("cannot read from clipboard");
    });
  if (text) {
    try {
      const message = document.getElementById("message");
      if (message) message.innerHTML = JSON.stringify(STORAGE.decode64(text));
    } catch (e) {
      message.innerHTML = "";
      console.error(e);
      window.alert("cannot decode clipboard contents");
    }
  }
}
/* eslint-enable -- external utilities */

const message = document.getElementById("message");
if (message)
  message.addEventListener("keydown", async (event) => {
    switch (event.key) {
      case "+":
        await _load();
        break;
      case "-":
        _export();
        break;
      default:
        break;
    }
  });
