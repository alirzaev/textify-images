#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <QImage>

void composeDivide(const QImage &first, QImage &second);

void applyBlur(const QImage &image, QImage &output);

#endif // IMAGE_UTILS_H
