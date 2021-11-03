#ifndef WIDGET_H
#define WIDGET_H

#include <QImage>
#include <QStringList>
#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget {
  Q_OBJECT

public:
  explicit Widget(QWidget *parent = nullptr);

  ~Widget();

private:
  Ui::Widget *ui;

  QStringList images;

  void loadImages();

  void convertImages();

  void onWorkCompleted();

  void composeDivide(const QImage &first, QImage &second);

  void applyBlur(const QImage &image, QImage &output);
};

#endif // WIDGET_H
