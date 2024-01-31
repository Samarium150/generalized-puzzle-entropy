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
    <Layout className="min-h-screen w-screen overflow-y-visible">
      <Header className="block h-fit text-center">
        <Title className="my-0.5 text-white">
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
        <Content className="w-max px-6 py-3">
          <Script src="/js/playground.js" strategy="beforeInteractive" />
          <Script src="/js/playground.worker.js" strategy="afterInteractive" />
          <Script src="/js/utils.js" strategy="afterInteractive" />
          <Playground />
        </Content>
      </Layout>
      <Footer className="my-1 h-fit p-0 text-center">
        <Link href="https://movingai.com/" target="_blank">
          Moving AI Lab
        </Link>{" "}
        © {new Date().getFullYear()}
      </Footer>
    </Layout>
  );
}
