#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QStringListModel>

class Backend : public QObject {
  Q_OBJECT

  Q_PROPERTY(QStringList images READ images NOTIFY imagesChanged)

  Q_PROPERTY(
      qreal progress READ progress WRITE setProgress NOTIFY progressChanged)

  Q_PROPERTY(bool grayscaled READ grayscaled WRITE setGrayscaled NOTIFY
                 grayscaledChanged)

  Q_PROPERTY(bool busy READ busy WRITE setBusy NOTIFY busyChanged)

public:
  explicit Backend(QObject *parent = nullptr);

  QStringList images() const;

  Q_INVOKABLE void selectImages();

  Q_INVOKABLE void convertImages();

  qreal progress() const;

  void setProgress(qreal value);

  bool grayscaled() const;

  void setGrayscaled(bool value);

  bool busy() const;

  void setBusy(bool value);

signals:
  void imagesChanged();
  void progressChanged();
  void grayscaledChanged();
  void busyChanged();
  void convertionDone();

private:
  QStringList m_images;

  qreal m_progress;

  bool m_grayscaled;

  bool m_busy;
};

#endif // BACKEND_H
