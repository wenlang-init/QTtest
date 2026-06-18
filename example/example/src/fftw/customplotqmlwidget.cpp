#include "customplotqmlwidget.h"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>
#include <QDebug>
#include "customplotitem.h"
#include "fftw3object.h"
#include <QTimer>

CustomPlotQMLWidget::CustomPlotQMLWidget(QWidget *parent)
    : QWidget{parent}
{
    qw = new QQuickWidget(this);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qw->rootContext()->setContextProperty("CustomPlotQMLWidget", this);
    qmlRegisterType<CustomPlotItem>("CustomPlotItem", 1, 0, "CustomPlotItem");
    qw->setSource(QUrl("qrc:/src/fftw/customplot.qml"));
    qw->showFullScreen();
    connect(qw, &QQuickWidget::sceneGraphError, this,
            [ = ](QQuickWindow::SceneGraphError error, const QString& message) {
        qDebug() << error << message;
    });
    connect(qw, &QQuickWidget::statusChanged, this,
            [ = ](QQuickWidget::Status status) {
        qDebug() << status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->addWidget(qw);

    {
        CustomPlotItem *rect =
            qw->findChild<CustomPlotItem *>("customPlotItem");

        if (rect) {
            qDebug() << rect << rect->getCustomPlot();
        }

        QVariant ret;
        QMetaObject::invokeMethod(
            qw->rootObject(),
            "getPlotPtr",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
            );

        CustomPlotItem *item = ret.value<CustomPlotItem *>();

        if (item) {
            qDebug() << item->getOBJQV();
            qDebug() << item->getCustomPlot();
            qDebug() << item->objectName();

            // QVector<double> data;

            // fftw3Object::generateSignalData(data,
            //                                 WaveformType::Sine
            //                                 );

            // item->addData(data, 0);
            // item->refresh();
        }
    }
    {
        QTimer  *timer = new QTimer(this);
        QVariant ret;
        QMetaObject::invokeMethod(
            qw->rootObject(),
            "getPlotPtr",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
            );

        CustomPlotItem *item = ret.value<CustomPlotItem *>();

        if (item) {
            timer->start(100);
            item->setYRang(-0.5, 0.5);
            item->setShowTracer(true);
            int cnt = 2048;
            connect(timer, &QTimer::timeout, this, [ = ]() {
                QVector<double>data;
                fftw3Object::generateSignalData(data,
                                                WaveformType::Sine,
                                                cnt, 440, 0.5, 0.0, 2, 44100
                                                );
                item->addData(data, 0);
                item->setShowLastLen(cnt, 0);
                item->refresh();
                item->update(item->getCustomPlot()->rect());
            });
        }
    }
}

void CustomPlotQMLWidget::addData(QVector<double>& data)
{
    QVariant ret;

    QMetaObject::invokeMethod(
        qw->rootObject(),
        "getPlotPtr",
        Qt::DirectConnection,
        Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
        );

    CustomPlotItem *item = ret.value<CustomPlotItem *>();

    if (item) {
        item->addData(data, 0);
        item->setShowLastLen(10000, 0);

        item->refresh();
    }
}

void CustomPlotQMLWidget::addData(QVector<double>& data, double l, double r)
{
    QVariant ret;

    QMetaObject::invokeMethod(
        qw->rootObject(),
        "getPlotPtr",
        Qt::DirectConnection,
        Q_RETURN_ARG(QVariant, ret) // ,Q_ARG(QVariant, arg1)
        );

    CustomPlotItem *item = ret.value<CustomPlotItem *>();

    if (item) {
        item->clearData(0);
        item->addData(data, 0);
        item->setXRang(l, r);

        item->refresh();
    }
}
