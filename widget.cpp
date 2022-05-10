#include <algorithm>

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImageReader>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QRunnable>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>
#include <QtConcurrent>

#include "ui_widget.h"
#include "widget.h"

static unsigned char table[256][256] = {};

static unsigned int divide(unsigned int a, unsigned int b) {
  return std::min(255u, a * 256u / (b + 1u));
}

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);

  connect(ui->importImages, &QPushButton::clicked, this, &Widget::loadImages);
  connect(ui->convertImages, &QPushButton::clicked, this,
          &Widget::convertImages);

  for (unsigned int i = 0; i < 256u; ++i) {
    for (unsigned int j = 0; j < 256u; ++j) {
      table[i][j] = static_cast<unsigned char>(divide(i, j));
    }
  }
}

Widget::~Widget() { delete ui; }

void Widget::loadImages() {
  auto locations =
      QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
  auto files = QFileDialog::getOpenFileNames(
      this, tr("Select images"),
      locations.empty() ? QDir::homePath() : locations.first());
  if (files.isEmpty()) {
    return;
  }

  images = files;
  auto *list = ui->imagesList;
  list->clear();

  for (const auto &file : files) {
    list->addItem(QFileInfo(file).fileName());
  }
}

void Widget::convertImages() {
  QTemporaryDir tmpDir;
  tmpDir.setAutoRemove(false);
  QString dir;

  if (tmpDir.isValid()) {
    dir = tmpDir.path();
  } else {
    auto location =
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    dir = QFileDialog::getExistingDirectory(
        this, tr("Select folder"),
        location.isEmpty() ? QDir::homePath() : location);
  }

  bool grayscale = ui->grayscaleCheck->isChecked();

  ui->importImages->setDisabled(true);
  ui->convertImages->setDisabled(true);
  ui->grayscaleCheck->setDisabled(true);

  auto future =
      QtConcurrent::map(images, [grayscale, dir, this](const auto &image) {
        auto baseName = QFileInfo(image).fileName();
        auto path = QDir::toNativeSeparators(dir + "/" + baseName);

        QImageReader reader(image);
        reader.setAutoTransform(true);

        QImage input = reader.read();
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
          [this](int value) {
            ui->progressBar->setValue(value * 100 / images.size());
          });
  connect(watcher, &QFutureWatcher<void>::finished, this,
          &Widget::onWorkCompleted);
  connect(watcher, &QFutureWatcher<void>::finished, this,
          [dir]() { QDesktopServices::openUrl("file:///" + dir); });
  watcher->setFuture(future);
}

void Widget::onWorkCompleted() {
  ui->importImages->setDisabled(false);
  ui->convertImages->setDisabled(false);
  ui->grayscaleCheck->setDisabled(false);
  ui->progressBar->setValue(0);

  QMessageBox::information(this, tr("Processing"), tr("Done!"));
}

void Widget::composeDivide(const QImage &first, QImage &second) {
  for (int h = 0; h < first.height(); ++h) {
    auto *in = first.constScanLine(h);
    auto *out = second.scanLine(h);
    for (int w = 0; w < first.bytesPerLine(); ++w) {
      *out = table[*in][*out];
      ++in;
      ++out;
    }
  }
}

void Widget::applyBlur(const QImage &image, QImage &output) {
  const auto width = image.width();
  const auto height = image.height();
  const qreal radius = 100;

  QImage extended(width + radius * 2, height + radius * 2, image.format());
  extended.fill(Qt::transparent);

  QImage mirroredV = image.mirrored();
  QImage mirroredH = image.mirrored(true, false);
  QImage mirroredHV = image.mirrored(true, true);

  QVector<std::pair<QPointF, QImage>> drawings{
      // draw center image
      {{radius, radius}, image},

      // draw top mirrored
      {{radius, radius - height}, mirroredV},
      // draw bottom mirrored
      {{radius, radius + height}, mirroredV},
      // draw left mirrored
      {{radius - width, radius}, mirroredH},
      // draw right mirrored
      {{radius + width, radius}, mirroredH},

      // draw top left mirrored
      {{radius - width, radius - height}, mirroredHV},
      // draw top right mirrored
      {{radius + width, radius - height}, mirroredHV},
      // draw bottom left mirrored
      {{radius - width, radius + height}, mirroredHV},
      // draw bottom right mirrored
      {{radius + width, radius + height}, mirroredHV}};
  QPainter painterExt(&extended);
  for (const auto &drawing : drawings) {
    painterExt.drawImage(drawing.first, drawing.second);
  }

  auto *effect = new QGraphicsBlurEffect();
  effect->setBlurRadius(radius);

  auto *item = new QGraphicsPixmapItem();
  item->setPixmap(QPixmap::fromImage(extended));
  item->setGraphicsEffect(effect);

  QGraphicsScene scene;
  scene.addItem(item);

  QImage result(width, height, image.format());
  result.fill(Qt::transparent);

  QPainter painterRes(&output);
  scene.render(&painterRes, QRectF(), QRectF(radius, radius, width, height));
}
