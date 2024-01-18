import { AntdRegistry } from "@ant-design/nextjs-registry";
import type { Metadata } from "next";
import { Inter } from "next/font/google";
import Script from "next/script";
import React from "react";

import "./globals.css";

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "The Witness Puzzle Editor",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>): React.JSX.Element {
  // noinspection JSUnresolvedLibraryURL
  return (
    <html lang="en">
      <body className={inter.className} suppressHydrationWarning>
        <AntdRegistry>{children}</AntdRegistry>
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
      </body>
    </html>
  );
}
