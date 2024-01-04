"use client";
import {
  BulbOutlined,
  ExportOutlined,
  ImportOutlined,
  LeftCircleOutlined,
  ReloadOutlined,
  RightCircleOutlined,
} from "@ant-design/icons";
import { Button, Card, Col, Empty, Row, Space, Typography } from "antd";
import React, { useEffect, useRef, useState } from "react";
import { getModule } from "./module";

import "./editor.css";

interface Data {
  numConstraints: number;
  numSolution: number;
  entropy: number;
  advEntropy: number;
}

interface TabTitle {
  key: string;
  label: React.ReactNode;
}

type TargetKey = string | React.MouseEvent | React.KeyboardEvent;
type TabContent = Map<string, React.ReactNode>;

const { Link, Paragraph, Text } = Typography;

export function Editor(): React.JSX.Element {
  const picture = useRef<HTMLDivElement>(null);
  const bg = useRef<HTMLCanvasElement>(null);
  const fg = useRef<HTMLCanvasElement>(null);
  const message = useRef<HTMLDivElement>(null);

  const htmlClientRect = (): DOMRect => {
    return document.getElementsByTagName("html")[0].getBoundingClientRect();
  };

  const init = useRef<() => null>();
  const frame = useRef<() => string>();
  const mouseEvent =
    useRef<
      (x: number, y: number, up: boolean, down: boolean, drag: boolean) => null
    >();
  const keyEvent = useRef<(key: string) => null>();
  const setCanvas = useRef<(width: number, height: number) => null>();

  const down = useRef(false);
  const elementOffsetX = useRef(0);
  const elementOffsetY = useRef(0);

  const newTabIndex = useRef(-1);

  const [suggestionDisabled, setSuggestionDisabled] = useState(true);
  const [suggestion, setSuggestion] = useState<React.ReactNode>();
  const [isEditorOpen, setIsEditorOpen] = useState(false);
  const [tabs, setTabs] = useState<TabTitle[]>([]);
  const [items, setItems] = useState<TabContent>(new Map());
  const [activeKey, setActiveKey] = useState("0");

  const mouseDown = (event: { clientX: number; clientY: number }): void => {
    if (mouseEvent.current) {
      down.current = true;
      const x = event.clientX + window.scrollX - elementOffsetX.current;
      const y = event.clientY + window.scrollY - elementOffsetY.current;
      mouseEvent.current(x, y, false, true, false);
    }
  };

  const mouseUp = (event: { clientX: number; clientY: number }): void => {
    if (mouseEvent.current) {
      down.current = false;
      const x = event.clientX + window.scrollX - elementOffsetX.current;
      const y = event.clientY + window.scrollY - elementOffsetY.current;
      mouseEvent.current(x, y, true, false, false);
    }
  };

  const mouseDrag = (event: {
    isPrimary: boolean;
    clientX: number;
    clientY: number;
  }): void => {
    if (mouseEvent.current) {
      if (event.isPrimary && down.current) {
        const x = event.clientX + window.scrollX - elementOffsetX.current;
        const y = event.clientY + window.scrollY - elementOffsetY.current;
        mouseEvent.current(x, y, false, true, true);
      }
      if (!down.current) {
        const x = event.clientX + window.scrollX - elementOffsetX.current;
        const y = event.clientY + window.scrollY - elementOffsetY.current;
        mouseEvent.current(x, y, false, false, true);
      }
    }
  };

  useEffect(() => {
    const doFrame = (): void => {
      if (frame.current) {
        frame.current();
        if (picture.current) {
          const pictureRect = picture.current.getBoundingClientRect();
          const htmlRect = htmlClientRect();
          elementOffsetX.current = pictureRect.left - htmlRect.left;
          elementOffsetY.current = pictureRect.top - htmlRect.top;
        }
      }
    };

    const installMouseHandlers = (): void => {
      if (fg.current) {
        fg.current.addEventListener("pointerdown", mouseDown, false);
        fg.current.addEventListener("pointerup", mouseUp, false);
        fg.current.addEventListener("pointermove", mouseDrag, false);

        const fgRect = fg.current.getBoundingClientRect();
        const htmlRect = htmlClientRect();
        elementOffsetX.current = fgRect.left - htmlRect.left;
        elementOffsetY.current = fgRect.top - htmlRect.top;
      }
    };

    getModule()
      .then((module) => {
        init.current = module.cwrap("InitHOG", null, []);
        frame.current = module.cwrap("DoFrame", "string", []);
        mouseEvent.current = module.cwrap("MouseEvent", null, [
          "number",
          "number",
          "boolean",
          "boolean",
          "boolean",
        ]);
        keyEvent.current = module.cwrap("HitKey", null, ["string"]);
        setCanvas.current = module.cwrap("SetCanvasSize", null, [
          "number",
          "number",
        ]);

        init.current();
        setCanvas.current(900, 450);
        setInterval(doFrame, 50);
        installMouseHandlers();
        setTimeout(() => {
          setSuggestionDisabled(false);
        }, 20000);
      })
      .catch((error) => {
        // eslint-disable-next-line no-alert -- load WASM failed
        window.alert(error);
      });
  }, []);

  const propose = (data: Data): void => {
    const result = (
      <Paragraph>
        <ul>
          {data.numConstraints < 4 ? (
            <li>
              <Text type="danger">Try adding more constraints</Text>
            </li>
          ) : null}
          {(() => {
            if (data.numSolution === 0) {
              return (
                <li>
                  <Text type="danger">The puzzle is unsolvable</Text>
                </li>
              );
            } else if (data.numSolution > 10) {
              return (
                <li>
                  <Text type="danger">The puzzle has too many solutions.</Text>
                </li>
              );
            }
            return null;
          })()}
          {(() => {
            if (data.entropy < 3) {
              return (
                <li>
                  <Text type="danger">Player can deduce solutions easily</Text>
                </li>
              );
            } else if (data.entropy > 8) {
              return (
                <li>
                  <Text type="success">The puzzle is hard to solve</Text>
                </li>
              );
            }
            return null;
          })()}
          {(() => {
            if (data.advEntropy < 3) {
              return (
                <li>
                  <Text type="danger">Players can easily notice mistakes</Text>
                </li>
              );
            } else if (data.advEntropy > 8) {
              return (
                <li>
                  <Text type="success">
                    Many paths don&apos;t lead to goal (generally good)
                  </Text>
                </li>
              );
            }
            return null;
          })()}
        </ul>
      </Paragraph>
    );
    setSuggestion(result);
  };

  const validate = (url: string | undefined): boolean => {
    if (!url) return false;
    try {
      const i = new URL(url);
      // noinspection SpellCheckingInspection
      return (
        i.protocol === "https:" && i.hostname === "windmill.thefifthmatt.com"
      );
    } catch (_) {
      return false;
    }
  };

  const add = (): void => {
    keyEvent.current?.("w");
    setTimeout(() => {
      message.current?.dispatchEvent(
        new KeyboardEvent("keydown", { key: "-" }),
      );
    }, 100);
    setTimeout(() => {
      const url = message.current?.innerHTML;
      if (validate(url)) {
        const children = (
          <Link
            copyable
            ellipsis
            href={url}
            style={{ height: 20 }}
            target="_blank"
          >
            {url}
          </Link>
        );
        const key = String(++newTabIndex.current);
        setTabs([...tabs, { label: `Puzzle ${key}`, key }]);
        setItems({ ...items, [key]: children });
        setActiveKey(key);
      }
    }, 200);
  };

  const remove = (targetKey: TargetKey): void => {
    const index = tabs.findIndex((item) => item.key === targetKey);
    const newTabs = tabs.filter((item) => item.key !== targetKey);
    if (newTabs.length === 0) {
      setActiveKey("-1");
      setItems(new Map());
    } else if (targetKey === activeKey) {
      const { key } = newTabs[index === newTabs.length ? index - 1 : index];
      setActiveKey(key);
      const newItems = new Map(items);
      newItems.delete(targetKey);
      setItems(newItems);
    }
    setTabs(newTabs);
  };

  return (
    <Space direction="vertical" size="middle">
      <Row>
        <Col flex="950px">
          <div id="picture" ref={picture}>
            <canvas height={450} id="fg" ref={fg} width={900} />
            <canvas height={450} id="bg" ref={bg} width={900} />
            <div id="message" ref={message} />
          </div>
        </Col>
        <Col flex="auto">
          <Card
            extra={
              <Button
                disabled={suggestionDisabled}
                icon={<BulbOutlined />}
                onClick={() => {
                  keyEvent.current?.("z");
                  if (message.current?.innerHTML) {
                    propose(JSON.parse(message.current.innerHTML) as Data);
                  }
                }}
                type="link"
              >
                Generate
              </Button>
            }
            style={{ width: "100%" }}
            title="Suggestions"
          >
            {suggestion ?? <Empty description={false} />}
          </Card>
        </Col>
      </Row>
      <Row>
        <Col flex="14rem">
          <Button
            icon={
              isEditorOpen ? <RightCircleOutlined /> : <LeftCircleOutlined />
            }
            onClick={() => {
              keyEvent.current?.("e");
              setIsEditorOpen(!isEditorOpen);
            }}
            shape="round"
            size="large"
            style={{ width: "13rem" }}
            type="primary"
          >
            {`${isEditorOpen ? "Close" : "Open "} editor panel`}
          </Button>
        </Col>
        <Col flex="14rem">
          <Button
            icon={<ReloadOutlined rotate={90} />}
            onClick={() => {
              keyEvent.current?.("v");
            }}
            shape="round"
            size="large"
            style={{ width: "13rem" }}
            type="primary"
          >
            Show a solution
          </Button>
        </Col>
        <Col flex="14rem">
          <Button
            icon={<ImportOutlined />}
            onClick={() => {
              message.current?.dispatchEvent(
                new KeyboardEvent("keydown", { key: "+" }),
              );
              setTimeout(() => {
                if (message.current?.innerHTML) keyEvent.current?.("l");
              }, 200);
            }}
            shape="round"
            size="large"
            style={{ width: "fit-content" }}
            type="primary"
          >
            Load from clipboard
          </Button>
        </Col>
        <Col flex="14rem">
          <Button
            icon={<ExportOutlined />}
            onClick={add}
            shape="round"
            size="large"
            style={{ width: "fit-content" }}
            type="primary"
          >
            Export current puzzle
          </Button>
        </Col>
      </Row>
      <Row>
        <Col span={14}>
          <Card
            activeTabKey={activeKey}
            onTabChange={setActiveKey}
            tabList={tabs.length > 0 ? tabs : undefined}
            tabProps={
              tabs.length > 0
                ? {
                    type: "editable-card",
                    hideAdd: true,
                    size: "small",
                    onEdit: remove,
                  }
                : undefined
            }
            title="Exported Puzzles"
          >
            {tabs.length === 0 ? (
              <Empty
                description={false}
                image={Empty.PRESENTED_IMAGE_SIMPLE}
                style={{ height: 20 }}
              />
            ) : (
              <Card.Meta description={items.get(activeKey)} />
            )}
          </Card>
        </Col>
      </Row>
    </Space>
  );
}
