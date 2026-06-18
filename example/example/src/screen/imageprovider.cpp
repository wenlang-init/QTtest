#include "imageprovider.h"
#include <QPainter>

imageProvider::imageProvider()
    : QQuickImageProvider{QQuickImageProvider::Image}
{}

void imageProvider::updateImage(const QImage& image)
{
    m_image = image;
    QPainter painter(&m_image);

    // 设置字体和颜色
    QFont font("Arial", 30);
    painter.setFont(font);
    painter.setPen(Qt::white);
    m_allcount++;

    if (m_datetime.msecsTo(QDateTime::currentDateTime()) >= 2000) {
        m_fps = qRound(m_count * 1000.0 / m_datetime.msecsTo(
                           QDateTime::currentDateTime()));
        m_datetime = QDateTime::currentDateTime();
        m_count = 0;
    } else {
        m_count++;
    }
    QString str = QString::number(m_fps) + "fps\n" +
                  QString::number(m_allcount);

    // 绘制文本到指定位置
    painter.drawText(QPointF(50, 50), str);
}

/*
 * Image {
            id: redImage
            width: 100
            height: 100
            source: "image://test/redcircle"
        }
    名称 "test" 就是 QML 中 URL 的 provider_id
    engine.addImageProvider(QStringLiteral("test"), new imageProvider());
    requestImage的id对应redcircle
 */
QImage imageProvider::requestImage(const QString& id,
                                   QSize         *size,
                                   const QSize  & requestedSize)
{
    Q_UNUSED(id)
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)

    if (!m_image.isNull()) return m_image;

    QImage placeholder(100, 100, QImage::Format_RGB32);
    placeholder.fill(qRgb(128, 128, 128)); // 默认占位图
    return placeholder;
}
