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

#include "PAGBatchExportTask.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QMetaObject>
#include <QUrl>
#include "task/export/PAGExportAPNGTask.h"
#include "task/export/PAGExportPNGTask.h"
#include "utils/FileUtils.h"
#include "utils/Utils.h"

namespace pag {

// Helper function to get PNG temp directory for APNG export
static QString GetPNGTempDir(const QString& apngFilePath) {
  QFileInfo fileInfo(apngFilePath);
  QString pngFilePath = fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_PNG";
  return pngFilePath;
}

// PAGBatchExportAPNGTask implementation
PAGBatchExportAPNGTask::PAGBatchExportAPNGTask(std::shared_ptr<PAGFile> pagFile,
                                               const QString& apngFilePath)
    : PAGExportPNGTask(std::move(pagFile), GetPNGTempDir(apngFilePath)),
      apngFilePath(apngFilePath) {
  openAfterExport = false;
}

void PAGBatchExportAPNGTask::onFrameFlush(double progress) {
  PAGExportPNGTask::onFrameFlush(progress * 0.9);
}

int PAGBatchExportAPNGTask::onFinish() {
  // Use toLocal8Bit() for Windows compatibility with non-ASCII paths
  std::string outPath = std::string(apngFilePath.toLocal8Bit());
  std::string firstPNGPath = std::string(QString("%1/1.png").arg(filePath).toLocal8Bit());
  int frameRate = static_cast<int>(pagFile->frameRate());

  // qDebug() << "正在从PNG序列合成APNG:";
  // qDebug() << "  输出:" << apngFilePath;
  // qDebug() << "  首帧:" << QString("%1/1.png").arg(filePath);
  // qDebug() << "  帧率:" << frameRate;

  Utils::ExportAPNGFromPNGSequence(outPath, firstPNGPath, frameRate);
  PAGExportPNGTask::onFrameFlush(1.0);
  Utils::DeleteDir(filePath);
  // Don't call Utils::OpenInFinder in batch mode
  return PAGExportPNGTask::onFinish();
}

// PAGBatchExportTask implementation

PAGBatchExportTask::PAGBatchExportTask(const QStringList& inputPaths, const QString& outputDir,
                                       BatchExportType exportType)
    : outputDir(outputDir), exportType(exportType) {
  collectPAGFiles(inputPaths);
  totalFiles = pagFiles.size();
  qDebug() << "批量导出任务已创建，共" << totalFiles << "个文件";
  // for (int i = 0; i < pagFiles.size(); ++i) {
  //   qDebug() << "  文件" << i << ":" << pagFiles[i].absolutePath;
  //   if (!pagFiles[i].relativePath.isEmpty()) {
  //     qDebug() << "    相对路径:" << pagFiles[i].relativePath;
  //   }
  // }
  QObject::connect(&workerThread, &QThread::started, this, &PAGBatchExportTask::startInternal,
                   Qt::DirectConnection);
}

void PAGBatchExportTask::collectPAGFiles(const QStringList& inputPaths) {
  for (const QString& inputPath : inputPaths) {
    QString localPath = inputPath;
    if (localPath.startsWith("file://")) {
      localPath = QUrl(localPath).toLocalFile();
    }

    QFileInfo fileInfo(localPath);
    if (fileInfo.isFile() && fileInfo.suffix().toLower() == "pag") {
      // Single PAG file
      BatchExportFileInfo info;
      info.absolutePath = fileInfo.absoluteFilePath();
      info.relativePath = "";  // No relative path for single files
      info.baseName = fileInfo.completeBaseName();
      pagFiles.append(info);
      // qDebug() << "添加文件:" << info.absolutePath;
    } else if (fileInfo.isDir()) {
      // Directory - scan recursively
      qDebug() << "扫描目录:" << localPath;
      scanDirectory(localPath, localPath);
    }
  }
}

void PAGBatchExportTask::scanDirectory(const QString& dirPath, const QString& baseDir) {
  QDir dir(dirPath);
  if (!dir.exists()) {
    qDebug() << "目录不存在:" << dirPath;
    return;
  }

  // Get all PAG files in current directory
  QStringList filters;
  filters << "*.pag" << "*.PAG";
  QFileInfoList files = dir.entryInfoList(filters, QDir::Files);

  for (const QFileInfo& fileInfo : files) {
    BatchExportFileInfo info;
    info.absolutePath = fileInfo.absoluteFilePath();
    info.baseName = fileInfo.completeBaseName();

    // Calculate relative path from base directory
    QString relativePath = QDir(baseDir).relativeFilePath(fileInfo.absoluteFilePath());
    // qDebug() << "  基础目录:" << baseDir;
    // qDebug() << "  文件路径:" << fileInfo.absoluteFilePath();
    // qDebug() << "  相对路径:" << relativePath;

    QFileInfo relativeInfo(relativePath);
    info.relativePath = relativeInfo.path();
    if (info.relativePath == ".") {
      info.relativePath = "";
    }

    // qDebug() << "  最终相对路径:" << info.relativePath;

    pagFiles.append(info);
    // qDebug() << "添加文件:" << info.absolutePath << "相对路径:" << info.relativePath;
  }

  // Recursively scan subdirectories
  QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QFileInfo& subdirInfo : subdirs) {
    scanDirectory(subdirInfo.absoluteFilePath(), baseDir);
  }
}

PAGBatchExportTask::~PAGBatchExportTask() {
  qDebug() << "PAGBatchExportTask 析构函数被调用";

  if (workerThread.isRunning()) {
    qDebug() << "等待工作线程结束...";
    isWorking = false;
    workerThread.quit();
    workerThread.wait(5000);  // Wait up to 5 seconds

    if (workerThread.isRunning()) {
      qDebug() << "警告：工作线程未能正常结束，强制终止";
      workerThread.terminate();
      workerThread.wait();
    }
  }

  QObject::disconnect(&workerThread, &QThread::started, this, &PAGBatchExportTask::startInternal);
  qDebug() << "PAGBatchExportTask 析构完成";
}

