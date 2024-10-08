import {
  BulbOutlined,
  CheckOutlined,
  CloseOutlined,
  ExportOutlined,
  EyeInvisibleOutlined,
  EyeOutlined,
  ImportOutlined,
  LeftCircleOutlined,
  ReloadOutlined,
  RightCircleOutlined,
} from "@ant-design/icons";
import { HappyProvider } from "@ant-design/happy-work-theme";
import { Button, Card, Col, Empty, QRCode, Row, Space, Typography } from "antd";
import React, { useCallback, useEffect, useRef, useState } from "react";
import { getModule, ParsedModule, WASMModule } from "./module";

import "./editor.css";

interface Data {
  numBest: number;
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

const { Paragraph, Text } = Typography;

export function Editor(): React.JSX.Element {
  const items = new Map<string, React.ReactNode>();
  const message = useRef<HTMLDivElement>(null);
  const newTabIndex = useRef(-1);

  const [module, setModule] = useState(new ParsedModule());
  const [suggestionDisabled, setSuggestionDisabled] = useState(true);
  const [suggestion, setSuggestion] = useState<React.ReactNode>();
  const [showDetails, setShowDetails] = useState(false);
  const [isEditorOpen, setIsEditorOpen] = useState(false);
  const [tabs, setTabs] = useState<TabTitle[]>([]);
  const [tabItems, setTabItems] = useState(items);
  const [activeKey, setActiveKey] = useState("0");

  const [state, setState] = useState(false);
  const forceUpdate = useCallback(() => {
    setState(!state);
  }, [state]);

  useEffect(() => {
    setTimeout(() => {
      setSuggestionDisabled(false);
    }, 2000);
    const load = (): void => {
      getModule("EditorModule")
        .then(setModule)
        // eslint-disable-next-line no-console -- load WASM failed
        .catch(console.error);
    };
    if (!process.env.NODE_ENV || process.env.NODE_ENV === "development")
      return load;
    load();
  }, []);

  const propose = useCallback(
    (data: Data): void => {
      const result = (
        <Space direction="vertical">
          <Button
            icon={showDetails ? <EyeInvisibleOutlined /> : <EyeOutlined />}
            onClick={() => {
              setShowDetails(!showDetails);
              // eslint-disable-next-line no-console -- show details
              if (showDetails) console.log(data);
            }}
            size="small"
            type="primary"
          >
            {showDetails ? "hide" : "show"} details
          </Button>
          <Paragraph>
            <ul className="list-none-outside">
              {data.numConstraints < 4 ? (
                <li>
                  <Text type="danger">
                    <CloseOutlined style={{ color: "red" }} /> Try adding more
                    constraints
                  </Text>
                </li>
              ) : null}
              {(() => {
                if (data.numSolution === 0) {
                  return (
                    <li>
                      <Text type="danger">
                        <CloseOutlined style={{ color: "red" }} /> The puzzle is
                        unsolvable
                      </Text>
                    </li>
                  );
                } else if (data.numSolution > 10) {
                  return (
                    <li>
                      <Text type="danger">
                        <CloseOutlined style={{ color: "red" }} /> The puzzle
                        has too many solutions.
                      </Text>
                    </li>
                  );
                }
                return null;
              })()}
              {(() => {
                if (data.entropy < 3) {
                  return (
                    <li>
                      <Text type="danger">
                        <CloseOutlined style={{ color: "red" }} /> Player can
                        deduce solutions easily
                        {showDetails ? `(Entropy: ${data.entropy})` : null}
                      </Text>
                    </li>
                  );
                } else if (data.entropy > 8) {
                  return (
                    <li>
                      <Text type="success">
                        <CheckOutlined style={{ color: "green" }} /> The puzzle
                        is hard to solve
                        {showDetails ? `(Entropy: ${data.entropy})` : null}
                      </Text>
                    </li>
                  );
                }
                return null;
              })()}
              {(() => {
                if (data.advEntropy < 3) {
                  return (
                    <li>
                      <Text type="danger">
                        <CloseOutlined style={{ color: "red" }} /> Players can
                        easily notice mistakes
                        {showDetails
                          ? `(Adversarial Entropy: ${data.advEntropy})`
                          : null}
                      </Text>
                    </li>
                  );
                } else if (data.advEntropy > 8) {
                  return (
                    <li>
                      <Text type="success">
                        <CheckOutlined style={{ color: "green" }} /> Many paths
                        don&apos;t lead to goal (generally good)
                        {showDetails
                          ? `(Adversarial Entropy: ${data.advEntropy})`
                          : null}
                      </Text>
                    </li>
                  );
                }
                return null;
              })()}
            </ul>
          </Paragraph>
          {data.numBest > 0 ? (
            <Space direction="vertical">
              <Text>Show suggested puzzles</Text>
              <Space direction="horizontal">
                <Button
                  onClick={() => {
                    module.keyEvent("[");
                    forceUpdate();
                  }}
                  type="primary"
                >
                  prev
                </Button>
                <Button
                  onClick={() => {
                    module.keyEvent("]");
                    forceUpdate();
                  }}
                  type="primary"
                >
                  next
                </Button>
              </Space>
            </Space>
          ) : null}
        </Space>
      );
      setSuggestion(result);
    },
    [forceUpdate, module, showDetails],
  );

  useEffect(() => {
    if (message.current?.innerHTML) {
      try {
        const data = JSON.parse(message.current.innerHTML);
        if ("entropy" in data) propose(data as Data);
      } catch (_) {
        /* empty */
      }
    }
  }, [message.current?.innerHTML, propose]);

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
    module.keyEvent("w");
    setTimeout(() => {
      message.current?.dispatchEvent(
        new KeyboardEvent("keydown", { key: "-" }),
      );
    }, 100);
    setTimeout(() => {
      const url = message.current?.innerHTML;
      if (validate(url)) {
        const child = (
          <a href={url!} target="_blank">
            <QRCode value={url!} />
          </a>
        );
        const key = String(++newTabIndex.current);
        setTabs([...tabs, { label: `Puzzle ${key}`, key }]);
        items.set(key, child);
        setTabItems(items);
        setActiveKey(key);
        module.keyEvent("s");
      }
    }, 200);
  };

