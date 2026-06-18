#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QDateTime>

class imageProvider : public QQuickImageProvider {
    Q_OBJECT

public:

    explicit imageProvider();
    void   updateImage(const QImage& image);
    QImage requestImage(const QString& id,
                        QSize         *size,
                        const QSize  & requestedSize) override;

signals:

private:

    QImage m_image;
    QDateTime m_datetime = QDateTime::currentDateTime();
    int m_count = 0;
    int m_fps = 0;
    qint64 m_allcount = 0;
};

#endif // IMAGEPROVIDER_H
