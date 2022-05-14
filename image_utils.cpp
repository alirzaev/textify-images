#include <algorithm>

#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QVector>

#include "image_utils.h"

static unsigned int divide(unsigned int a, unsigned int b) {
  return std::min(255u, a * 256u / (b + 1u));
}

void composeDivide(const QImage &first, QImage &second) {
  for (int h = 0; h < first.height(); ++h) {
    auto *in = first.constScanLine(h);
    auto *out = second.scanLine(h);
    for (int w = 0; w < first.bytesPerLine(); ++w) {
      *out = divide(*in, *out);
      ++in;
      ++out;
    }
  }
}

void applyBlur(const QImage &image, QImage &output) {
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
