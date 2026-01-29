# PAGViewer Enhanced

> 🎨 基于 [libpag](https://github.com/Tencent/libpag) 的增强版，添加了批量导出等实用功能

[![license](https://img.shields.io/badge/license-Apache%202-blue)](LICENSE.txt)
[![based-on](https://img.shields.io/badge/based%20on-libpag-orange)](https://github.com/Tencent/libpag)

---

## ✨ 新增功能（Enhanced Features）

### 🚀 批量导出（Batch Export）

#### 批量导出文件

- ✅ 批量导出为 PNG 序列帧
- ✅ 批量导出为 APNG
- ✅ 支持多文件选择
- ✅ 显示总体导出进度

#### 批量导出文件夹

- ✅ 递归扫描所有子文件夹
- ✅ 自动保持原始文件夹结构
- ✅ 支持中文路径和特殊字符

### 🐛 Bug 修复

- ✅ 修复 APNG 导出中文路径问题
- ✅ 优化批量导出的线程安全性

---

## 📦 快速开始（Quick Start）

### 批量导出文件

1. 点击菜单：**批量导出 → 批量导出为PNG序列帧/APNG**
2. 选择一个或多个 PAG 文件
3. 选择输出目录
4. 等待导出完成

### 批量导出文件夹

1. 点击菜单：**批量导出 → 批量导出文件夹为PNG序列帧/APNG**
2. 选择输入文件夹（会递归扫描所有 PAG 文件）
3. 选择输出目录
4. 自动保持原始文件夹结构导出

---

## 📄 协议与致谢

本项目基于 [Apache License 2.0](LICENSE.txt) 开源。

- **原始项目**: [libpag](https://github.com/Tencent/libpag) by Tencent
- **原始版权**: Copyright (C) 2025 Tencent. All rights reserved.
- **修改部分**: Copyright (C) 2025 Augenstern

详细修改内容请查看 [MODIFICATIONS.md](MODIFICATIONS.md)

感谢 Tencent 开源 libpag 项目！

---

## 🔧 编译说明

请参考原项目的编译文档：[libpag Building Guide](https://github.com/Tencent/libpag#building)

---

<details>
<summary><b>📖 关于 libpag 原项目（About Original libpag Project）</b></summary>

<br>

<img src="resources/readme/logo.png" alt="PAG Logo" width="400"/>

## Introduction

libpag is a real-time rendering library for PAG (Portable Animated Graphics) files, capable of
rendering both vector-based and raster-based animations across various platforms, including iOS,
Android, OpenHarmony, macOS, Windows, Linux, and the Web.

PAG is an open-source file format designed for recording animations. You can create and export PAG
files from Adobe After Effects using the PAGExporter plugin and preview them in the PAGViewer app,
both available on macOS and Windows.

PAG is widely used in major Tencent apps like WeChat, Mobile QQ, Honor of Kings, Tencent Video, and
QQ Music, as well as in thousands of third-party apps, reaching hundreds of millions of users.

## Advantages

- **Highly efficient file format**: PAG files decode 10x faster than JSON and are ~50% smaller
- **All AE features supported**: Combines vector and raster techniques to support all AE animations
- **Measurable performance**: Comprehensive performance metrics and monitoring

更多信息请访问：[libpag GitHub](https://github.com/Tencent/libpag) | [官方网站](https://pag.io)

</details>

---

**Modified by Augenstern | Based on libpag open source project**
