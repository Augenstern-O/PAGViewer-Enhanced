/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making libpag available.
//
//  Copyright (C) 2025 Tencent. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
//  except in compliance with the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QMap>
#include <QStringList>
#include <QThread>
#include <atomic>
#include "task/PAGTask.h"
#include "task/export/PAGExportAPNGTask.h"
#include "task/export/PAGExportPNGTask.h"

namespace pag {

enum class BatchExportType { PNG_SEQUENCE, APNG };

// Structure to hold file information with relative path for batch export
struct BatchExportFileInfo {
  QString absolutePath;  // Full path to the PAG file
  QString relativePath;  // Relative path from base directory (for folder structure)
  QString baseName;      // File name without extension
};

// Helper class for batch export that doesn't open files after export
class PAGBatchExportPNGTask : public PAGExportPNGTask {
 public:
  explicit PAGBatchExportPNGTask(std::shared_ptr<PAGFile> pagFile, const QString& pngSequenceDirPath)
      : PAGExportPNGTask(std::move(pagFile), pngSequenceDirPath) {
    openAfterExport = false;
  }
};

// Helper class for batch export APNG that doesn't open files after export
class PAGBatchExportAPNGTask : public PAGExportPNGTask {
 public:
  explicit PAGBatchExportAPNGTask(std::shared_ptr<PAGFile> pagFile, const QString& apngFilePath);

 protected:
  void onFrameFlush(double progress) override;
  int onFinish() override;

 private:
  QString apngFilePath;
};

class PAGBatchExportTask : public PAGTask {
  Q_OBJECT
 public:
  explicit PAGBatchExportTask(const QStringList& inputPaths, const QString& outputDir,
                              BatchExportType exportType);
  ~PAGBatchExportTask() override;

  void start() override;
  void stop() override;

 protected:
  void onBegin() override;
  int onFinish() override;

 private:
  void startInternal() override;
  void collectPAGFiles(const QStringList& inputPaths);
  void scanDirectory(const QString& dirPath, const QString& baseDir);
  int exportSingleFile(const BatchExportFileInfo& fileInfo);

 private:
  QList<BatchExportFileInfo> pagFiles;
  QString outputDir;
  BatchExportType exportType;
  int currentFileIndex = 0;
  int totalFiles = 0;
  QThread workerThread;
  std::atomic_bool isWorking = false;
};

}  // namespace pag
