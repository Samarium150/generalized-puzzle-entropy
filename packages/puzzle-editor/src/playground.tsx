import { EyeOutlined, ImportOutlined, ReloadOutlined } from "@ant-design/icons";
import { Button, Col, Row, Space } from "antd";
import { type JSX, useEffect, useRef, useState } from "react";
import { ParsedModule, getModule, WASMModule } from "./module";

export function Playground(): JSX.Element {
  const message = useRef<HTMLDivElement>(null);

  const [module, setModule] = useState(new ParsedModule());

  useEffect(() => {
    getModule("PlaygroundModule")
      .then(setModule)
      // eslint-disable-next-line no-console -- load WASM failed
      .catch(console.error);
  }, []);

  return (
    <Space
      direction="vertical"
      size="middle"
      style={{ width: "-webkit-fill-available", alignItems: "center" }}
    >
      <WASMModule height={450} module={module} ref={message} width={450} />
      <Row align="middle" gutter={[8, 0]} justify="center">
        <Col>
          <Button
            icon={<ReloadOutlined />}
            onClick={() => {
              module.keyEvent("r");
            }}
            shape="round"
            size="large"
            type="primary"
          >
            Reset
          </Button>
        </Col>
        <Col>
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
            type="primary"
          >
            Load from clipboard
          </Button>
        </Col>
        <Col>
          <Button
            icon={<EyeOutlined />}
            onClick={() => {
              module.keyEvent("v");
            }}
            shape="round"
            size="large"
            type="primary"
          >
            Show a solution
          </Button>
        </Col>
      </Row>
    </Space>
  );
}
