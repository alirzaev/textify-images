#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtConcurrent>

#include "image_utils.h"

#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}, m_progress{0}, m_grayscaled{false}, m_busy{false} {}

QStringList Backend::images() const { return m_images; }

void Backend::selectImages() {
  auto locations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  auto files = QFileDialog::getOpenFileNames(
      nullptr, tr("Select images"),
      locations.empty() ? QDir::homePath() : locations.first());
  if (files.isEmpty()) {
    return;
  }

  m_images = std::move(files);
  emit imagesChanged();
}

void Backend::convertImages() {
  QTemporaryDir tmpDir;
  tmpDir.setAutoRemove(false);
  QString dir;

  if (tmpDir.isValid()) {
    dir = tmpDir.path();
  } else {
    auto location =
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    dir = QFileDialog::getExistingDirectory(
        nullptr, tr("Select folder"),
        location.isEmpty() ? QDir::homePath() : location);
  }

  setProgress(0);
  setBusy(true);

  auto future = QtConcurrent::map(
      m_images, [grayscale = m_grayscaled, dir](const auto &image) {
        auto baseName = QFileInfo(image).fileName();
        auto path = QDir::toNativeSeparators(dir + "/" + baseName);

        QImageReader reader(image);
        reader.setAutoTransform(true);

        QImage input = reader.read().convertToFormat(QImage::Format_ARGB32);
        QImage output(input.size(), input.format());

        applyBlur(input, output);
        composeDivide(input, output);
        if (grayscale) {
          output = output.convertToFormat(QImage::Format_Grayscale8);
        }

        output.save(path);
      });

  auto *watcher = new QFutureWatcher<void>(this);
  connect(watcher, &QFutureWatcher<void>::progressValueChanged, this,
          [this](int value) { setProgress(value * 1.0 / m_images.size()); });
  connect(watcher, &QFutureWatcher<void>::finished, this, [this, dir]() {
    setProgress(0);
    setBusy(false);

    emit convertionDone();
    QDesktopServices::openUrl("file:///" + dir);
  });
  watcher->setFuture(future);
}

qreal Backend::progress() const { return m_progress; }

void Backend::setProgress(qreal value) {
  if (m_progress == value) {
    return;
  }

  m_progress = value;
  emit progressChanged();
}

bool Backend::grayscaled() const { return m_grayscaled; }

void Backend::setGrayscaled(bool value) {
  if (m_grayscaled == value) {
    return;
  }

  m_grayscaled = value;
  emit grayscaledChanged();
}

bool Backend::busy() const { return m_busy; }

void Backend::setBusy(bool value) {
  if (m_busy == value) {
    return;
  }

  m_busy = value;
  emit busyChanged();
}
