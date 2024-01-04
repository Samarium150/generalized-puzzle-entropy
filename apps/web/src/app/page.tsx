"use client";
import { Flex, Layout, Switch, Typography } from "antd";
import { type JSX, useState } from "react";
import { Editor } from "puzzle-editor";
import Script from "next/script";

const { Header, Content, Footer } = Layout;
const { Title, Link, Paragraph, Text } = Typography;

export default function Home(): JSX.Element {
  const [showInstructions, setShowInstructions] = useState(true);
  // noinspection JSUnresolvedLibraryURL
  return (
    <Layout
      className="w-screen overflow-y-visible"
      style={{ minHeight: "100vh" }}
    >
      <Header className="block text-center" style={{ height: "fit-content" }}>
        <Title style={{ color: "white", margin: "0.1em 0" }}>
          The Witness Puzzle Editor
        </Title>
      </Header>
      <Content className="px-6 py-3">
        <Script
          src="https://cdn.jsdelivr.net/npm/long@3.0.1/dist/long.min.js"
          strategy="beforeInteractive"
        />
        <Script
          src="https://cdn.jsdelivr.net/npm/bytebuffer@5.0.1/dist/bytebuffer.min.js"
          strategy="beforeInteractive"
        />
        <Script
          src="https://cdn.jsdelivr.net/npm/protobufjs@5.0.1/dist/protobuf.min.js"
          strategy="beforeInteractive"
        />
        <Script src="/js/editor.js" strategy="beforeInteractive" />
        <Script src="/js/editor.worker.js" strategy="afterInteractive" />
        <Script src="/js/utils.js" strategy="afterInteractive" />
        <Flex gap="small" vertical>
          <Title level={2} style={{ margin: "0.1em 0" }}>
            Instructions{" "}
            <Switch
              checkedChildren="shown"
              defaultChecked={showInstructions}
              onChange={() => {
                setShowInstructions(!showInstructions);
              }}
              unCheckedChildren="hidden"
            />
          </Title>
          <Paragraph
            className="w-2/3"
            ellipsis={showInstructions ? false : { rows: 3 }}
            style={{ margin: 0 }}
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
                A star should be paired with exactly one other region constraint
                of the same color regardless the type of it.
              </li>
              <li>
                Hexagons and disjoints are path constraints. The solution path
                must be across the hexagons and not across the disjoints.
              </li>
              <li>
                For loading a puzzle from the clipboard, you can choose a puzzle
                from{" "}
                <Link href="https://windmill.thefifthmatt.com" target="_blank">
                  The Windmill
                </Link>{" "}
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
      <Footer className="my-1 h-fit p-0 text-center" style={{ padding: "0" }}>
        <Link href="https://movingai.com/" target="_blank">
          Moving AI Lab
        </Link>{" "}
        © {new Date().getFullYear()}
      </Footer>
    </Layout>
  );
}
