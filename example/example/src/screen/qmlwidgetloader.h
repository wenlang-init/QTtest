#ifndef QMLWIDGETLOADER_H
#define QMLWIDGETLOADER_H

#include <QWidget>
#include "imageprovider.h"
class QQuickWidget;
class qmlWidgetLoader : public QWidget {
    Q_OBJECT

public:

    explicit qmlWidgetLoader(QWidget *parent = nullptr);
    ~qmlWidgetLoader();

protected:

    double getZoom();
    void   showImage(QImage& image);

signals:

private:

    imageProvider *imageP;
    QQuickWidget *qw;
    QVector<char>m_screenshotData;
};

#endif // QMLWIDGETLOADER_H