void PAGBatchExportTask::start() {
  if (pagFiles.isEmpty()) {
    qDebug() << "没有可导出的PAG文件";
    return;
  }
  isWorking = true;
  onBegin();
  workerThread.start();
}

void PAGBatchExportTask::stop() {
  isWorking = false;
  currentFileIndex = 0;
}

void PAGBatchExportTask::onBegin() {
  visible = true;
  Q_EMIT visibleChanged(visible);
}

int PAGBatchExportTask::onFinish() {
  visible = false;
  Q_EMIT visibleChanged(visible);

  // Open output folder in main thread to avoid potential crashes
  QMetaObject::invokeMethod(qApp, [this]() {
    Utils::OpenInFinder(outputDir, false);
  }, Qt::QueuedConnection);

  return 0;
}

void PAGBatchExportTask::startInternal() {
  QString outDir = outputDir;
  if (outDir.startsWith("file://")) {
    outDir = QUrl(outDir).toLocalFile();
  }

  QDir dir(outDir);
  if (!dir.exists()) {
    qDebug() << "输出目录不存在:" << outDir;
    isWorking = false;
    int result = -1;
    Q_EMIT taskFinished(result, outDir);
    workerThread.exit(0);
    return;
  }

  for (currentFileIndex = 0; currentFileIndex < totalFiles; ++currentFileIndex) {
    if (!isWorking) {
      qDebug() << "批量导出已被用户停止";
      break;
    }

    const BatchExportFileInfo& fileInfo = pagFiles[currentFileIndex];
    QString pagFilePath = fileInfo.absolutePath;
    // qDebug() << "原始路径:" << pagFilePath;

    if (pagFilePath.startsWith("file://")) {
      pagFilePath = QUrl(pagFilePath).toLocalFile();
      // qDebug() << "转换后路径:" << pagFilePath;
    }

    qDebug() << "正在处理文件" << (currentFileIndex + 1) << "/" << totalFiles << ":"
            << pagFilePath;

    int result = exportSingleFile(fileInfo);
    if (result != 0) {
      qDebug() << "导出文件失败:" << pagFilePath << "错误代码:" << result;
    }
  }

  isWorking = false;
  qDebug() << "批量导出完成，共处理" << totalFiles << "个文件";

  int result = onFinish();
  currentFileIndex = 0;

  qDebug() << "发送任务完成信号";
  Q_EMIT taskFinished(result, outDir);

  qDebug() << "工作线程退出";
  workerThread.exit(0);
}

int PAGBatchExportTask::exportSingleFile(const BatchExportFileInfo& fileInfo) {
  QString localPath = fileInfo.absolutePath;
  if (localPath.startsWith("file://")) {
    localPath = QUrl(localPath).toLocalFile();
  }

  // Check if file exists
  QFileInfo checkFile(localPath);
  if (!checkFile.exists()) {
    qDebug() << "文件不存在:" << localPath;
    return -1;
  }

  // Use toLocal8Bit() for Windows compatibility with non-ASCII paths
  std::string strPath = std::string(localPath.toLocal8Bit());
  auto pagFile = PAGFile::Load(strPath);
  if (!pagFile) {
    qDebug() << "加载PAG文件失败:" << localPath;
    return -1;
  }

  QString outDir = outputDir;
  if (outDir.startsWith("file://")) {
    outDir = QUrl(outDir).toLocalFile();
  }

  // qDebug() << "导出文件信息:";
  // qDebug() << "  文件名:" << fileInfo.baseName;
  // qDebug() << "  相对路径:" << fileInfo.relativePath;
  // qDebug() << "  输出目录:" << outDir;

  // Create output directory with folder structure
  QString outputPath = outDir;
  if (!fileInfo.relativePath.isEmpty()) {
    outputPath = outDir + "/" + fileInfo.relativePath;
    // qDebug() << "  创建子目录:" << outputPath;
    QDir().mkpath(outputPath);
  }

  PAGPlayTask* exportTask = nullptr;
  int taskResult = 0;

  if (exportType == BatchExportType::APNG) {
    // Export as APNG
    QString apngPath = outputPath + "/" + fileInfo.baseName + ".png";
    // qDebug() << "导出APNG到:" << apngPath;

    exportTask = new PAGBatchExportAPNGTask(pagFile, apngPath);
  } else {
    // Export as PNG sequence
    QString pngDir = outputPath + "/" + fileInfo.baseName;
    // qDebug() << "导出PNG序列到:" << pngDir;

    Utils::MakeDir(pngDir);

    exportTask = new PAGBatchExportPNGTask(pagFile, pngDir);
  }

  // Connect progress signals
  QObject::connect(exportTask, &PAGTask::progressChanged, this, [this](double progress) {
    double totalProgress = (currentFileIndex + progress) / totalFiles;
    this->progress = totalProgress;
    Q_EMIT progressChanged(totalProgress);
  });

  // Use QEventLoop to wait for task completion
  QEventLoop loop;
  QObject::connect(exportTask, &PAGTask::taskFinished, &loop, [&loop, &taskResult](int result, QString) {
    // qDebug() << "任务完成信号已接收，结果:" << result;
    taskResult = result;
    loop.quit();
  });

  // qDebug() << "启动导出任务...";
  exportTask->start();

  // qDebug() << "等待任务完成...";
  loop.exec();

  // qDebug() << "任务已完成，结果:" << taskResult;
  delete exportTask;
  return taskResult;
}

}  // namespace pag
