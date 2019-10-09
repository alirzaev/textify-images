#include <algorithm>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QRunnable>
#include <QString>
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
  auto files = QFileDialog::getOpenFileNames(this);
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
  auto dir = QFileDialog::getExistingDirectory(this);

  bool grayscale = ui->grayscaleCheck->isChecked();

  ui->importImages->setDisabled(true);
  ui->convertImages->setDisabled(true);
  ui->grayscaleCheck->setDisabled(true);

  auto future =
      QtConcurrent::map(images, [grayscale, dir, this](const auto &image) {
        auto baseName = QFileInfo(image).fileName();
        auto path = QDir::toNativeSeparators(dir + "/" + baseName);

        QImage input(image);
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
  watcher->setFuture(future);
}

void Widget::onWorkCompleted() {
  ui->importImages->setDisabled(false);
  ui->convertImages->setDisabled(false);
  ui->grayscaleCheck->setDisabled(false);
  ui->progressBar->setValue(0);

  QMessageBox::information(this, "Конвертирование", "Готово!");
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
  auto *effect = new QGraphicsBlurEffect();
  effect->setBlurRadius(100);

  auto *item = new QGraphicsPixmapItem();
  item->setPixmap(QPixmap::fromImage(image));
  item->setGraphicsEffect(effect);

  QGraphicsScene scene;
  scene.addItem(item);

  output.fill(Qt::transparent);

  QPainter painter(&output);
  scene.render(&painter, QRectF(), QRectF(0, 0, image.width(), image.height()));
}
