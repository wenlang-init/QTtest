#include "mapwidget.h"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>
#include <QDebug>

MapWidget::MapWidget(QWidget *parent)
    : QWidget{parent}
{
    QQuickWidget *qw = new QQuickWidget(this);

    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qw->rootContext()->setContextProperty("MapWidget", this);
    qw->setSource(QUrl("qrc:/src/map/qml/Main.qml"));
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
}