  const remove = (targetKey: TargetKey): void => {
    const index = tabs.findIndex((item) => item.key === targetKey);
    const newTabs = tabs.filter((item) => item.key !== targetKey);
    if (newTabs.length === 0) {
      setActiveKey("-1");
      items.clear();
    } else if (targetKey === activeKey) {
      const { key } = newTabs[index === newTabs.length ? index - 1 : index]!;
      setActiveKey(key);
      items.delete(targetKey);
    }
    setTabs(newTabs);
    setTabItems(items);
  };

  return (
    <HappyProvider>
      <Space
        direction="vertical"
        size="middle"
        style={{ width: "-webkit-fill-available" }}
      >
        <Row wrap={false}>
          <Col flex="920px">
            <WASMModule
              height={450}
              module={module}
              ref={message}
              width={900}
            />
          </Col>
          <Col flex="auto">
            <Card
              extra={
                <Button
                  disabled={suggestionDisabled}
                  icon={<BulbOutlined />}
                  onClick={() => {
                    module.keyEvent("z");
                    if (message.current?.innerHTML) {
                      try {
                        const data = JSON.parse(message.current.innerHTML);
                        if ("entropy" in data) propose(data as Data);
                      } catch (_) {
                        //
                      }
                    }
                  }}
                  type="link"
                >
                  Generate
                </Button>
              }
              title="Suggestions"
            >
              {suggestion ?? <Empty description={false} />}
            </Card>
          </Col>
        </Row>
        <Row wrap={false}>
          <Col flex="14rem">
            <Button
              icon={
                isEditorOpen ? <RightCircleOutlined /> : <LeftCircleOutlined />
              }
              onClick={() => {
                module.keyEvent("e");
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
                module.keyEvent("v");
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
                  if (message.current?.innerHTML) module.keyEvent("l");
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
                <Card.Meta description={tabItems.get(activeKey)} />
              )}
            </Card>
          </Col>
        </Row>
      </Space>
    </HappyProvider>
  );
}
