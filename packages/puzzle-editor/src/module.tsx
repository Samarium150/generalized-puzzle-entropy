/// <reference types="emscripten" />
// noinspection JSUnresolvedReference

"use client";
import { forwardRef, memo, type Ref, useEffect, useRef } from "react";

import "./module.css";

interface Module extends EmscriptenModule {
  // noinspection SpellCheckingInspection
  cwrap: typeof cwrap;
}

declare const ModuleFactory: EmscriptenModuleFactory<Module>;

export class ParsedModule {
  constructor(
    readonly init: () => null = () => null,
    readonly frame: () => string = () => "",
    readonly mouseEvent: (
      x: number,
      y: number,
      up: boolean,
      down: boolean,
      drag: boolean,
    ) => null = () => null,
    readonly keyEvent: (key: string) => null = () => null,
    readonly setCanvas: (width: number, height: number) => null = () => null,
  ) {}

  static from = (module: Module): ParsedModule => {
    return new ParsedModule(
      module.cwrap("InitHOG", null, []),
      module.cwrap("DoFrame", "string", []),
      module.cwrap("MouseEvent", null, [
        "number",
        "number",
        "boolean",
        "boolean",
        "boolean",
      ]),
      module.cwrap("HitKey", null, ["string"]),
      module.cwrap("SetCanvasSize", null, ["number", "number"]),
    );
  };
}

export async function getModule(name: string): Promise<ParsedModule> {
  /* eslint-disable @typescript-eslint/no-explicit-any, @typescript-eslint/no-unsafe-member-access -- load WASM */
  if (typeof (window as any)[name] !== "undefined") {
    const factory = (window as any)[name] as typeof ModuleFactory;
    /* eslint-enable @typescript-eslint/no-explicit-any, @typescript-eslint/no-unsafe-member-access -- load WASM */
    return factory().then(ParsedModule.from);
  }
  return Promise.reject(new Error("WASM module not found"));
}

interface ModuleProps {
  readonly width: number;
  readonly height: number;
  readonly module: ParsedModule;
}

function getHTMLClientRect(): DOMRect {
  return document.getElementsByTagName("html")[0].getBoundingClientRect();
}

export const WASMModule = memo(
  forwardRef((props: ModuleProps, messageRef: Ref<HTMLDivElement>) => {
    const picture = useRef<HTMLDivElement>(null);
    const bg = useRef<HTMLCanvasElement>(null);
    const fg = useRef<HTMLCanvasElement>(null);
    const down = useRef(false);
    const elementOffsetX = useRef(0);
    const elementOffsetY = useRef(0);

    useEffect(() => {
      const mouseDown = (event: { clientX: number; clientY: number }): void => {
        down.current = true;
        const x = event.clientX + window.scrollX - elementOffsetX.current;
        const y = event.clientY + window.scrollY - elementOffsetY.current;
        props.module.mouseEvent(x, y, false, true, false);
      };

      const mouseUp = (event: { clientX: number; clientY: number }): void => {
        down.current = false;
        const x = event.clientX + window.scrollX - elementOffsetX.current;
        const y = event.clientY + window.scrollY - elementOffsetY.current;
        props.module.mouseEvent(x, y, true, false, false);
      };

      const mouseDrag = (event: {
        isPrimary: boolean;
        clientX: number;
        clientY: number;
      }): void => {
        if (event.isPrimary && down.current) {
          const x = event.clientX + window.scrollX - elementOffsetX.current;
          const y = event.clientY + window.scrollY - elementOffsetY.current;
          props.module.mouseEvent(x, y, false, true, true);
        }
        if (!down.current) {
          const x = event.clientX + window.scrollX - elementOffsetX.current;
          const y = event.clientY + window.scrollY - elementOffsetY.current;
          props.module.mouseEvent(x, y, false, false, true);
        }
      };

      const doFrame = (): void => {
        props.module.frame();
        if (picture.current) {
          const pictureRect = picture.current.getBoundingClientRect();
          const htmlRect = getHTMLClientRect();
          elementOffsetX.current = pictureRect.left - htmlRect.left;
          elementOffsetY.current = pictureRect.top - htmlRect.top;
        }
      };

      const installMouseHandlers = (): void => {
        if (fg.current) {
          fg.current.addEventListener("pointerdown", mouseDown, false);
          fg.current.addEventListener("pointerup", mouseUp, false);
          fg.current.addEventListener("pointermove", mouseDrag, false);
          const fgRect = fg.current.getBoundingClientRect();
          const htmlRect = getHTMLClientRect();
          elementOffsetX.current = fgRect.left - htmlRect.left;
          elementOffsetY.current = fgRect.top - htmlRect.top;
        }
      };

      const ctx = fg.current?.getContext("2d");
      if (ctx) {
        ctx.font = "30px Arial";
        ctx.fillStyle = "black";
        ctx.textAlign = "center";
        ctx.fillText("Loading...", ctx.canvas.width / 2, ctx.canvas.height / 2);
      }
      props.module.init();
      props.module.setCanvas(props.width, props.height);
      setInterval(doFrame, 50);
      installMouseHandlers();
    }, [props.height, props.module, props.width]);

    return (
      <div id="picture" ref={picture}>
        <canvas height={props.height} id="fg" ref={fg} width={props.width} />
        <canvas height={props.height} id="bg" ref={bg} width={props.width} />
        <div id="message" ref={messageRef} />
      </div>
    );
  }),
);
WASMModule.displayName = "WASMModule";
