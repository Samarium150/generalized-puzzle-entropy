"use client";
import { EditOutlined, PlayCircleOutlined } from "@ant-design/icons";
import { HappyProvider } from "@ant-design/happy-work-theme";
import { Flex, Layout, Menu, type MenuProps, Switch, Typography } from "antd";
import Link from "next/link";
import Script from "next/script";
import { Editor } from "@repo/puzzle-editor";
import { type JSX, useState } from "react";

// noinspection SpellCheckingInspection
const { Header, Sider, Content, Footer } = Layout;
const { Title, Paragraph, Text } = Typography;

// noinspection HtmlUnknownTarget
const menuItems: MenuProps["items"] = [
  { label: <Link href="/">Editor</Link>, key: "1", icon: <EditOutlined /> },
  {
    label: <Link href="/playground">Playground</Link>,
    key: "2",
    icon: <PlayCircleOutlined />,
  },
];

export default function Home(): JSX.Element {
  const [collapsed, setCollapsed] = useState(true);
  const [showInstructions, setShowInstructions] = useState(true);

  return (
    <Layout className="min-h-screen w-screen overflow-y-visible">
      <Header className="block h-fit text-center">
        <Title className="my-0.5 text-white">The Witness Puzzle Editor</Title>
      </Header>
      <Layout>
        <Sider collapsed={collapsed} collapsible onCollapse={setCollapsed}>
          <Menu
            items={menuItems}
            mode="inline"
            selectedKeys={["1"]}
            theme="dark"
          />
        </Sider>
        <Content className="px-6 py-3">
          <Script src="/js/editor.js" strategy="beforeInteractive" />
          <Script src="/js/editor.worker.js" strategy="afterInteractive" />
          <Script src="/js/utils.js" strategy="afterInteractive" />
          <Flex gap="small" vertical>
            <Title className="my-0.5" level={2}>
              Instructions{" "}
              <HappyProvider>
                <Switch
                  checkedChildren="shown"
                  defaultChecked={showInstructions}
                  onChange={() => {
                    setShowInstructions(!showInstructions);
                  }}
                  unCheckedChildren="hidden"
                />
              </HappyProvider>
            </Title>
            <Paragraph
              className="m-0 w-2/3"
              ellipsis={showInstructions ? false : { rows: 3 }}
            >
              <ul>
                <li>
                  Solve the puzzle normally when the editor panel is closed.
                </li>
                <li>Click the button to open or close the editor panel.</li>
                <li>
                  Rounded squares are separation constraints. Two squares of the
                  different colors should not be in the same region.
                </li>
                <li>
                  A star should be paired with exactly one other region
                  constraint of the same color regardless the type of it.
                </li>
                <li>
                  Question marks are used for letting AI suggest a good
                  region/path constraint. The maximum number of question marks
                  is 4. AI will propose suggestions by clicking the{" "}
                  <Text keyboard>Generate</Text> button on the right (the UI may
                  freeze).
                </li>
                <li>
                  Hexagons and disjoints are path constraints. The solution path
                  must be across the hexagons and not across the disjoints.
                </li>
                <li>
                  For loading a puzzle from the clipboard, you can choose a
                  puzzle from{" "}
                  <Typography.Link
                    href="https://windmill.thefifthmatt.com"
                    target="_blank"
                  >
                    The Windmill
                  </Typography.Link>{" "}
                  and click <Text keyboard>Edit</Text>, then copy the URL that
                  starts with{" "}
                  <Text code>https://windmill.thefifthmatt.com/build/</Text>
                  and ends with <Text code>_0</Text>, and finally click the{" "}
                  <Text keyboard>load</Text> button below.
                </li>
              </ul>
            </Paragraph>
            <Editor />
          </Flex>
        </Content>
      </Layout>
      <Footer className="my-1 h-fit p-0 text-center">
        <Typography.Link href="https://movingai.com/" target="_blank">
          Moving AI Lab
        </Typography.Link>{" "}
        © {new Date().getFullYear()}
      </Footer>
    </Layout>
  );
}
