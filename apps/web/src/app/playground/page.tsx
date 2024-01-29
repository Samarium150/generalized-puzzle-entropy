"use client";
import { EditOutlined, PlayCircleOutlined } from "@ant-design/icons";
import { Layout, Menu, type MenuProps, Typography } from "antd";
import Script from "next/script";
import { Playground } from "puzzle-editor";
import { type JSX, useState } from "react";

// noinspection SpellCheckingInspection
const { Header, Sider, Content, Footer } = Layout;
const { Link, Title } = Typography;

// noinspection HtmlUnknownTarget
const menuItems: MenuProps["items"] = [
  { label: <Link href="/">Editor</Link>, key: "1", icon: <EditOutlined /> },
  {
    label: <Link href="/playground">Playground</Link>,
    key: "2",
    icon: <PlayCircleOutlined />,
  },
];

export default function Page(): JSX.Element {
  const [collapsed, setCollapsed] = useState(true);
  return (
    <Layout
      className="w-screen overflow-y-visible"
      style={{ minHeight: "100vh" }}
    >
      <Header className="block text-center" style={{ height: "fit-content" }}>
        <Title style={{ color: "white", margin: "0.1em 0" }}>
          The Witness Puzzle Playground
        </Title>
      </Header>
      <Layout>
        <Sider collapsed={collapsed} collapsible onCollapse={setCollapsed}>
          <Menu
            items={menuItems}
            mode="inline"
            selectedKeys={["2"]}
            theme="dark"
          />
        </Sider>
        <Content className="px-6 py-3" style={{ width: "-webkit-max-content" }}>
          <Script src="/js/playground.js" strategy="beforeInteractive" />
          <Script src="/js/playground.worker.js" strategy="afterInteractive" />
          <Script src="/js/utils.js" strategy="afterInteractive" />
          <Playground />
        </Content>
      </Layout>
      <Footer className="my-1 h-fit p-0 text-center" style={{ padding: "0" }}>
        <Link href="https://movingai.com/" target="_blank">
          Moving AI Lab
        </Link>{" "}
        © {new Date().getFullYear()}
      </Footer>
    </Layout>
  );
}
