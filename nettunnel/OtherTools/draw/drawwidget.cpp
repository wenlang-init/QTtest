#include "drawwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QImage>
#include "drawobject.h"

drawWidget::drawWidget(QWidget *parent)
    : QWidget{parent}
{}

void drawWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(Qt::red);
    painter.setBrush(Qt::yellow);

    QImage image;
    int    w = width() < 400 ? width() : 400;
    int    h = height() < 400 ? height() : 400;
    QByteArray data(w * h, 0);
#if 0
    {
        QByteArray imagedata(w * h * 3, 0);

        for (int i = 0; i < DRAWTYPE::DRAWTYPE_END; i++) {
            drawObject::getGraphData(data.data(), w, h, (DRAWTYPE)i);

            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    int v = data[j * w + i];

                    if (v) {
                        imagedata[(j * w + i) * 3 + 0] = rand() % 255; // R
                        imagedata[(j * w + i) * 3 + 1] = rand() % 255; // G
                        imagedata[(j * w + i) * 3 + 2] = rand() % 255; // B
                    } else {
                        imagedata[(j * w + i) * 3 + 0] = 0;            // R
                        imagedata[(j * w + i) * 3 + 1] = 0;            // G
                        imagedata[(j * w + i) * 3 + 2] = 0;            // B
                    }
                }
            }
            image =
                QImage((uchar *)imagedata.data(), w, h, QImage::Format_RGB888);
            image.save(QString("D:\\imagesave_%1.png").arg(i));
        }
    }
    drawObject::getGraphData(data.data(), w, h, DRAWTYPE_PENTAGRAM);
    QByteArray imagedata(w * h * 3, 0);

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            int v = data[j * w + i];

            if (v) {
                imagedata[(j * w + i) * 3 + 0] = rand() % 255; // B
                imagedata[(j * w + i) * 3 + 1] = rand() % 255; // G
                imagedata[(j * w + i) * 3 + 2] = rand() % 255; // R
            } else {
                imagedata[(j * w + i) * 3 + 0] = 0;            // B
                imagedata[(j * w + i) * 3 + 1] = 0;            // G
                imagedata[(j * w + i) * 3 + 2] = 0;            // R
            }
        }
    }
    image = QImage((uchar *)imagedata.data(), w, h, QImage::Format_BGR888);
#endif // if 1
    drawObject::getGraphQImage(image, w, h, DRAWTYPE_PENTAGRAM);
    painter.drawImage(rect(), image);
    painter.drawEllipse(width() / 4, height() / 4, width() / 2, height() / 2);
    QWidget::paintEvent(event);
}
