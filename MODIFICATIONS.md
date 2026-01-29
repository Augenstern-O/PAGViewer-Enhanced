# PAGViewer 修改说明

本项目基于 [libpag](https://github.com/Tencent/libpag) 开源项目进行二次开发。

## 原始项目信息

- **项目名称**: libpag
- **原始作者**: Tencent
- **原始协议**: Apache License 2.0
- **原始仓库**: https://github.com/Tencent/libpag
- **原始版权**: Copyright (C) 2025 Tencent. All rights reserved.

## 修改者信息

- **修改者**: Augenstern
- **修改时间**: 2025年1月
- **项目仓库**: https://github.com/Augenstern-O/PAGViewer-Enhanced

## 主要修改内容

### 1. 批量导出功能

**文件**:

- `viewer/src/task/export/PAGBatchExportTask.h`
- `viewer/src/task/export/PAGBatchExportTask.cpp`
- `viewer/src/task/PAGTaskFactory.h`
- `viewer/src/task/PAGTaskFactory.cpp`

**功能**:

- 批量导出为 PNG 序列帧
- 批量导出为 APNG
- 支持多文件选择
- 显示总体导出进度
- 支持中文路径和特殊字符

### 2. 批量导出文件夹功能

**文件**:

- `viewer/src/task/export/PAGBatchExportTask.h`
- `viewer/src/task/export/PAGBatchExportTask.cpp`
- `viewer/assets/qml/Menu.qml`
- `viewer/assets/qml/Main.qml`

**功能**:

- 批量导出文件夹为 PNG 序列帧
- 批量导出文件夹为 APNG
- 递归扫描子文件夹
- 保持原始文件夹结构

### 3. Bug 修复

**文件**:

- `viewer/src/task/export/PAGExportAPNGTask.cpp`

**修复**:

- 修复了 APNG 导出时中文路径处理的 bug
- 使用 `toLocal8Bit()` 代替 `toStdString()` 以支持 Windows 中文路径

### 4. UI 改进

**文件**:

- `viewer/assets/qml/Main.qml`
- `viewer/assets/translation/Chinese.ts`

**改进**:

- 更新了关于对话框，标注基于开源项目修改
- 添加了批量导出相关的中文翻译
- 优化了菜单结构

## 协议声明

本修改版本遵循原项目的 Apache License 2.0 协议。所有修改内容同样采用 Apache License 2.0 协议发布。

原始项目的版权归 Tencent 所有，修改部分的版权归 Augenstern 所有。

## 致谢

感谢 Tencent 开源 libpag 项目，为社区提供了优秀的 PAG 动画渲染工具。
